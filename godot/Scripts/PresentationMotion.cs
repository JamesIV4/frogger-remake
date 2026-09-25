using System;
using System.Collections.Generic;
namespace FroggerRemake;

/// Reconstructs continuous visual lane motion from the ROM's byte positions.
/// Gameplay and collision keep reading the unchanged native coordinates.
public sealed class PresentationMotion
{
    private sealed class Track(float position,int frame) {
        public float Native=position,Display=position,Velocity;
        public int Frame=frame;
        public readonly Queue<(int Frame,float X)> History=new(new[]{(frame,position)});
    }
    private readonly Dictionary<int,Track> tracks=new();
    private readonly int historyFrames;
    private readonly float contactTolerance;
    private readonly float correctionRate;
    public PresentationMotion(int historyFrames=32,float contactTolerance=1f,float correctionRate=24f){
        this.historyFrames=Math.Max(2,historyFrames);
        this.contactTolerance=Math.Clamp(contactTolerance,.25f,2f);
        this.correctionRate=Math.Clamp(correctionRate,1f,120f);
    }

    public void Reset()=>tracks.Clear();
    public float? DisplayX(int id)=>tracks.TryGetValue(id,out var track)?Wrap(track.Display):null;
    public float VelocityX(int id)=>tracks.TryGetValue(id,out var track)?track.Velocity:0;
    public float Step(int id,float nativeX,int nativeFrame,float renderSeconds,bool paused=false) {
        if(!tracks.TryGetValue(id,out var track)){
            tracks[id]=new Track(nativeX,nativeFrame);return Wrap(nativeX);
        }
        int elapsed=nativeFrame-track.Frame;
        if(elapsed<0||elapsed>24){tracks[id]=new Track(nativeX,nativeFrame);return Wrap(nativeX);}
        if(elapsed>0){
            float advance=nativeX-track.Native;
            advance-=256f*MathF.Round(advance/256f);
            if(Math.Abs(advance)>12f){tracks[id]=new Track(nativeX,nativeFrame);return Wrap(nativeX);}
            track.Native+=advance;track.Frame=nativeFrame;
            track.History.Enqueue((nativeFrame,track.Native));
            while(track.History.Count>historyFrames)track.History.Dequeue();
            var oldest=track.History.Peek();
            if(nativeFrame>oldest.Frame){
                float desired=(track.Native-oldest.X)/(nativeFrame-oldest.Frame);
                track.Velocity+=(desired-track.Velocity)*(1-MathF.Exp(-.55f*elapsed));
            }
        }
        if(!paused)track.Display+=track.Velocity*Math.Clamp(renderSeconds/(float)ArcadeSimulation.FrameSeconds,0,1.5f);
        float error=track.Native-track.Display;
        if(Math.Abs(error)>4)track.Display=track.Native;
        else if(Math.Abs(error)>contactTolerance){
            float correction=error-MathF.CopySign(contactTolerance,error);
            track.Display+=correction*Math.Clamp(renderSeconds*correctionRate,0,1);
        }
        // Keep the unwrapped accumulator precise after repeated board cycles.
        if(track.Display>=512||track.Display< -256){
            float shift=256f*MathF.Floor(track.Display/256f);
            track.Display-=shift;track.Native-=shift;
            var history=track.History.ToArray();track.History.Clear();
            foreach(var sample in history)track.History.Enqueue((sample.Frame,sample.X-shift));
        }
        return Wrap(track.Display);
    }
    private static float Wrap(float x){x%=256f;return x<0?x+256f:x;}
}
