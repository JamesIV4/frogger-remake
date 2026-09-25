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
    private bool reviewGatorClose;
    private bool reviewSnakeClose;
    private bool reviewLadyClearVisible,reviewLadyPatrolVisible;
    private bool reviewGatorSafeObserved,reviewGatorSnoutDeathObserved;
    private readonly List<object> reviewMeasurements=new();
    private int[] ReviewFrames=>review switch {
        "carry" or "carry-left"=>new[]{1,5,9,13,25},
        "lady-move"=>new[]{1,8,17,25,30,40,45},
        "lady-hidden"=>new[]{1,8},
        "lady-hidden-pickup"=>new[]{1,8,9,12},
        "lady-goal-overwrite"=>new[]{1,8},
        "lady-rom-clear"=>Enumerable.Range(1,12).ToArray(),
        "snake" or "snake-left"=>new[]{1,8},
        "bottom-grass"=>new[]{1,8},
        "hop-right"=>new[]{1,6,12,24,48},
        "log-left" or "log-right"=>new[]{1,4,8,12,16,20,24,32,40,48},
        "turtle-rider"=>new[]{1,25,50,75,100,125,150,175,200,225,250},
        "turtle-drown"=>new[]{1,100,124,125,126,130,140,150,175},
        "squash"=>new[]{1,4,8,16,32,48,72,92},
        "edge-squash" or "upper-edge-squash"=>new[]{1,4,8,16,32},
        "drown"=>new[]{1,8,16,32,48,64,76},
        "bonus" or "bonus-follow"=>new[]{1,13,25,50,95,105,115},
        "home-input"=>new[]{1,13,16,18,25},
        "home-normal"=>new[]{1,13,25,30,50},
        "final-home"=>new[]{1,3,4,5,7,9,11,16,25,28},
        "game-over"=>new[]{1,10},
        "game-over-a"=>new[]{1,2,4,8},
        "default-view"=>new[]{1,40},
        "home-gator"=>new[]{1,20,40,60,80,100},
        "home-gator-retreat"=>new[]{1,20,40,60,80,100,175,176,178,182,186,188,190},
        "river-gator"=>new[]{1,12,24},
        "river-back" or "river-snout"=>new[]{1,2,4,8,16},
        "snake-motion"=>new[]{1,8,16,24,32,40,48,56,64,72,80},
        "wrap"=>new[]{1,2,8,24,48,72,96,120},
        "motion"=>new[]{60,61,62,63,64,65,66,67,68},
        "frog-motion"=>new[]{66,67,68,69,70,71,72,73,74},
        "turtle"=>new[]{1,25,50,75,100,125,150,175,200,225,250},
        "perspective" or "perspective-follow" or "ortho-follow"=>new[]{1,40},
        _=>new[]{10}
    };
    private void ReadReviewArgs() {
        if(screenshot=="")return;
        foreach(var arg in OS.GetCmdlineUserArgs())if(arg.StartsWith("--review="))review=arg[9..];
        if(review!=""&&review!="default-view"){
            perspectiveView=review is "perspective" or "perspective-follow";
            followCamera=review is "perspective-follow" or "ortho-follow" or "bonus-follow";
        }
        reviewClose=Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--review-close");
        reviewLadyClose=Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--review-lady-close");
        reviewTurtleClose=Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--review-turtle-close");
        reviewGatorClose=Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--review-gator-close");
        reviewSnakeClose=Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--review-snake-close");
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
            if(review=="carry"||review=="carry-left"||review=="bonus"||review=="bonus-follow"||review=="home-input")PutPassengerOnLog();
            if(review=="squash"||review=="drown"||review=="edge-squash"||review=="upper-edge-squash") {
                int x=120,row=208;
                if(review=="edge-squash"){
                    row=216;simulation.Poke(0x815a,1);simulation.Poke(0x815b,x+12);
                }
                if(review=="upper-edge-squash"){
                    row=136;simulation.Poke(0x8136,1);simulation.Poke(0x8137,x+20);
                }
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
            if(review=="final-home"){
                SetReviewFrog(216,42);
                for(int bay=0;bay<4;bay++)simulation.Poke(0x825e+bay,1);
                simulation.Poke(0x825c,4);simulation.Poke(0x8268,0);
                simulation.Poke(0x8249,1);simulation.Poke(0x8251,5);simulation.Poke(0x8254,2);
                simulation.Poke(0x8134,1);simulation.Poke(0x8135,1);
                simulation.Poke(0x8040,216);simulation.Poke(0x8041,0x1e);simulation.Poke(0x8043,44);
            }
        }
        if((review=="bonus"||review=="bonus-follow"||review=="home-input")&&reviewFrame==12) {
            SetReviewFrog(120,32);simulation.Poke(0x8134,1);simulation.Poke(0x8135,1);
            simulation.Poke(0x8120,0);simulation.Poke(0x8121,3);simulation.Poke(0x8122,1);
        }
        if(review=="home-normal"&&reviewFrame==12){
            SetReviewFrog(120,32);simulation.Poke(0x8120,0);simulation.Poke(0x8121,0);simulation.Poke(0x8122,1);
            simulation.Poke(0x8134,0);simulation.Poke(0x8135,0);
        }
        int input=(review=="carry"||review=="carry-left")&&reviewFrame>=4&&reviewFrame<6
            ?review=="carry-left"?4:8:0;
        // Presentation-only half frames do not sample physical input; an edge
        // must be consumed on a frame that actually advances the native ROM.
        if((review=="motion"||review=="frog-motion")&&reviewFrame%2==0){accumulator=0;return;}
        if(review=="hop-right")input=AdaptDirection(8);
        if(review=="frog-motion")input=AdaptDirection(8);
        if(review=="home-input"&&reviewFrame>=16&&reviewFrame<18)input=AdaptDirection(8);
        if(review is "log-left" or "log-right")input=AdaptDirection(review=="log-left"?4:8);
        simulation.Step(input);
        if(review=="lady-rom-clear"&&reviewFrame==1){
            SetReviewFrog(120,224);
            simulation.Poke(0x8134,0);simulation.Poke(0x8135,1);simulation.Poke(0x813d,0);
            simulation.Poke(0x8040,80);simulation.Poke(0x8041,0x21);
            simulation.Poke(0x8042,4);simulation.Poke(0x8043,96);
            simulation.Poke(0x811c,100);simulation.Poke(0x833d,1);simulation.Poke(0x833e,50);
            simulation.Poke(0x8340,2);
        }
        if(review=="game-over-a"){
            if(reviewFrame==1)simulation.Poke(0x83fe,0);
            if(reviewFrame==2)_UnhandledInput(new InputEventJoypadButton{ButtonIndex=JoyButton.A,Pressed=true});
        }
        if(review is "river-gator" or "river-back" or "river-snout"){
            // Put the ROM's armed lane-0 crocodile at a known screen position.
            const int x=120;
            simulation.Poke(0x83b7,2);simulation.Poke(0x8100,1);simulation.Poke(0x8101,x+12+60/2);
            simulation.Poke(0x8150,1);
            if(reviewFrame==1&&review!="river-gator")SetReviewFrog(review=="river-back"?130:152,48);
        }
        if(review is "home-gator" or "home-gator-retreat"){
            int phase=Math.Min(reviewFrame,review=="home-gator"?120:200),bay=2,baseAddress=0xab64-bay*0xc0;
            simulation.Poke(0x8122,phase);
            simulation.Poke(baseAddress,phase<80||phase>=176?16:208);
            simulation.Poke(baseAddress+1,phase<80||phase>=176?16:209);
            simulation.Poke(baseAddress+32,phase>=176?16:phase<80?208:210);
            simulation.Poke(baseAddress+33,phase>=176?16:phase<80?209:211);
        }
        if(review=="game-over")simulation.Poke(0x83fe,0);
        if(review=="turtle-rider"||review=="turtle-drown"&&reviewFrame<=125){
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
            if(review=="turtle-drown"&&reviewFrame==125){simulation.Poke(0x8004,1);simulation.Poke(0x829c,1);}
        }
        if(review=="lady-move"){
            int bx=reviewFrame<=16?96+reviewFrame:reviewFrame<=30?128-reviewFrame:98+reviewFrame-30;
            simulation.Poke(0x8134,0);simulation.Poke(0x8135,1);
            simulation.Poke(0x8040,bx);simulation.Poke(0x8041,reviewFrame<=16?0xa1:reviewFrame<=30?0x21:0x1e);
            simulation.Poke(0x8042,4);simulation.Poke(0x8043,96);
        }
        if(review is "lady-hidden" or "lady-hidden-pickup" or "lady-goal-overwrite"){
            const int patrolX=80;
            if(reviewFrame==1)SetReviewFrog(120,224);
            if(review=="lady-hidden-pickup"&&reviewFrame>=9)SetReviewFrog(patrolX,96);
            simulation.Poke(0x8135,1);simulation.Poke(0x8134,review=="lady-hidden-pickup"&&reviewFrame>=9?1:0);
            simulation.Poke(0x811c,100);simulation.Poke(0x833d,1);simulation.Poke(0x833e,49);
            simulation.Poke(0x8040,review=="lady-goal-overwrite"?216:0);
            simulation.Poke(0x8041,review=="lady-goal-overwrite"?0x19:0);
            simulation.Poke(0x8042,review=="lady-goal-overwrite"?3:0);
            simulation.Poke(0x8043,review=="lady-goal-overwrite"?16:0);
        }
        if(review is "snake" or "snake-left"){
            int x=review=="snake-left"?180-reviewFrame:(simulation.Peek(0x811c)-34)&255;
            if(review=="snake"&&(x<50||x>195))x=(simulation.Peek(0x811d)-34)&255;
            simulation.Poke(0x8048,x);simulation.Poke(0x8049,review=="snake-left"?0x81:1);simulation.Poke(0x804b,96);
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
        actors.TryGetValue("homegator2",out var homeGator);
        actors.TryGetValue("lane0.0.0.0.True",out var riverGator);
        int body=Enumerable.Range(0,player.Skeleton!.GetBoneCount()).Single(i=>player.Skeleton.GetBoneParent(i)==-1);
        var bodyScale=player.Skeleton.GetBonePoseScale(body);var bodyPose=player.Skeleton.GetBoneGlobalPose(body);
        bool riderVisible=actors.TryGetValue("passenger",out var rider)&&rider.Root.IsVisibleInTree();
        if(review=="river-back"&&state.At(0x8004)!=0&&!frogVisual.Dying)reviewGatorSafeObserved=true;
        if(review=="river-snout"&&state.At(0x829c)!=0&&frogVisual.Dying)reviewGatorSnoutDeathObserved=true;
        if(review=="lady-rom-clear"&&state.At(0x8135)!=0&&state.At(0x8041)==0){
            if(state.At(0x8040)==0)reviewLadyClearVisible|=freeLady?.Root.IsVisibleInTree()??false;
            else reviewLadyPatrolVisible|=freeLady?.Root.IsVisibleInTree()??false;
        }
        float socketError=riderVisible?(rider!.Root.GlobalPosition-player.PassengerSocket!.GlobalPosition).Length():0;
        reviewMeasurements.Add(new{frame,romFrame=state.frame,dying=frogVisual.Dying,drowning=frogVisual.Drowning,
            gameState=state.At(0x83fe),started,menuVisible=menu.Visible,mouseMode=Input.MouseMode.ToString(),messageText=message.Text,
            messagePosition=new[]{message.GlobalPosition.X,message.GlobalPosition.Y},
            messageSize=new[]{message.Size.X,message.Size.Y},
            nativeX=state.At(0x8044),nativeRow=state.At(0x8047),
            displayedFrogX,displayedFrogRow,
            hopFlags=Enumerable.Range(0,4).Select(i=>state.At(0x8248+i)).ToArray(),
            hopCounters=Enumerable.Range(0,4).Select(i=>state.At(0x8250+i)).ToArray(),
            deathSeconds=frogVisual.DeathSeconds(state.frame,0),deathInitialHeight,
            drownFloor=BoardVisuals.SurfaceHeight(frogVisual.DeathRow)-FrogVisualState.MaximumDrownDepth,
            carrying=frogVisual.Carrying,riderVisible,socketError,
            playerY=player.Root.GlobalPosition.Y,riderY=riderVisible?rider!.Root.GlobalPosition.Y:0,
            playerZ=player.Root.GlobalPosition.Z,riderZ=riderVisible?rider!.Root.GlobalPosition.Z:0,
            playerYaw=player.Root.GlobalRotation.Y,riderYaw=riderVisible?rider!.Root.GlobalRotation.Y:0,
            riderScale=riderVisible?rider!.Root.Scale.X:0,
            ladyVisible=freeLady?.Root.IsVisibleInTree()??false,
            ladyYaw=freeLady?.Root.GlobalRotation.Y??0,ladyScale=freeLady?.Root.Scale.X??0,
            ladyCode=state.At(0x8041),ladyX=state.At(0x8040),ladyRow=state.At(0x8043),
            ladyArmed=state.At(0x8135)!=0,ladyAttached=state.At(0x8134)!=0,
            ladyY=freeLady?.Root.GlobalPosition.Y??0,ladyVisualX=ladyVisual.X,
            snakeVisible=snake?.Root.IsVisibleInTree()??false,
            snakeY=snake?.Root.GlobalPosition.Y??0,
            snakeX=state.At(0x8048),snakeRow=state.At(0x804b),snakeCode=state.At(0x8049),
            snakeYaw=snake?.Root.GlobalRotation.Y??0,
            homeGatorVisible=homeGator?.Root.IsVisibleInTree()??false,
            homeGatorZ=homeGator?.Root.GlobalPosition.Z??0,
            homeGatorY=homeGator?.Root.GlobalPosition.Y??0,
            homeGatorPhase=state.At(0x8122),
            homeGatorRetreating=homeGatorVisuals[2].Retreating,
            riverGatorVisible=riverGator?.Root.IsVisibleInTree()??false,
            riverGatorScale=riverGator==null?Array.Empty<float>():new[]{riverGator.Root.Scale.X,riverGator.Root.Scale.Y,riverGator.Root.Scale.Z},
            riverGatorX=riverGator?.Root.GlobalPosition.X??0,
            riverGatorTipX=riverGator==null?0:120f+16f*(riverGator.Root.GlobalPosition.X+ModelFootprints.RiverGatorFrontTiles*riverGator.Root.Scale.Z),
            riverGatorSnoutLength=riverGator==null?0:16f*ModelFootprints.RiverGatorSnoutTiles*riverGator.Root.Scale.Z,
            nativeGatorTipX=state.At(0x8101),
            riverGatorArmed=BoardVisuals.RiverGatorActive(state),
            riverGatorZone=BoardVisuals.RiverGatorContact(state.At(0x8044),state.At(0x8101)).ToString(),
            riverGatorRideTile=state.At(0xa846),holdFlag=state.At(0x8004),secondBank=state.At(0x829c),
            hopActive=frogVisual.HopActive(state.frame),hopDirection=frogVisual.HopDirection,
            ladyHopping=ladyHopStartFrame>0&&state.frame-ladyHopStartFrame<12,
            turtleRideDepth=TurtleRideDepth(state.At(0x8044),state.At(0x8047)),
            bodyScale=new[]{bodyScale.X,bodyScale.Y,bodyScale.Z},bodyOrigin=new[]{bodyPose.Origin.X,bodyPose.Origin.Y,bodyPose.Origin.Z},
            modelScale=new[]{player.Root.Scale.X,player.Root.Scale.Y,player.Root.Scale.Z},
            holdingHome=homeArrival.Active(state.frame,0),finishingHomeHop=homeArrival.FinishingHop(state.frame,0),
            homeX=homeArrival.X,homeRow=homeArrival.Row,homeVisualRow=homeArrival.VisualRow(state.frame,0),
            bonusLabels=popups.Select(p=>new{amount=p.Award.Amount,kind=p.Award.Kind.ToString(),text=p.DisplayText,
                screenX=p.View.Position.X,screenY=p.View.Position.Y}).ToArray(),
            cameraMode=camera.Projection.ToString(),following=followCamera,modernCollision=modern,
            fullscreenPreference=fullscreen,windowMode=DisplayServer.WindowGetMode().ToString(),
            cameraSize=camera.Size,cameraFov=camera.Fov,
            cameraPosition=new[]{camera.Position.X,camera.Position.Y,camera.Position.Z},
            motionLogX=movingVisuals.DisplayX(3*16),motionCarX=movingVisuals.DisplayX(8*16),
            nativeLogX=state.At(0x811c)-34,nativeCarX=state.At(0x8149)-12,
            visibleLogs=actors.Count(p=>p.Key.StartsWith("lane")&&p.Value.Root.IsVisibleInTree()),error=error.ToString()});
        GD.Print($"REVIEW {review} {frame} {error}");
        if(error!=Error.Ok){GetTree().Quit(1);return;}
        if(frame==ReviewFrames[^1]){
            if(review=="lady-move"&&ladyFacing!=3){GD.PushError("Pink frog lost its left-facing pose during neutral log drift");GetTree().Quit(1);return;}
            if(review=="lady-hidden"&&!(freeLady?.Root.IsVisibleInTree()??false)){GD.PushError("Armed pink frog is invisible with a blank ROM sprite");GetTree().Quit(1);return;}
            if(review=="lady-goal-overwrite"&&(!(freeLady?.Root.IsVisibleInTree()??false)||Math.Abs(ladyVisual.X-80)>1)){GD.PushError("Goal sprite overwrote the pink frog's position");GetTree().Quit(1);return;}
            if(review=="lady-rom-clear"&&(!reviewLadyClearVisible||!reviewLadyPatrolVisible)){GD.PushError("The original ROM clear made the pink frog invisible in Godot");GetTree().Quit(1);return;}
            if(review=="river-back"&&!reviewGatorSafeObserved){GD.PushError("Modern river gator back did not remain safe");GetTree().Quit(1);return;}
            if(review=="river-snout"&&!reviewGatorSnoutDeathObserved){GD.PushError("Modern river gator snout was not fatal");GetTree().Quit(1);return;}
            if(review=="lady-hidden-pickup"&&(!riderVisible||(freeLady?.Root.IsVisibleInTree()??false))){GD.PushError("Pink frog did not move visibly from log to passenger");GetTree().Quit(1);return;}
            if(review=="game-over-a"&&state.At(0x83fe)==0){GD.PushError("Controller A did not restart after game over");GetTree().Quit(1);return;}
            if(review=="default-view"&&(!modern||!perspectiveView||!followCamera||!fullscreen)){GD.PushError("Fresh-game defaults changed unexpectedly");GetTree().Quit(1);return;}
            if(review=="snake-left"&&snake?.Root.Rotation.Y> -1.3f){GD.PushError("Snake failed to face its leftward travel");GetTree().Quit(1);return;}
            if(review=="snake-motion"&&snake?.Root.Rotation.Y<1.3f){GD.PushError("Snake followed a ROM flip instead of its rightward lane travel");GetTree().Quit(1);return;}
            System.IO.File.WriteAllText(stem+".json",JsonSerializer.Serialize(reviewMeasurements,new JsonSerializerOptions{WriteIndented=true}));
            GetTree().Quit();
        }
        reviewCapturePending=false;
    }
    private void UpdateReviewCamera(){
        if(!reviewClose&&!reviewLadyClose&&!reviewTurtleClose&&!reviewGatorClose&&!reviewSnakeClose)return;
        var player=Actor("player","frog");
        var target=reviewGatorClose?(review is "river-gator" or "river-back" or "river-snout"?Pos(134,48,0):new Vector3(0,.30f,-6.3f))
            :reviewSnakeClose&&actors.TryGetValue("hazard32840",out var activeSnake)?activeSnake.Root.Position+Vector3.Up*.1f
            :reviewTurtleClose&&turtleSupports.Any(t=>t.X>=24&&t.X<=216)
            ?turtleSupports.Where(t=>t.X>=24&&t.X<=216).OrderByDescending(t=>t.Depth).Select(t=>Pos(t.X,t.Row,-.22f-t.Depth)+Vector3.Up*.2f).First()
            :reviewLadyClose&&actors.TryGetValue("lady",out var lady)?lady.Root.Position+Vector3.Up*.2f:player.Root.Position+Vector3.Up*.3f;
        camera.Size=reviewGatorClose?3.3f:2.5f;
        camera.Position=target+(reviewGatorClose?new Vector3(2.0f,2.2f,2.9f):new Vector3(2.5f,2.5f,3.5f));camera.LookAt(target);
    }
}
