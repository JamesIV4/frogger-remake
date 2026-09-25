using System;
using System.Collections.Generic;
using System.Security.Cryptography;
using System.Text.Json;
using System.Linq;
namespace FroggerRemake;

/// Native C# execution of the statically recovered program at the original board clock.
public sealed class ArcadeSimulation : IDisposable
{
    public const double FrameSeconds=33.0/2000.0;
    public const int CyclesPerFrame=50688;
    public readonly FroggerBus Bus;
    public readonly MainProgram Cpu;
    public readonly NativeSound? Sound;
    public bool ModernCollision;
    public int Frame { get; private set; }
    private long nextNmi;
    private int previousX,previousY;
    public ArcadeSimulation(byte[] rom,bool modern=false,byte[]? soundRom=null) {
        if(Convert.ToHexStringLower(SHA256.HashData(rom))!="f8c0a2ef4105769c627b7bbf13d0844ada8bbe43c00d177eb90dd395d1e3a1e5")
            throw new ArgumentException("ROM differs from recovered program. Run prepare_rom.py with the supplied set.");
        Bus=new FroggerBus(rom);Cpu=new MainProgram(Bus);ModernCollision=modern;
        if(soundRom!=null){Sound=new NativeSound(soundRom);Bus.SoundCommand=value=>{Sound.AdvanceTo(Cpu.Cycles*NativeSound.Clock/3072000);Sound.Command(value);};}
    }
    public void Step(int buttons=0) {
        Bus.Input=buttons;long target=(Frame+1L)*CyclesPerFrame;
        while(Cpu.Cycles<target) {
            if(Cpu.Cycles>=nextNmi) {nextNmi+=CyclesPerFrame;previousX=Peek(0x8044);previousY=Peek(0x8047);if(Bus.NmiEnabled)Cpu.Nmi();}
            if(ModernCollision&&Cpu.PC==0x11bf&&ResolveModernRoad())Cpu.ReturnFromHook();else Cpu.StepInstruction();
        }
        Frame++;
        Sound?.AdvanceTo(Cpu.Cycles*NativeSound.Clock/3072000);
    }
    public void Modern(bool enabled)=>ModernCollision=enabled;
    public int Peek(int address)=>Bus.Read(address);
    public void Poke(int address,int value)=>Bus.Write(address,value);
    public FrameState Snapshot() {
        var s=new FrameState{frame=Frame,ram=Array.ConvertAll(Bus.Ram,b=>(int)b),video=Array.ConvertAll(Bus.Video,b=>(int)b),objects=Array.ConvertAll(Bus.Objects,b=>(int)b),sounds=Bus.Sounds.ToArray()};
        Bus.Sounds.Clear();return s;
    }
    public byte[] StateArray()=>Bus.Ram.Concat(Bus.Video).Concat(Bus.Objects).ToArray();
    public string StateBytes()=>JsonSerializer.Serialize(Array.ConvertAll(StateArray(),b=>(int)b));
    public void Dispose() { }
    // Intentional ROAD-only modernization of ROM 0x11bf/0x12e4. The original
    // kill latch, river support, score, timers and level transitions still own play.
    private bool ResolveModernRoad() {
        int y=Peek(0x8047),x=Peek(0x8044);if(y<136||y>216)return false;
        if(Peek(0x83cd)!=0||Peek(0x8004)!=0)return true;
        for(int lane=9;lane<=13;lane++) {
            int width=lane==9?34:18,table=0x8100+(lane-3)*9;
            for(int i=0;i<Math.Min(8,Peek(table));i++) {
                double cx=Peek(table+1+i)-3-width/2.0;
                for(int wrap=-256;wrap<=256;wrap+=256)
                    if(SweptBox(previousX,previousY,x,y,cx+wrap,lane*16,width/2.0-1.5,5.7)){Poke(0x8004,1);return true;}
            }
        }
        return true;
    }
    public static bool SweptBox(double x0,double y0,double x1,double y1,double cx,double cy,double rx,double ry) {
        double near=0,far=1;
        foreach(var (p,d,c,r) in new[]{(x0,x1-x0,cx,rx),(y0,y1-y0,cy,ry)}) {
            if(Math.Abs(d)<1e-9){if(p<c-r||p>c+r)return false;}
            else {double a=(c-r-p)/d,b=(c+r-p)/d;if(a>b)(a,b)=(b,a);near=Math.Max(near,a);far=Math.Min(far,b);if(near>far)return false;}
        }
        return true;
    }
}
public sealed class FrameState {
    public int frame{get;set;}
    public int[] ram{get;set;}=new int[2048];
    public int[] video{get;set;}=new int[1024];
    public int[] objects{get;set;}=new int[256];
    public int[] sounds{get;set;}=Array.Empty<int>();
    public int At(int a)=>a>=0xa800?video[(a-0xa800)&1023]:ram[(a-0x8000)&2047];
    public int BcdScore(int a)=>10*((At(a)&15)+10*(At(a)>>4)+100*(At(a+1)&15)+1000*(At(a+1)>>4));
}
public sealed class FroggerBus : IArcadeBus {
    public readonly byte[] Rom,Ram=new byte[2048],Video=new byte[1024],Objects=new byte[256];
    public readonly List<int> Sounds=new();
    public bool NmiEnabled,FlipX,FlipY;
    public int Input,SoundData,SoundControl;
    public Action<int>? SoundCommand;
    private readonly Ppi inputPpi,soundPpi;
    public FroggerBus(byte[] rom) {
        Rom=rom;
        inputPpi=new Ppi(p=>p switch {
            0=>255&~(((Input&4)!=0?32:0)|((Input&8)!=0?16:0)|((Input&16)!=0?128:0)),
            1=>252&~(((Input&32)!=0?128:0)|((Input&64)!=0?64:0)),
            _=>241&~(((Input&1)!=0?16:0)|((Input&2)!=0?64:0))},(_,_)=>{});
        soundPpi=new Ppi(p=>p==2?0:255,(p,v)=>{
            if(p==0)SoundData=v;
            if(p==1){if((SoundControl&8)!=0&&(v&8)==0){Sounds.Add(SoundData);SoundCommand?.Invoke(SoundData);}SoundControl=v;}
        });
    }
    public int Read(int a) {
        a&=65535;
        if(a<0x4000)return Rom[a];
        if(a>=0x8000&&a<0x8800)return Ram[a-0x8000];
        if((a&0xf800)==0xa800)return Video[a&1023];
        if((a&0xf800)==0xb000)return Objects[a&255];
        if(a>=0xc000){int v=255,p=(a>>1)&3;if((a&0x1000)!=0)v&=soundPpi.Read(p);if((a&0x2000)!=0)v&=inputPpi.Read(p);return v;}
        return 255;
    }
    public void Write(int a,int value) {
        a&=65535;byte v=(byte)value;
        if(a<0x4000)throw new InvalidOperationException($"Write to ROM {a:x4}");
        if(a>=0x8000&&a<0x8800){Ram[a-0x8000]=v;return;}
        if((a&0xf800)==0xa800){Video[a&1023]=v;return;}
        if((a&0xf800)==0xb000){Objects[a&255]=v;return;}
        switch(a&0xf81c){case 0xb808:NmiEnabled=(v&1)!=0;return;case 0xb80c:FlipY=(v&1)!=0;return;case 0xb810:FlipX=(v&1)!=0;return;}
        if(a>=0xc000){int p=(a>>1)&3;if((a&0x1000)!=0)soundPpi.Write(p,v);if((a&0x2000)!=0)inputPpi.Write(p,v);}
    }
    public int In(int port)=>255;
    public void Out(int port,int value) { }
}
internal sealed class Ppi(Func<int,int> input,Action<int,int> output) {
    private int control=0x9b;
    private readonly int[] latch=new int[3];
    public int Read(int p)=>p switch {0=>(control&16)!=0?input(0):latch[0],1=>(control&2)!=0?input(1):latch[1],
        2=>(((control&8)!=0?input(2):latch[2])&240)|(((control&1)!=0?input(2):latch[2])&15),_=>control};
    public void Write(int p,int v) {
        if(p==3){if((v&128)!=0){control=v;Array.Clear(latch);if((control&16)==0)output(0,0);if((control&2)==0)output(1,0);output(2,0);}
            else{int bit=(v>>1)&7;latch[2]=(latch[2]&~(1<<bit))|((v&1)<<bit);output(2,latch[2]);}}
        else if(p==2||(p==0?(control&16)==0:(control&2)==0)){latch[p]=v;output(p,v);}
    }
}
