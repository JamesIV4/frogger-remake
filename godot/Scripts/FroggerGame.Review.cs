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
    private readonly List<object> reviewMeasurements=new();
    private int[] ReviewFrames=>review switch {
        "carry"=>new[]{1,5,9,13,25},
        "squash"=>new[]{1,4,8,16,32,48,72,92},
        "drown"=>new[]{1,8,16,32,48,64,76},
        "bonus"=>new[]{1,13,25,50,95,115},
        "wrap"=>new[]{1,2,8,24,48,72,96,120},
        "turtle"=>new[]{1,25,50,75,100,125,150,175,200,225,250},
        _=>new[]{10}
    };
    private void ReadReviewArgs() {
        if(screenshot=="")return;
        foreach(var arg in OS.GetCmdlineUserArgs())if(arg.StartsWith("--review="))review=arg[9..];
        reviewClose=Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--review-close");
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
            if(review=="carry"||review=="bonus")PutPassengerOnLog();
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
        }
        if(review=="bonus"&&reviewFrame==12) {
            SetReviewFrog(120,32);simulation.Poke(0x8134,1);simulation.Poke(0x8135,1);
            simulation.Poke(0x8120,0);simulation.Poke(0x8121,3);simulation.Poke(0x8122,1);
        }
        int input=review=="carry"&&reviewFrame>=4&&reviewFrame<6?8:0;
        simulation.Step(input);ObserveFrame();accumulator=0;
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
        int body=Enumerable.Range(0,player.Skeleton!.GetBoneCount()).Single(i=>player.Skeleton.GetBoneParent(i)==-1);
        var bodyScale=player.Skeleton.GetBonePoseScale(body);var bodyPose=player.Skeleton.GetBoneGlobalPose(body);
        bool riderVisible=actors.TryGetValue("passenger",out var rider)&&rider.Root.IsVisibleInTree();
        float socketError=riderVisible?(rider!.Root.GlobalPosition-player.PassengerSocket!.GlobalPosition).Length():0;
        reviewMeasurements.Add(new{frame,romFrame=state.frame,dying=frogVisual.Dying,drowning=frogVisual.Drowning,
            deathSeconds=frogVisual.DeathSeconds(state.frame,0),carrying=frogVisual.Carrying,riderVisible,socketError,
            playerY=player.Root.GlobalPosition.Y,riderY=riderVisible?rider!.Root.GlobalPosition.Y:0,
            bodyScale=new[]{bodyScale.X,bodyScale.Y,bodyScale.Z},bodyOrigin=new[]{bodyPose.Origin.X,bodyPose.Origin.Y,bodyPose.Origin.Z},
            modelScale=new[]{player.Root.Scale.X,player.Root.Scale.Y,player.Root.Scale.Z},
            bonusLabels=popups.Select(p=>new{amount=p.Award.Amount,kind=p.Award.Kind.ToString(),p.Label.Text}).ToArray(),
            visibleLogs=actors.Count(p=>p.Key.StartsWith("lane")&&p.Value.Root.IsVisibleInTree()),error=error.ToString()});
        GD.Print($"REVIEW {review} {frame} {error}");
        if(error!=Error.Ok){GetTree().Quit(1);return;}
        if(frame==ReviewFrames[^1]){
            System.IO.File.WriteAllText(stem+".json",JsonSerializer.Serialize(reviewMeasurements,new JsonSerializerOptions{WriteIndented=true}));
            GetTree().Quit();
        }
        reviewCapturePending=false;
    }
    private void UpdateReviewCamera(){
        if(!reviewClose)return;
        var player=Actor("player","frog");var target=player.Root.Position+Vector3.Up*.3f;
        camera.Size=2.5f;camera.Position=target+new Vector3(2.5f,2.5f,3.5f);camera.LookAt(target);
    }
}
