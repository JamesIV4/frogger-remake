using System;
namespace FroggerRemake;

public readonly record struct RiverGatorFit(float CenterOffsetPixels,float WidthScale,float LengthScale);
public enum RiverGatorZone { Outside, Back, Snout }

/// Decodes presentation state from the original tile page; no independent hazard timers.
public static class BoardVisuals
{
    public const float LeftEdge=8,RightEdge=232;
    public const int RiverGatorVisiblePixels=57,RiverGatorDangerPixels=16;
    public static RiverGatorZone RiverGatorContact(int frogX,int tipX){
        int behind=(tipX-frogX+256)&255;
        return behind<RiverGatorDangerPixels?RiverGatorZone.Snout:
            behind<=RiverGatorVisiblePixels?RiverGatorZone.Back:RiverGatorZone.Outside;
    }
    public static bool RiverGatorActive(FrameState state)=>
        state.At(0x83b7)>=2&&(state.At(0x8150)&1)!=0&&state.At(0x8101)!=0;
    public static bool RiverGatorRide(FrameState state)=>
        RiverGatorActive(state)&&state.At(0x829c)==0&&
        ((state.At(0x8047)+8)&255)>=42&&((state.At(0x8047)+8)&255)<59&&
        RiverGatorContact(state.At(0x8044),state.At(0x8101))==RiverGatorZone.Back;
    public const int LadyFrogRow=96;
    public static float LadyFrogHeight(float scale)=>
        -.18f+ModelFootprints.LogTopTiles-scale*ModelFootprints.LadyBottomTiles+.012f;
    public static RiverGatorFit FitRiverGator(int nativeWidth){
        float lengthScale=(nativeWidth-3f)/(16f*ModelFootprints.RiverGatorLengthTiles);
        float widthScale=14f/(16f*ModelFootprints.RiverGatorWidthTiles);
        // Native object X is the tip. The lethal region is [X-16, X].
        float centerOffset=nativeWidth/2f+12f-16f*ModelFootprints.RiverGatorFrontTiles*lengthScale;
        return new RiverGatorFit(centerOffset,widthScale,lengthScale);
    }
    public static bool IntersectsPlayfield(float center,float halfWidth)=>center+halfWidth>=LeftEdge&&center-halfWidth<=RightEdge;
    public static float InterpolateByte(int previous,int current,float alpha) {
        int d=((current-previous+128)&255)-128;
        return Math.Abs(d)>8?current:previous+d*Math.Clamp(alpha,0,1);
    }
    public static float SurfaceHeight(float row) {
        if(row>=216)return Mix(.02f,.075f,(row-216)/8);
        if(row>=136)return .02f;
        if(row>=128)return Mix(.075f,.02f,(row-128)/8);
        if(row>=112)return Mix(.20f,.075f,(row-112)/16);
        if(row>=48)return .20f;
        return Mix(.08f,.20f,(row-32)/16);
    }
    private static float Mix(float a,float b,float t)=>a+(b-a)*Math.Clamp(t,0,1);
    private static float Smooth(float t){t=Math.Clamp(t,0,1);return t*t*(3-2*t);}
    // The home gator's ROM tiles switch from emerging to full at phase 0x50.
    // Move the 3D model through the rear hedge during that existing interval.
    public static float HomeGatorReveal(FrameState state,bool full,float fraction=0)=>
        full?1:Smooth((state.At(0x8122)+Math.Clamp(fraction,0,1))/80f);
    // Actual ROM phase clocks, including the pre-dive warning. Finish going
    // under BEFORE the fatal interval; do not ease toward an already-fatal state.
    public static float TurtleDepth(FrameState state,float x,int row,float alpha=0,bool knownDiver=false) {
        if(!knownDiver&&TurtlePhase(state,x,row)==0)return 0;
        float p=state.At(row==64?0x8110:0x8111)+(row==64?1:2)*alpha;
        if(row==64) {
            if(p<80)return 0;
            // Use the whole ROM warning interval for the visible descent.
            if(p<160)return .70f*Smooth((p-80)/80);
            if(p<176)return .70f;
            return .70f*(1-Smooth((p-176)/80));
        }
        // Same 80-unit descent and rise on the faster row.
        if(p<80)return .70f*Smooth(p/80);
        if(p<96)return .70f;
        if(p<176)return .70f*(1-Smooth((p-96)/80));
        return 0;
    }
    public static int TileAt(FrameState state,float frogSpaceX,int frogSpaceRow,int columnOffset=0) {
        int col=(frogSpaceRow/8+columnOffset)&31;
        int raw=state.objects[col*2],scroll=((raw>>4)|(raw<<4))&255;
        int nativeY=(248-(int)MathF.Round(frogSpaceX)+scroll)&255;
        return state.video[(nativeY>>3)*32+col];
    }
    public static int TurtlePhase(FrameState state,float x,int row) {
        bool surface=false,bubbles=false,blank=true;
        foreach(int offset in new[]{-4,0,4})for(int col=0;col<2;col++){
            int tile=TileAt(state,x+offset,row,col);
            surface|=tile>=0x70&&tile<=0x87;
            bubbles|=tile>=0x94&&tile<=0x9b;
            blank&=tile==0x10;
        }
        // ROM tables 0x2190/0x2194 and 0x2231/0x2235 are the bubble quads;
        // 0x2198 and 0x2239 replace all four cells with 0x10 at full submergence.
        return surface?0:bubbles?1:blank?2:0;
    }
}

