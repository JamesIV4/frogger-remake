using Godot;
using System;
using System.Collections.Generic;
namespace FroggerRemake;

public partial class FroggerGame
{
    private readonly FrogVisualState frogVisual=new();
    private readonly PresentationMotion movingVisuals=new();
    private float presentationDelta;
    private FrameState previousState=new();
    private readonly List<(BonusAward Award,Label3D Label)> popups=new();
    private MeshInstance3D? deathRipple;
    private StandardMaterial3D? rippleMaterial;
    private readonly HashSet<(int Lane,int Group)> knownDivingGroups=new();
    private readonly List<(float X,int Row,float Depth)> turtleSupports=new();
    private int knownDivingLevel=-1;
    private int ladyFacing=2;
    private int ladyHopStartFrame,ladyLastMotionFrame;
    private const float LadyInRiverScale=.62f,PassengerScale=.76f;

    private void ObserveFrame() {
        previousState=state;state=simulation.Snapshot();frogVisual.Observe(state);TrackLadyHop();
    }
    private void ClearPresentation() {
        frogVisual.Reset();movingVisuals.Reset();facing=2;ladyFacing=2;ladyHopStartFrame=ladyLastMotionFrame=0;knownDivingGroups.Clear();knownDivingLevel=-1;
        foreach(var popup in popups)popup.Label.QueueFree();popups.Clear();
    }
    private float RenderFraction=>paused?0:(float)Math.Clamp(accumulator/ArcadeSimulation.FrameSeconds,0,1);
    private float BlendCoordinate(int address,float fraction) =>previousState.frame+1==state.frame
        ?BoardVisuals.InterpolateByte(previousState.At(address),state.At(address),fraction):state.At(address);
    private void TrackLadyHop() {
        if(state.At(0x8135)==0||frogVisual.Carrying)return;
        // ROM sprite 0x21 / 0xa1 is the actual patrol heading. X also drifts
        // with the log, so its sign can reverse after a left hop without a
        // new pink-frog hop. Neutral 0x1e keeps the last facing direction.
        int code=state.At(0x8041),direction=code==0x21?1:code==0xa1?3:0;
        if(ladyFacing==2)ladyFacing=(state.At(0x833d)&0x80)!=0?3:1;
        if(direction==0)return;
        if(ladyHopStartFrame==0||direction!=ladyFacing||state.frame-ladyHopStartFrame>=12)ladyHopStartFrame=state.frame;
        ladyFacing=direction;ladyLastMotionFrame=state.frame;
    }

