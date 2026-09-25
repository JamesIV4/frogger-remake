using Godot;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
namespace FroggerRemake;

// Bounded, deterministic in-engine visual fixtures. Only enabled with BOTH
// --screenshot=<path> and --review=<case>; they never save player preferences.
public partial class FroggerGame
{
    private string review="";
    private int reviewFrame;
    private bool reviewCapturePending;
    private bool reviewClose;
    private bool reviewLadyClose;
    private bool reviewTurtleClose;
    private readonly List<object> reviewMeasurements=new();
    private int[] ReviewFrames=>review switch {
        "carry" or "carry-left"=>new[]{1,5,9,13,25},
        "lady-move"=>new[]{1,8,17,25,30,40,45},
        "snake"=>new[]{1,8},
        "bottom-grass"=>new[]{1,8},
        "hop-right"=>new[]{1,6,12,24,48},
        "log-left" or "log-right"=>new[]{1,4,8,12,16,20,24,32,40,48},
        "turtle-rider"=>new[]{1,25,50,75,100,125,150,175,200,225,250},
        "squash"=>new[]{1,4,8,16,32,48,72,92},
        "drown"=>new[]{1,8,16,32,48,64,76},
        "bonus"=>new[]{1,13,25,50,95,115},
        "wrap"=>new[]{1,2,8,24,48,72,96,120},
        "turtle"=>new[]{1,25,50,75,100,125,150,175,200,225,250},
        "perspective" or "perspective-follow" or "ortho-follow"=>new[]{1,40},
        _=>new[]{10}
    };
    private void ReadReviewArgs() {
        if(screenshot=="")return;
        foreach(var arg in OS.GetCmdlineUserArgs())if(arg.StartsWith("--review="))review=arg[9..];
        if(review is "perspective" or "perspective-follow")perspectiveView=true;
        if(review is "perspective-follow" or "ortho-follow")followCamera=true;
        reviewClose=Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--review-close");
        reviewLadyClose=Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--review-lady-close");
        reviewTurtleClose=Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--review-turtle-close");
    }
    private void SetReviewFrog(int x,int row) {
        simulation.Poke(0x8044,x);simulation.Poke(0x8047,row);simulation.Poke(0x8004,0);simulation.Poke(0x829c,0);simulation.Poke(0x83cd,0);
        simulation.Poke(0x81b2,0);simulation.Poke(0x8247,0);
        for(int a=0x8248;a<=0x8253;a++)simulation.Poke(a,0);
    }
    private void PutPassengerOnLog() {
        int x=(simulation.Peek(0x811c)-34)&255;if(x<50||x>195)x=(simulation.Peek(0x811d)-34)&255;
        SetReviewFrog(x,96);simulation.Poke(0x8134,1);simulation.Poke(0x8135,1);
        simulation.Poke(0x8040,x);simulation.Poke(0x8041,0x1e);simulation.Poke(0x8042,4);simulation.Poke(0x8043,98);
    }
    private void StepReview() {
        reviewFrame++;
        if(reviewFrame==1) {
            if(review=="carry"||review=="carry-left"||review=="bonus")PutPassengerOnLog();
            if(review=="squash"||review=="drown") {
                int x=120,row=208;
                if(review=="drown") {
                    row=96;float best=-1;
                    for(int candidate=40;candidate<=200;candidate++) {
                        float gap=256;
                        for(int i=0;i<simulation.Peek(0x811b);i++) {
                            float center=simulation.Peek(0x811c+i)-34;
                            float distance=Math.Abs(((candidate-(int)center+128)&255)-128);
                            gap=Math.Min(gap,distance-22);
                        }
                        if(gap>best){best=gap;x=candidate;}
                    }
                }
                SetReviewFrog(x,row);simulation.Poke(0x8004,1);simulation.Poke(0x829c,review=="drown"?1:0);
            }
            if(review=="wrap")simulation.Poke(0x8113,254);
            if(review=="bottom-grass")SetReviewFrog(120,240);
            if(review is "log-left" or "log-right"){
                int chosen=120,count=simulation.Peek(0x811b);
                for(int index=0;index<Math.Min(8,count);index++){
                    int center=(simulation.Peek(0x811c+index)-34)&255;
                    if(center>=56&&center<=184){chosen=center;break;}
                }
                SetReviewFrog(chosen,96);
            }
        }
        if(review=="bonus"&&reviewFrame==12) {
            SetReviewFrog(120,32);simulation.Poke(0x8134,1);simulation.Poke(0x8135,1);
            simulation.Poke(0x8120,0);simulation.Poke(0x8121,3);simulation.Poke(0x8122,1);
        }
        int input=(review=="carry"||review=="carry-left")&&reviewFrame>=4&&reviewFrame<6
            ?review=="carry-left"?4:8:0;
        if(review=="hop-right")input=InputPulse.ForHeldDirection(8,state);
        if(review is "log-left" or "log-right")input=InputPulse.ForHeldDirection(review=="log-left"?4:8,state);
        simulation.Step(input);
        if(review=="turtle-rider"){
            var probe=simulation.Snapshot();float best=-1;int rideX=120,rideRow=64;
            foreach(int lane in new[]{1,4}){
                int table=0x8100+lane*9,row=(lane+3)*16,groupSize=lane==1?2:3,width=lane==1?31:47;
                for(int index=0;index<Math.Min(8,probe.At(table));index++){
                    float center=probe.At(table+index+1)-12-width/2f;
                    bool diver=knownDivingGroups.Contains((lane,index))||BoardVisuals.TurtlePhase(probe,center,row)>0;
                    float depth=BoardVisuals.TurtleDepth(probe,center,row,0,diver);
                    for(int member=0;member<groupSize;member++){
                        int x=(int)MathF.Round(center+(member-(groupSize-1)/2f)*16);
                        if(x>=24&&x<=216&&depth>best){best=depth;rideX=x;rideRow=row;}
                    }
                }
            }
            SetReviewFrog(rideX,rideRow);
        }
        if(review=="lady-move"){
            int bx=reviewFrame<=16?96+reviewFrame:reviewFrame<=30?128-reviewFrame:98+reviewFrame-30;
            simulation.Poke(0x8134,0);simulation.Poke(0x8135,1);
            simulation.Poke(0x8040,bx);simulation.Poke(0x8041,reviewFrame<=16?0x21:reviewFrame<=30?0xa1:0x1e);
            simulation.Poke(0x8042,4);simulation.Poke(0x8043,96);
        }
        if(review=="snake"){
            int x=(simulation.Peek(0x811c)-34)&255;
            if(x<50||x>195)x=(simulation.Peek(0x811d)-34)&255;
            simulation.Poke(0x8048,x);simulation.Poke(0x8049,1);simulation.Poke(0x804b,96);
        }
        ObserveFrame();accumulator=0;
    }
    private async void CaptureReview() {
        if(reviewCapturePending||!ReviewFrames.Contains(reviewFrame))return;
        reviewCapturePending=true;
        int frame=reviewFrame;
        await ToSignal(RenderingServer.Singleton,RenderingServer.SignalName.FramePostDraw);
        string stem=System.IO.Path.Combine(System.IO.Path.GetDirectoryName(screenshot)!,System.IO.Path.GetFileNameWithoutExtension(screenshot));
        string path=$"{stem}-{frame:D3}.png";
        var error=GetViewport().GetTexture().GetImage().SavePng(path);
        var player=Actor("player","frog");
        actors.TryGetValue("lady",out var freeLady);
        actors.TryGetValue("hazard32840",out var snake);
        int body=Enumerable.Range(0,player.Skeleton!.GetBoneCount()).Single(i=>player.Skeleton.GetBoneParent(i)==-1);
        var bodyScale=player.Skeleton.GetBonePoseScale(body);var bodyPose=player.Skeleton.GetBoneGlobalPose(body);
        bool riderVisible=actors.TryGetValue("passenger",out var rider)&&rider.Root.IsVisibleInTree();
        float socketError=riderVisible?(rider!.Root.GlobalPosition-player.PassengerSocket!.GlobalPosition).Length():0;
        reviewMeasurements.Add(new{frame,romFrame=state.frame,dying=frogVisual.Dying,drowning=frogVisual.Drowning,
            nativeX=state.At(0x8044),nativeRow=state.At(0x8047),
            hopFlags=Enumerable.Range(0,4).Select(i=>state.At(0x8248+i)).ToArray(),
            hopCounters=Enumerable.Range(0,4).Select(i=>state.At(0x8250+i)).ToArray(),
            deathSeconds=frogVisual.DeathSeconds(state.frame,0),carrying=frogVisual.Carrying,riderVisible,socketError,
            playerY=player.Root.GlobalPosition.Y,riderY=riderVisible?rider!.Root.GlobalPosition.Y:0,
            playerYaw=player.Root.GlobalRotation.Y,riderYaw=riderVisible?rider!.Root.GlobalRotation.Y:0,
            riderScale=riderVisible?rider!.Root.Scale.X:0,
            ladyVisible=freeLady?.Root.IsVisibleInTree()??false,
            ladyYaw=freeLady?.Root.GlobalRotation.Y??0,ladyScale=freeLady?.Root.Scale.X??0,
            snakeVisible=snake?.Root.IsVisibleInTree()??false,
            snakeY=snake?.Root.GlobalPosition.Y??0,
            hopActive=frogVisual.HopActive(state.frame),hopDirection=frogVisual.HopDirection,
            ladyHopping=ladyHopStartFrame>0&&state.frame-ladyHopStartFrame<12,
            turtleRideDepth=TurtleRideDepth(state.At(0x8044),state.At(0x8047)),
            bodyScale=new[]{bodyScale.X,bodyScale.Y,bodyScale.Z},bodyOrigin=new[]{bodyPose.Origin.X,bodyPose.Origin.Y,bodyPose.Origin.Z},
            modelScale=new[]{player.Root.Scale.X,player.Root.Scale.Y,player.Root.Scale.Z},
            bonusLabels=popups.Select(p=>new{amount=p.Award.Amount,kind=p.Award.Kind.ToString(),p.Label.Text}).ToArray(),
            cameraMode=camera.Projection.ToString(),following=followCamera,cameraSize=camera.Size,cameraFov=camera.Fov,
            cameraPosition=new[]{camera.Position.X,camera.Position.Y,camera.Position.Z},
            visibleLogs=actors.Count(p=>p.Key.StartsWith("lane")&&p.Value.Root.IsVisibleInTree()),error=error.ToString()});
        GD.Print($"REVIEW {review} {frame} {error}");
        if(error!=Error.Ok){GetTree().Quit(1);return;}
        if(frame==ReviewFrames[^1]){
            if(review=="lady-move"&&ladyFacing!=3){GD.PushError("Pink frog lost its left-facing pose during neutral log drift");GetTree().Quit(1);return;}
            System.IO.File.WriteAllText(stem+".json",JsonSerializer.Serialize(reviewMeasurements,new JsonSerializerOptions{WriteIndented=true}));
            GetTree().Quit();
        }
        reviewCapturePending=false;
    }
    private void UpdateReviewCamera(){
        if(!reviewClose&&!reviewLadyClose&&!reviewTurtleClose)return;
        var player=Actor("player","frog");
        var target=reviewTurtleClose&&turtleSupports.Any(t=>t.X>=24&&t.X<=216)
            ?turtleSupports.Where(t=>t.X>=24&&t.X<=216).OrderByDescending(t=>t.Depth).Select(t=>Pos(t.X,t.Row,-.22f-t.Depth)+Vector3.Up*.2f).First()
            :reviewLadyClose&&actors.TryGetValue("lady",out var lady)?lady.Root.Position+Vector3.Up*.2f:player.Root.Position+Vector3.Up*.3f;
        camera.Size=2.5f;camera.Position=target+new Vector3(2.5f,2.5f,3.5f);camera.LookAt(target);
    }
}