/// Presentation-only pink-frog position. The ROM's fly-bonus teardown erases
/// its shared sprite descriptor while leaving the pickup latch armed, so the
/// separate patrol table is authoritative until the descriptor is valid again.
public sealed class LadyFrogPresentation
{
    private float? lastX;
    public bool Active {get;private set;}
    public float X {get;private set;}
    public bool Visible(float halfWidth)=>Active&&BoardVisuals.IntersectsPlayfield(X,halfWidth);
    public void Reset(){Active=false;lastX=null;X=0;}
    public void Observe(FrameState state,Func<int,int> readRom){
        Active=state.At(0x83fe)!=0&&state.At(0x8135)!=0&&state.At(0x8134)==0;
        if(!Active){lastX=null;return;}
        int code=state.At(0x8041),color=state.At(0x8042),row=state.At(0x8043);
        bool patrolSprite=(code is 0x1e or 0x21 or 0xa1)&&color==4&&row==BoardVisuals.LadyFrogRow;
        int x;
        if(patrolSprite)x=state.At(0x8040);
        else {
            int index=(state.At(0x833d)&0x7f)+1;
            int offset=readRom(0x279f+index);
            x=offset>=2?(offset+state.At(0x811c))&255:lastX.HasValue?(int)lastX.Value:state.At(0x8040);
        }
        X=x;lastX=x;
    }
}

/// Keeps the home gator visible only long enough to slide back behind its hedge.
/// The ROM's tile and collision state remains authoritative.
public sealed class HomeGatorVisual
{
    public const float RetreatFrames=12;
    private float retreatStart=-1,lastReveal;
    private bool wasActive;
    public bool Retreating=>retreatStart>=0;
    public void Reset(){retreatStart=-1;lastReveal=0;wasActive=false;}
    public float? Reveal(float frame,bool active,float nativeReveal){
        if(active){wasActive=true;retreatStart=-1;lastReveal=nativeReveal;return nativeReveal;}
        if(wasActive){wasActive=false;retreatStart=frame;}
        if(retreatStart<0)return null;
        float t=Math.Clamp((frame-retreatStart)/RetreatFrames,0,1);
        if(t>=1){retreatStart=-1;return null;}
        float smooth=t*t*(3-2*t);
        return lastReveal*(1-smooth);
    }
}
