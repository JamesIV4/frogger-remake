using System;
namespace FroggerRemake;

/// Decodes presentation state from the original tile page; no independent hazard timers.
public static class BoardVisuals
{
    public const float LeftEdge=8,RightEdge=232;
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
    public static bool GatorOnLog(FrameState state,float x,int row) {
        if(state.At(0x83b7)<2)return false;
        for(int dx=-24;dx<=24;dx+=8)for(int col=0;col<2;col++){
            int tile=TileAt(state,x+dx,row,col);if(tile>=0x68&&tile<=0x6b||tile>=0xd0&&tile<=0xd3)return true;
        }
        return false;
    }
}
