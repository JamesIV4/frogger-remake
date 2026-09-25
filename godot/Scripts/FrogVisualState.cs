using System;
namespace FroggerRemake;

/// Presentation lifecycle sampled once per original frame, with no writes to the game.
public sealed class FrogVisualState
{
    public bool Dying { get; private set; }
    public bool Drowning { get; private set; }
    public bool Carrying { get; private set; }
    public int DeathFrame { get; private set; }
    public int DeathX { get; private set; }
    public int DeathRow { get; private set; }
    public void Reset(){Dying=Drowning=Carrying=false;DeathFrame=0;}
    public static bool PlayerOnBoard(FrameState s)=>s.At(0x8044)>=8&&s.At(0x8044)<=240&&s.At(0x8047)>=26&&s.At(0x8047)<=232;
    public void Observe(FrameState s) {
        bool dead=PlayerOnBoard(s)&&s.At(0x8004)!=0&&s.At(0x83cd)==0;
        if(dead&&!Dying){DeathFrame=s.frame;DeathX=s.At(0x8044);DeathRow=s.At(0x8047);Drowning=s.At(0x829c)!=0;}
        Dying=dead;
        Carrying=!dead&&PlayerOnBoard(s)&&s.At(0x8134)!=0&&s.At(0x8135)!=0;
    }
    public double DeathSeconds(int frame,float fraction)=>Math.Max(0,frame-DeathFrame+fraction)*ArcadeSimulation.FrameSeconds;
    private static float Ease(double t){float p=(float)Math.Clamp(t,0,1);return p*p*(3-2*p);}
    public static float SquashProgress(double seconds)=>Ease(seconds/.11);
    public static float DrownDepth(double seconds)=>1.15f*Ease(seconds/(64.0/60.0));
}
