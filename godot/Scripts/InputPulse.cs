namespace FroggerRemake;

/// Turns a held direction into successive complete ROM hops. The original
/// directional latch needs one released NMI before it will accept another hop.
/// This changes only the input sampling; the recovered movement remains exact.
public static class InputPulse
{
    public static int ForHeldDirection(int direction,FrameState state) {
        int counter=direction switch {1=>0x8250,2=>0x8251,4=>0x8253,8=>0x8252,_=>0};
        if(counter==0)return direction;
        bool moving=false;for(int i=0;i<4;i++)moving|=state.At(0x8248+i)!=0;
        return !moving&&state.At(counter)!=0?0:direction;
    }
}
