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
    public int HopDirection { get; private set; }
    public int HopStartFrame { get; private set; }
    private int previousHop;
    public void Reset(){Dying=Drowning=Carrying=false;DeathFrame=0;HopDirection=HopStartFrame=previousHop=0;}
    public static bool PlayerOnBoard(FrameState s)=>s.At(0x8044)>=8&&s.At(0x8044)<=240&&s.At(0x8047)>=26&&s.At(0x8047)<=240;
    public void Observe(FrameState s) {
        // ROM 0x28BB sets the same HOLD_FLAG for a safe crocodile-back ride
        // and for a fatal snout hit. The river death flag and position split
        // distinguish the two; a safe ride must not play the drown animation.
        bool ridingGator=s.At(0x8004)!=0&&BoardVisuals.RiverGatorRide(s);
        bool dead=PlayerOnBoard(s)&&s.At(0x8004)!=0&&s.At(0x83cd)==0&&!ridingGator;
        if(dead&&!Dying){DeathFrame=s.frame;DeathX=s.At(0x8044);DeathRow=s.At(0x8047);Drowning=s.At(0x829c)!=0;}
        Dying=dead;
        Carrying=!dead&&PlayerOnBoard(s)&&s.At(0x8134)!=0&&s.At(0x8135)!=0;
        int hop=0;for(int i=0;i<4;i++)if(s.At(0x8248+i)!=0)hop=i+1;
        if(hop!=0&&hop!=previousHop){HopDirection=hop;HopStartFrame=s.frame;}
        previousHop=hop;
    }
    public bool HopActive(int frame)=>!Dying&&HopDirection!=0&&frame-HopStartFrame<10;
    public double HopSeconds(int frame,float fraction)=>Math.Clamp((frame-HopStartFrame+fraction)/10.0,0,1)*(10.0/60.0);
    public double DeathSeconds(int frame,float fraction)=>Math.Max(0,frame-DeathFrame+fraction)*ArcadeSimulation.FrameSeconds;
    private static float Ease(double t){float p=(float)Math.Clamp(t,0,1);return p*p*(3-2*p);}
    public static float SquashProgress(double seconds)=>Ease(seconds/.11);
    // The ROM can latch death in the middle of a sideways log hop. Sink at
    // once so that a stopped horizontal hop reads as drowning, not hovering.
    public const float MaximumDrownDepth=1.15f;
    public static float DrownDepth(double seconds)=>MaximumDrownDepth*(1-(float)Math.Exp(-Math.Max(0,seconds)/.25));
    public static float DrownHeight(float surfaceHeight,float initialHeight,double seconds){
        float floor=surfaceHeight-MaximumDrownDepth;
        float remaining=Math.Max(0,initialHeight-floor);
        return initialHeight-remaining*DrownDepth(seconds)/MaximumDrownDepth;
    }
}

/// A brief presentation hold after the ROM awards a home. The machine keeps
/// running normally; movement input cancels the visual hold immediately.
public sealed class HomeArrivalVisual
{
    public const double HoldSeconds=.25;
    public const double HopCompletionSeconds=6*ArcadeSimulation.FrameSeconds;
    private const double HopClipSeconds=10.0/60.0;
    private int startFrame=-1;
    public int X {get;private set;}
    public int Row {get;private set;}
    public int TargetX {get;private set;}
    public bool Passenger {get;private set;}
    private double startHopPoseSeconds;
    public void Reset(){startFrame=-1;Passenger=false;}
    public void Cancel()=>Reset();
    public void Begin(BonusAward timeAward,bool passenger,double hopPoseSeconds=0){
        if(timeAward.Kind!=BonusKind.Time)throw new ArgumentException("Expected a home time award",nameof(timeAward));
        startFrame=timeAward.Frame;X=timeAward.X;Row=timeAward.Row;Passenger=passenger;
        int bay=Math.Clamp((int)Math.Round((X-24)/48.0),0,4);TargetX=24+48*bay;
        startHopPoseSeconds=Math.Clamp(hopPoseSeconds,0,HopClipSeconds);
    }
    private double Elapsed(int frame,float fraction)=>Math.Max(0,frame-startFrame+fraction)*ArcadeSimulation.FrameSeconds;
    private double Completion=>Row>32?HopCompletionSeconds:0;
    public bool Active(int frame,float fraction=0)=>startFrame>=0&&frame>=startFrame&&
        Elapsed(frame,fraction)<Completion+HoldSeconds;
    public bool FinishingHop(int frame,float fraction=0)=>Active(frame,fraction)&&Elapsed(frame,fraction)<Completion;
    private float Progress(int frame,float fraction){
        if(Completion==0)return 1;
        float p=(float)Math.Clamp(Elapsed(frame,fraction)/Completion,0,1);
        return p*p*(3-2*p);
    }
    public float VisualRow(int frame,float fraction=0)=>Row+(32-Row)*Progress(frame,fraction);
    public float VisualX(int frame,float fraction=0)=>X+(TargetX-X)*Progress(frame,fraction);
    public double HopPoseSeconds(int frame,float fraction=0)=>startHopPoseSeconds+
        (HopClipSeconds-startHopPoseSeconds)*Progress(frame,fraction);
}
