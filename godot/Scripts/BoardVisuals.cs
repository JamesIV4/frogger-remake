using System;
namespace FroggerRemake;

/// Decodes presentation state from the original tile page; no independent hazard timers.
public static class BoardVisuals
{
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
