// Native arithmetic/control-flow support for the statically recovered programs.
// Flag semantics cross-checked against the separately frozen arcade-js oracle.
using System;
using System.Collections.Generic;

namespace FroggerRemake;
public interface IArcadeBus
{
    int Read(int address);
    void Write(int address, int value);
    int In(int port);
    void Out(int port, int value);
}

public abstract class Z80Program
{
    protected readonly IArcadeBus Bus;
    public int PC, AFAlt, BCAlt, DEAlt, HLAlt, InterruptMode, EiDelay;
    public bool Iff1, Iff2, Halted;
    public long Cycles;
    public bool[] Executed = new bool[65536];
    public readonly HashSet<int> IndirectTargets=new();
    private int a,f=0x40,b,c,d,e,h,l,ix=65535,iy=65535,sp,i,r;
    public int A { get=>a; set=>a=value&255; } public int F { get=>f; set=>f=value&255; }
    public int B { get=>b; set=>b=value&255; } public int C { get=>c; set=>c=value&255; }
    public int D { get=>d; set=>d=value&255; } public int E { get=>e; set=>e=value&255; }
    public int H { get=>h; set=>h=value&255; } public int L { get=>l; set=>l=value&255; }
    public int I { get=>i; set=>i=value&255; } public int R { get=>r; set=>r=value&255; }
    public int IX { get=>ix; set=>ix=value&65535; } public int IY { get=>iy; set=>iy=value&65535; }
    public int SP { get=>sp; set=>sp=value&65535; }
    public int AF { get=>(A<<8)|F; set { A=value>>8;F=value; } }
    public int BC { get=>(B<<8)|C; set { B=value>>8;C=value; } }
    public int DE { get=>(D<<8)|E; set { D=value>>8;E=value; } }
    public int HL { get=>(H<<8)|L; set { H=value>>8;L=value; } }
    public int IXH { get=>IX>>8; set=>IX=(IX&255)|((value&255)<<8); }
    public int IXL { get=>IX&255; set=>IX=(IX&65280)|(value&255); }
    public int IYH { get=>IY>>8; set=>IY=(IY&255)|((value&255)<<8); }
    public int IYL { get=>IY&255; set=>IY=(IY&65280)|(value&255); }
    protected Z80Program(IArcadeBus bus) { Bus=bus; }
    protected abstract int ExecuteInstruction();
    public void StepInstruction() {
        if (Halted) { Cycles+=4; return; }
        Executed[PC]=true; R=(R&128)|((R+1)&127);
        Cycles+=ExecuteInstruction(); if(EiDelay>0) EiDelay--;
    }
    public void Nmi() { Halted=false; Iff2=Iff1; Iff1=false; Push(PC); PC=0x66; Cycles+=11; }
    public bool Irq() {
        if(!Iff1 || EiDelay>0) return false;
        Halted=false; Iff1=Iff2=false; Push(PC);PC=InterruptMode==2?Read16((I<<8)|255):0x38;Cycles+=13;return true;
    }
    protected Exception Missing()=>new InvalidOperationException($"Unrecovered execution address 0x{PC:x4}");
    public void ReturnFromHook() { PC=Pop();Cycles+=10; }
    protected int Read8(int address)=>Bus.Read(address&65535);
    protected void Write8(int address,int value)=>Bus.Write(address&65535,value&255);
    protected int Read16(int address)=>Read8(address)|(Read8(address+1)<<8);
    protected void Write16(int address,int value) { Write8(address,value); Write8(address+1,value>>8); }
    protected int PortRead(int port)=>Bus.In(port&65535);
    protected void PortWrite(int port,int value)=>Bus.Out(port&65535,value&255);
    protected void Push(int value) { SP-=2;Write16(SP,value); }
    protected int Pop() { int v=Read16(SP);SP+=2;return v; }
    protected static int Parity(int v) { v^=v>>4;v^=v>>2;v^=v>>1;return (v&1)==0?4:0; }
    protected static int Sz(int v)=>(v&0xa8)|(v==0?64:0);
    protected void Add(int v,int carry=0) { int old=A,res=old+v+carry;A=res;F=Sz(A)|(res>255?1:0)|((old^v^A)&16)|((~(old^v)&(old^A)&128)!=0?4:0); }
    protected void Adc(int v)=>Add(v,F&1);
    protected void Sub(int v,int carry=0) { int old=A,res=old-v-carry;A=res;F=Sz(A)|2|(res<0?1:0)|((old^v^A)&16)|(((old^v)&(old^A)&128)!=0?4:0); }
    protected void Sbc(int v)=>Sub(v,F&1);
    protected void Cp(int v) { int old=A;Sub(v);F=(F&~0x28)|(v&0x28);A=old; }
    protected void And(int v) { A&=v;F=Sz(A)|16|Parity(A); }
    protected void Or(int v) { A|=v;F=Sz(A)|Parity(A); }
    protected void Xor(int v) { A^=v;F=Sz(A)|Parity(A); }
    protected int Inc8(int v) { int res=(v+1)&255;F=(F&1)|Sz(res)|((res&15)==0?16:0)|(res==128?4:0);return res; }
    protected int Dec8(int v) { int res=(v-1)&255;F=(F&1)|Sz(res)|2|((res&15)==15?16:0)|(res==127?4:0);return res; }
    protected void Bit(int n,int v,int yx) { bool set=(v&(1<<n))!=0;F=(F&1)|16|(set?0:68)|(n==7&&set?128:0)|(yx&40); }
    protected int Add16(int old,int v) { int res=old+v;F=(F&196)|(res>65535?1:0)|(((old^v^res)>>8)&16)|((res>>8)&40);return res&65535; }
    protected void AdcHl(int v) { int old=HL,res=old+v+(F&1);HL=res;F=((HL>>8)&168)|(HL==0?64:0)|(((old^HL^v)>>8)&16)|(((old^HL)&(v^HL)&32768)!=0?4:0)|(res>65535?1:0); }
    protected void SbcHl(int v) { int old=HL,res=old-v-(F&1);HL=res;F=((HL>>8)&168)|(HL==0?64:0)|(((old^HL^v)>>8)&16)|(((old^v)&(old^HL)&32768)!=0?4:0)|2|(res<0?1:0); }
    protected int Rotate(int v,int mode) {
        int carry=(mode%2==0?v>>7:v&1)&1;
        int res=mode switch {0=>(v<<1)|(v>>7),1=>(v>>1)|(v<<7),2=>(v<<1)|(F&1),3=>(v>>1)|((F&1)<<7),4=>v<<1,5=>(v>>1)|(v&128),6=>(v<<1)|1,_=>v>>1};
        res&=255;F=Sz(res)|Parity(res)|carry;return res;
    }
    protected void RotateA(int mode) { int keep=F&196;A=Rotate(A,mode);F=keep|(F&1)|(A&40); }
    protected void Daa() {
        int old=A,correction=0,carry=F&1;
        if((F&16)!=0||(A&15)>9)correction|=6;
        if(carry!=0||A>153){correction|=96;carry=1;}
        int res=((F&2)!=0?A-correction:A+correction)&255;
        F=Sz(res)|Parity(res)|(F&2)|carry|(((F&2)!=0?((F&16)!=0&&(old&15)<6):(old&15)>9)?16:0);A=res;
    }
    protected void Block(bool down,bool compare) {
        int v=Read8(HL),delta=down?-1:1;HL+=delta;BC--;
        if(compare) {int res=(A-v)&255,half=(A^v^res)&16,yx=(res-(half!=0?1:0))&255;F=(F&1)|(res&128)|(res==0?64:0)|half|(BC!=0?4:0)|2|(((yx<<4)|(yx&15))&40);}
        else {Write8(DE,v);DE+=delta;int sum=(A+v)&255;F=(F&193)|(BC!=0?4:0)|(sum&8)|((sum&2)!=0?32:0);}
    }
    protected void NibbleRotate(bool left) {
        int v=Read8(HL),old=A;
        if(left){A=(A&240)|(v>>4);Write8(HL,(v<<4)|(old&15));}
        else {A=(A&240)|(v&15);Write8(HL,((old&15)<<4)|(v>>4));}
        F=(F&1)|Sz(A)|Parity(A);
    }
    protected void BlockIo(bool input,bool down) {
        int delta=down?-1:1,v;
        if(input){v=PortRead(BC);Write8(HL,v);B--;}
        else {v=Read8(HL);B--;PortWrite(BC,v);}
        HL+=delta;int k=v+(input?((C+delta)&255):L);
        F=Sz(B)|((v&128)!=0?2:0)|(k>255?17:0)|Parity((k&7)^B);
    }
}
