namespace FroggerRemake;

/// Forwards a held direction to the original ROM latch. The ROM itself allows
/// only one hop until release; stick hysteresis avoids accidental releases and
/// re-presses when an axis jitters near the engagement threshold.
public sealed class InputPulse
{
    private const int Up=1,Down=2,Left=4,Right=8;
    private int analogMask;
    public void Reset()=>analogMask=0;
    public int FromInputs(int digitalMask,float stickX,float stickY){
        int previous=analogMask;
        analogMask=0;
        if(stickY<((previous&Up)!=0?-.30f:-.55f))analogMask|=Up;
        if(stickY>((previous&Down)!=0?.30f:.55f))analogMask|=Down;
        if(stickX<((previous&Left)!=0?-.30f:-.55f))analogMask|=Left;
        if(stickX>((previous&Right)!=0?.30f:.55f))analogMask|=Right;
        return FromHeldMask(digitalMask|analogMask);
    }
    public int FromHeldMask(int mask){
        mask&=Up|Down|Left|Right;
        if((mask&Up)!=0)return Up;
        if((mask&Down)!=0)return Down;
        if((mask&Left)!=0)return Left;
        if((mask&Right)!=0)return Right;
        return 0;
    }
}