    private void UpdateActors() {
        float fraction=RenderFraction;
        foreach(var actor in actors.Values)actor.Root.Visible=false;
        UpdateLanes(fraction);
        UpdatePlayer(fraction);
        UpdateHomesAndHazards();
        UpdateBonuses(fraction);
    }
    private void UpdatePlayer(float fraction) {
        var player=Actor("player","frog");
        player.Root.Scale=Vector3.One;
        player.Root.Visible=FrogVisualState.PlayerOnBoard(state);
        int hop=frogVisual.HopDirection;
        if(frogVisual.HopActive(state.frame))facing=hop switch{1=>0,2=>2,3=>1,_=>3};
        player.Root.Rotation=new Vector3(0,facing*Mathf.Pi/2,0);
        if(frogVisual.Dying) {
            // Freeze the impact point and sample one continuous clip. 0x8247
            // resets every 16 frames and must never be used as sink height.
            double age=frogVisual.DeathSeconds(state.frame,fraction);
            float height=BoardVisuals.SurfaceHeight(frogVisual.DeathRow);
            if(frogVisual.Drowning)height-=FrogVisualState.DrownDepth(age);
            else {float squash=FrogVisualState.SquashProgress(age);player.Root.Scale=new Vector3(1+.50f*squash,1-.92f*squash,1+.40f*squash);}
            player.Root.Position=Pos(frogVisual.DeathX,frogVisual.DeathRow,height);
            player.Pose(frogVisual.Drowning?"Drown":"Squash",age);
        } else {
            float x=BlendCoordinate(0x8044,fraction),row=BlendCoordinate(0x8047,fraction);
            player.Root.Position=Pos(x,row,BoardVisuals.SurfaceHeight(row)-TurtleRideDepth(x,row));
            if(frogVisual.HopActive(state.frame))player.Pose("Hop",frogVisual.HopSeconds(state.frame,fraction));
            else player.Play("Idle");
        }
        if(frogVisual.Carrying) {
            if(player.PassengerSocket==null)throw new InvalidOperationException("The Blender frog rig has no PassengerSocket");
            if(!actors.TryGetValue("passenger",out var passenger)) {
                passenger=new ModelActor(player.PassengerSocket,"lady_frog");actors.Add("passenger",passenger);
            }
            passenger.Root.Visible=true;passenger.Root.Position=Vector3.Zero;
            passenger.Root.Scale=Vector3.One*PassengerScale;
            passenger.Root.GlobalRotation=new Vector3(0,player.Root.GlobalRotation.Y,0);
            if(frogVisual.HopActive(state.frame))passenger.Pose("Hop",frogVisual.HopSeconds(state.frame,fraction));
            else passenger.Play("Idle");
        }
        UpdateDeathRipple(fraction);
    }
    private void UpdateLanes(float fraction) {
        turtleSupports.Clear();
        int[] widths={60,31,92,44,47,0,34,18,18,18,18};
        string[] models={"log","turtle","log","log","turtle","","truck","sport","car","dozer","racecar"};
        if(knownDivingLevel!=state.At(0x83b7)){knownDivingGroups.Clear();knownDivingLevel=state.At(0x83b7);}
        for(int lane=0;lane<11;lane++) {
            if(lane==5)continue;
            int table=0x8100+lane*9,count=Math.Min(8,state.At(table)),row=(lane+3)*16,width=widths[lane];
            bool turtle=models[lane]=="turtle";
            for(int index=0;index<count;index++) {
                float rawCenter=state.At(table+index+1)-(lane<5?12:3)-width/2f;
                float center=movingVisuals.Step(lane*16+index,rawCenter,presentationDelta);
                int members=lane==1?2:lane==4?3:1;
                if(turtle&&BoardVisuals.TurtlePhase(state,rawCenter,row)>0)knownDivingGroups.Add((lane,index));
                float depth=turtle?BoardVisuals.TurtleDepth(state,rawCenter,row,fraction,knownDivingGroups.Contains((lane,index))):0;
                bool crocodile=lane==0&&BoardVisuals.GatorOnLog(state,rawCenter,row);
                for(int member=0;member<members;member++)for(int wrap=-1;wrap<=1;wrap++) {
                    float x=center+wrap*256+(member-(members-1)/2f)*16;
                    if(turtle)turtleSupports.Add((x,row,depth));
                    float halfWidth=lane<5&&!turtle?(width-3)*.52f:turtle?9:lane==6?15:10;
                    // Keep a wrapping replica until the LAST visible part leaves
                    // the board. The shader clips the portion beyond the rail.
                    if(!BoardVisuals.IntersectsPlayfield(x,halfWidth))continue;
                    var obj=Actor($"lane{lane}.{index}.{member}.{wrap}.{crocodile}",crocodile?"gator":models[lane]);
                    obj.Root.Visible=true;obj.Root.Position=Pos(x,row,lane<5?(turtle?-.22f:-.18f):.02f);
                    if(crocodile){obj.Root.Rotation=new Vector3(0,Mathf.Pi/2,0);obj.Root.Scale=new Vector3(1,1,(width-3)/30f);obj.Play("Bite");}
                    else if(models[lane]=="log")obj.Root.Scale=new Vector3((width-3)/16f,1,1);
                    else if(turtle){obj.Root.Position+=new Vector3(0,-depth,0);obj.Root.Rotation=new Vector3(0,-Mathf.Pi/2,0);obj.Play(depth>.01f?"Dive":"Swim");}
                    else{obj.Root.Rotation=new Vector3(0,(lane%2==0?-1:1)*Mathf.Pi/2,0);obj.Root.Scale=Vector3.One*.84f;obj.Play("Move");}
                }
            }
        }
    }
    private float TurtleRideDepth(float x,float row) {
        float nearest=10,depth=0;
        foreach(var support in turtleSupports){
            float distance=Math.Abs(x-support.X);
            if(Math.Abs(row-support.Row)<=7.5f&&distance<nearest){nearest=distance;depth=support.Depth;}
        }
        return nearest<=10?depth:0;
    }
    private void UpdateHomesAndHazards() {
        int homeBase=state.At(0x83fd)==2?0x8263:0x825e;
        for(int i=0;i<5;i++) {
            if(state.At(homeBase+i)!=0){var a=Actor($"home{i}","frog");a.Root.Visible=true;a.Root.Position=new Vector3(-6+3*i,.08f,-6);a.Root.Rotation=new Vector3(0,Mathf.Pi,0);a.Play("Celebrate");}
            int tile=state.At(0xab64-i*0xc0);
            if(tile>=44&&tile<=47){var a=Actor($"homefly{i}","fly");a.Root.Visible=true;a.Root.Position=new Vector3(-6+3*i,.12f,-6);}
            if(tile==208||state.At(0xab64-i*0xc0+32)==208){var a=Actor($"homegator{i}","gator");a.Root.Visible=true;a.Root.Position=new Vector3(-6+3*i,tile==208?0:-.15f,-6);a.Root.Scale=Vector3.One*.6f;a.Play("Bite");}
        }
        foreach(int addr in new[]{0x8048,0x8050,0x8058}) {
            int x=state.At(addr),y=state.At(addr+3);if(x<8||x>235||y<32||y>136||state.At(addr+1)==0)continue;
            var a=Actor($"hazard{addr}",addr==0x8058?"otter":"snake");a.Root.Visible=true;
            a.Root.Position=Pos(movingVisuals.Step(1000+addr,x,presentationDelta),y,BoardVisuals.SurfaceHeight(y)+(addr==0x8058?.01f:.07f));
            if(addr!=0x8058&&y<128)a.Root.Position+=Vector3.Forward*.14f;
            a.Root.Rotation=new Vector3(0,Mathf.Pi/2,0);a.Root.Scale=Vector3.One*.75f;a.Play("Move");
        }
        int bx=state.At(0x8040),by=state.At(0x8043);
        if(!frogVisual.Carrying&&state.At(0x8135)!=0&&bx>7&&bx<235&&by>=32&&by<128&&state.At(0x8041)!=0x19) {
            var a=Actor("lady","lady_frog");a.Root.Visible=true;a.Root.Position=Pos(movingVisuals.Step(50000,bx,presentationDelta),by,BoardVisuals.SurfaceHeight(by));
            a.Root.Scale=Vector3.One*LadyInRiverScale;
            a.Root.Rotation=new Vector3(0,ladyFacing*Mathf.Pi/2,0);
            if(ladyHopStartFrame>0&&ladyLastMotionFrame>=ladyHopStartFrame&&state.frame-ladyHopStartFrame<12)
                a.Pose("Hop",Math.Clamp((state.frame-ladyHopStartFrame+RenderFraction)/12.0,0,1)*(10.0/60.0));
            else a.Play("Idle");
        }
    }
    private void UpdateBonuses(float fraction) {
        while(simulation.BonusAwards.TryDequeue(out var award)) {
            var label=new Label3D{Text=award.Kind==BonusKind.Time?$"TIME +{award.Amount}":$"+{award.Amount}",Font=displayFont,FontSize=award.Kind==BonusKind.Time?24:32,PixelSize=.008f,
                OutlineSize=9,OutlineModulate=new Color("09132b"),Billboard=BaseMaterial3D.BillboardModeEnum.Enabled,
                NoDepthTest=true,Shaded=false};
            AddChild(label);popups.Add((award,label));
        }
        for(int i=popups.Count-1;i>=0;i--) {
            var (award,label)=popups[i];float age=(float)((state.frame-award.Frame+fraction)*ArcadeSimulation.FrameSeconds);
            if(age>=1.7f){label.QueueFree();popups.RemoveAt(i);continue;}
            int extras=0;bool bug=false,rescue=false;
            foreach(var peer in popups)if(Math.Abs(peer.Award.Frame-award.Frame)<=2&&peer.Award.X==award.X){
                if(peer.Award.Kind==BonusKind.Bug)bug=true;if(peer.Award.Kind==BonusKind.Rescue)rescue=true;
            }
            extras=(bug?1:0)+(rescue?1:0);
            float lateral=award.Kind==BonusKind.Time&&extras>0?(award.X>160?-1:1)*(extras==2?2.25f:1.6f)
                :extras==2?(award.Kind==BonusKind.Bug?-.60f:award.Kind==BonusKind.Rescue?.60f:0):0;
            label.Position=Pos(award.X,Math.Max(32,award.Row),.83f)+camera.GlobalBasis.X*lateral+camera.GlobalBasis.Y*(age*.27f);
            // Keep the floating number below the score panel even at the top bay.
            float minY=140;
            float screenY=camera.UnprojectPosition(label.Position).Y;
            if(screenY<minY)label.Position-=camera.GlobalBasis.Y*((minY-screenY)*camera.Size/GetViewport().GetVisibleRect().Size.Y);
            var color=award.Kind==BonusKind.Time?new Color("7beaff"):award.Kind==BonusKind.Rescue?new Color("ff75da"):new Color("fff32f");
            color.A=Math.Clamp((1.7f-age)/.4f,0,1);label.Modulate=color;
        }
    }
    private void UpdateDeathRipple(float fraction) {
        bool active=frogVisual.Dying&&frogVisual.Drowning;
        if(!active){if(deathRipple!=null)deathRipple.Visible=false;return;}
        if(deathRipple==null) {
            rippleMaterial=new StandardMaterial3D{AlbedoColor=new Color(.65f,.88f,1,.6f),ShadingMode=BaseMaterial3D.ShadingModeEnum.Unshaded,Transparency=BaseMaterial3D.TransparencyEnum.Alpha};
            deathRipple=new MeshInstance3D{Mesh=new TorusMesh{InnerRadius=.42f,OuterRadius=.45f,Rings=32,RingSegments=6},MaterialOverride=rippleMaterial};AddChild(deathRipple);
        }
        float age=(float)frogVisual.DeathSeconds(state.frame,fraction);
        deathRipple.Visible=true;deathRipple.Position=Pos(frogVisual.DeathX,frogVisual.DeathRow,.025f);
        deathRipple.Scale=new Vector3(1+age*.6f,.18f,1+age*.6f);
        var tint=rippleMaterial!.AlbedoColor;tint.A=Math.Max(0,.65f-age*.4f);rippleMaterial.AlbedoColor=tint;
    }
}
