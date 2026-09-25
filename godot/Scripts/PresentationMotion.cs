using System;
using System.Collections.Generic;
namespace FroggerRemake;

/// Continuous visual positions for the ROM's byte-stepped moving objects.
/// The original RAM remains the sole gameplay and collision authority.
public sealed class PresentationMotion
{
    private readonly Dictionary<int,float> positions=new();
    public void Reset()=>positions.Clear();
    public float Step(int id,float nativeX,float renderSeconds) {
        if(!positions.TryGetValue(id,out float shown)){
            positions[id]=nativeX;return nativeX;
        }
        float distance=nativeX-shown;
        distance-=256f*MathF.Round(distance/256f);
        if(Math.Abs(distance)>12f){positions[id]=nativeX;return nativeX;}
        // Keep the display within roughly half a native pixel of a steadily
        // moving car, so visual contact remains aligned with ROM collision.
        float blend=1-MathF.Exp(-78f*Math.Clamp(renderSeconds,0,.1f));
        shown+=distance*blend;
        shown%=256f;if(shown<0)shown+=256f;
        positions[id]=shown;
        return shown;
    }
}
