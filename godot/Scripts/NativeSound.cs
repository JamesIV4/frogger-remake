using System;
using System.Collections.Generic;
using System.Security.Cryptography;
namespace FroggerRemake;

/// The second ROM is statically compiled to SoundProgram.cs. Its own routines
/// select tunes, sequence notes, mix effects, and write the AY registers.
/// Only the physical PSG/analogue output is represented by a synthesis model.
public sealed class NativeSound : IArcadeBus
{
    public const double Clock=14318181.0/8;
    public readonly SoundProgram Cpu;
    public readonly byte[] Ram=new byte[1024];
    public readonly int[] Registers=new int[16];
    public readonly Queue<float> Samples=new();
    public readonly List<(long cycle,int register,int value)> Writes=new();
    private readonly byte[] rom;
    private readonly double[] tone=new double[3];
    private long sampleNumber;
    private int selected,latch,lfsr=1,envelopeStep=15,envelopeDirection=-1;
    private double noisePhase,envelopePhase,filtered,previous;
    private bool irq,envelopeHeld;
    public bool RecordWrites;
    public NativeSound(byte[] data) {
        if(Convert.ToHexStringLower(SHA256.HashData(data))!="77cee6c5fc88c666f7d41a9d94a92a96bf44800981e537eb5bc626e3485d5935")throw new ArgumentException("Sound ROM checksum mismatch");
        rom=data;Cpu=new SoundProgram(this);
    }
    public void Command(int value){latch=value;irq=true;}
    public void AdvanceTo(double cycles){
        long end=(long)(cycles*48000/Clock);
        while(sampleNumber<end){
            double target=(sampleNumber+1)*Clock/48000;
            while(Cpu.Cycles<target){if(irq&&Cpu.Irq())irq=false;Cpu.StepInstruction();}
            Samples.Enqueue(Synthesize());sampleNumber++;
        }
    }
    public int Read(int address){int a=address&0x7fff;if(a<0x2000)return a<rom.Length?rom[a]:0;if(a>=0x4000&&a<0x6000)return Ram[a&1023];return 255;}
    public void Write(int address,int value){int a=address&0x7fff;if(a>=0x4000&&a<0x6000)Ram[a&1023]=(byte)value;}
    public int In(int port){
        if((port&64)==0)return 255;
        if(selected==14)return latch;
        if(selected==15){long c=(Cpu.Cycles*8)%40960;int hi=0;if(c>=20480){hi=128;c-=20480;}
            int v=hi|(int)((c>>8)&64)|(int)((c>>8)&32)|(int)((c>>7)&16)|14;
            return (v&~40)|((v&8)<<2)|((v&32)>>2);
        }
        return Registers[selected];
    }
    public void Out(int port,int value){
        if((port&64)!=0){
            Registers[selected]=value;if(RecordWrites)Writes.Add((Cpu.Cycles,selected,value));
            if(selected==13){envelopeDirection=(value&4)!=0?1:-1;envelopeStep=envelopeDirection==1?0:15;envelopePhase=0;envelopeHeld=false;}
        } else if((port&128)!=0)selected=value&15;
    }
    private float Synthesize(){
        noisePhase+=Clock/(16*48000*Math.Max(1,Registers[6]&31));
        while(noisePhase>=1){noisePhase--;int bit=(lfsr^(lfsr>>3))&1;lfsr=(lfsr>>1)|(bit<<16);}
        int period=Registers[11]|(Registers[12]<<8);
        envelopePhase+=Clock/(256*48000*Math.Max(1,period));
        while(envelopePhase>=1&&!envelopeHeld){
            envelopePhase--;envelopeStep+=envelopeDirection;
            if(envelopeStep<0||envelopeStep>15){
                int shape=Registers[13];
                if((shape&8)==0){envelopeStep=0;envelopeHeld=true;}
                else if((shape&1)!=0){envelopeStep=(shape&2)!=0?(envelopeDirection>0?0:15):(envelopeDirection>0?15:0);envelopeHeld=true;}
                else{if((shape&2)!=0)envelopeDirection=-envelopeDirection;envelopeStep=envelopeDirection>0?0:15;}
            }
        }
        double sum=0;
        for(int ch=0;ch<3;ch++){
            int p=Registers[2*ch]|((Registers[2*ch+1]&15)<<8);tone[ch]=(tone[ch]+Clock/(16*48000*Math.Max(1,p)))%1;
            bool gate=((Registers[7]&(1<<ch))!=0||tone[ch]<.5)&&((Registers[7]&(8<<ch))!=0||(lfsr&1)!=0);
            int volume=(Registers[8+ch]&16)!=0?envelopeStep:Registers[8+ch]&15;
            if(gate&&volume>0)sum+=Math.Pow(10,(volume-15)*1.5/20);
        }
        // DC blocker followed by modest analogue smoothing; not a netlist-exact amplifier model.
        double raw=sum*.23,high=raw-previous+.995*filtered;previous=raw;filtered=high;
        return (float)Math.Clamp(high,-1,1);
    }
}
