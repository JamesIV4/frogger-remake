using Godot;
using System;
using System.Collections.Generic;
namespace FroggerRemake;

public partial class FroggerGame
{
    private readonly FrogVisualState frogVisual=new();
    private FrameState previousState=new();
    private readonly List<(BonusAward Award,Label3D Label)> popups=new();
    private MeshInstance3D? deathRipple;
    private StandardMaterial3D? rippleMaterial;

    private void ObserveFrame() {
        previousState=state;state=simulation.Snapshot();frogVisual.Observe(state);
    }
    private void ClearPresentation() {
        frogVisual.Reset();facing=2;
        foreach(var popup in popups)popup.Label.QueueFree();popups.Clear();
    }
    private float RenderFraction=>paused?0:(float)Math.Clamp(accumulator/ArcadeSimulation.FrameSeconds,0,1);
    private float BlendCoordinate(int address,float fraction) =>previousState.frame+1==state.frame
        ?BoardVisuals.InterpolateByte(previousState.At(address),state.At(address),fraction):state.At(address);

    private void UpdateActors() {
        float fraction=RenderFraction;
        foreach(var actor in actors.Values)actor.Root.Visible=false;
        UpdatePlayer(fraction);
        UpdateLanes(fraction);
        UpdateHomesAndHazards();
        UpdateBonuses(fraction);
    }
    private void UpdatePlayer(float fraction) {
        var player=Actor("player","frog");
        player.Root.Scale=Vector3.One;
        player.Root.Visible=FrogVisualState.PlayerOnBoard(state);
        int hop=0;for(int i=0;i<4;i++)if(state.At(0x8248+i)!=0)hop=i+1;
        if(hop>0&&!frogVisual.Dying)facing=hop switch{1=>0,2=>2,3=>1,_=>3};
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
            player.Root.Position=Pos(x,row,BoardVisuals.SurfaceHeight(row));
            if(hop>0){int counter=state.At(0x824f+hop);player.Pose("Hop",Math.Max(0,8-counter+fraction)*ArcadeSimulation.FrameSeconds*1.12);}
            else player.Play("Idle");
        }
        if(frogVisual.Carrying) {
            if(player.PassengerSocket==null)throw new InvalidOperationException("The Blender frog rig has no PassengerSocket");
            if(!actors.TryGetValue("passenger",out var passenger)) {
                passenger=new ModelActor(player.PassengerSocket,"lady_frog");actors.Add("passenger",passenger);
            }
            passenger.Root.Visible=true;passenger.Root.Position=Vector3.Zero;
            passenger.Root.Rotation=Vector3.Zero;passenger.Root.Scale=Vector3.One*.58f;passenger.Play("Idle");
        }
        UpdateDeathRipple(fraction);
    }
    private void UpdateLanes(float fraction) {
        int[] widths={60,31,92,44,47,0,34,18,18,18,18};
        string[] models={"log","turtle","log","log","turtle","","truck","sport","car","dozer","racecar"};
        for(int lane=0;lane<11;lane++) {
            if(lane==5)continue;
            int table=0x8100+lane*9,count=Math.Min(8,state.At(table)),row=(lane+3)*16,width=widths[lane];
            bool turtle=models[lane]=="turtle";
            for(int index=0;index<count;index++) {
                float rawCenter=state.At(table+index+1)-(lane<5?12:3)-width/2f;
                float center=BlendCoordinate(table+index+1,fraction)-(lane<5?12:3)-width/2f;
                int members=lane==1?2:lane==4?3:1;
                float depth=turtle?BoardVisuals.TurtleDepth(state,rawCenter,row,fraction):0;
                bool crocodile=lane==0&&BoardVisuals.GatorOnLog(state,rawCenter,row);
                for(int member=0;member<members;member++)for(int wrap=-1;wrap<=1;wrap++) {
                    float x=center+wrap*256+(member-(members-1)/2f)*16;
                    float halfWidth=lane<5&&!turtle?(width-3)*.52f:turtle?9:lane==6?15:10;
                    // Keep a wrapping replica until the LAST visible part leaves
                    // the board. The shader clips the portion beyond the rail.
                    if(!BoardVisuals.IntersectsPlayfield(x,halfWidth))continue;
                    var obj=Actor($"lane{lane}.{index}.{member}.{wrap}.{crocodile}",crocodile?"gator":models[lane]);
                    obj.Root.Visible=true;obj.Root.Position=Pos(x,row,lane<5?.01f:.02f);
                    if(crocodile){obj.Root.Rotation=new Vector3(0,Mathf.Pi/2,0);obj.Root.Scale=new Vector3(1,1,(width-3)/30f);obj.Play("Bite");}
                    else if(models[lane]=="log")obj.Root.Scale=new Vector3((width-3)/16f,1,1);
                    else if(turtle){obj.Root.Position+=new Vector3(0,-depth,0);obj.Root.Rotation=new Vector3(0,-Mathf.Pi/2,0);obj.Play(depth>.01f?"Dive":"Swim");}
                    else{obj.Root.Rotation=new Vector3(0,(lane%2==0?-1:1)*Mathf.Pi/2,0);obj.Root.Scale=Vector3.One*.84f;obj.Play("Move");}
                }
            }
        }
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
            var a=Actor($"hazard{addr}",addr==0x8058?"otter":"snake");a.Root.Visible=true;a.Root.Position=Pos(x,y,.12f);a.Root.Rotation=new Vector3(0,Mathf.Pi/2,0);a.Root.Scale=Vector3.One*.75f;a.Play("Move");
        }
        int bx=state.At(0x8040),by=state.At(0x8043);
        if(!frogVisual.Carrying&&state.At(0x8135)!=0&&bx>7&&bx<235&&by>=32&&by<128&&state.At(0x8041)!=0x19) {
            var a=Actor("lady","lady_frog");a.Root.Visible=true;a.Root.Position=Pos(bx,by,.42f);
            a.Root.Scale=Vector3.One*.76f;a.Root.Rotation=new Vector3(0,Mathf.Pi,0);a.Play("Idle");
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
            float minY=(GetViewport().GetVisibleRect().Size.Y-960)/2+140;
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
