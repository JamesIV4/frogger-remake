using Godot;
using System;
using System.Collections.Generic;
namespace FroggerRemake;

public partial class FroggerGame
{
    private readonly FrogVisualState frogVisual=new();
    private readonly HomeArrivalVisual homeArrival=new();
    private readonly PresentationMotion movingVisuals=new();
    private readonly PresentationMotion frogMotion=new(4,.35f,72f);
    private float presentationDelta;
    private float displayedFrogX,displayedFrogRow;
    private sealed record BonusPopup(BonusAward Award,Control View,Vector2 AnchorUv,string DisplayText);
    private readonly List<BonusPopup> popups=new();
    private MeshInstance3D? deathRipple;
    private StandardMaterial3D? rippleMaterial;
    private readonly HashSet<(int Lane,int Group)> knownDivingGroups=new();
    private readonly List<(float X,int Row,float Depth)> turtleSupports=new();
    private readonly Dictionary<int,int> snakeFacing=new();
    private readonly HomeGatorVisual[] homeGatorVisuals={new(),new(),new(),new(),new()};
    private int anchoredDeathFrame=-1,lastLivePlayerFrame=-1;
    private float deathInitialHeight,lastLivePlayerHeight,lastLivePlayerX,lastLivePlayerRow;
    private int knownDivingLevel=-1;
    private int ladyFacing=2;
    private int ladyHopStartFrame,ladyLastMotionFrame;
    private const float LadyInRiverScale=.62f,PassengerScale=.76f;

    private void ObserveFrame() {
        state=simulation.Snapshot();frogVisual.Observe(state);TrackLadyHop();
    }
    private void ClearPresentation() {
        frogVisual.Reset();homeArrival.Reset();movingVisuals.Reset();frogMotion.Reset();snakeFacing.Clear();foreach(var gator in homeGatorVisuals)gator.Reset();facing=2;ladyFacing=2;ladyHopStartFrame=ladyLastMotionFrame=anchoredDeathFrame=lastLivePlayerFrame=-1;knownDivingGroups.Clear();knownDivingLevel=-1;
        foreach(var popup in popups)popup.View.QueueFree();popups.Clear();
    }
    private float RenderFraction=>paused?0:(float)Math.Clamp(accumulator/ArcadeSimulation.FrameSeconds,0,1);
    private void UpdateFrogMotion(){
        displayedFrogX=frogMotion.Step(0,state.At(0x8044),state.frame,presentationDelta,paused);
        displayedFrogRow=frogMotion.Step(1,state.At(0x8047),state.frame,presentationDelta,paused);
    }
    private void TrackLadyHop() {
        if(state.At(0x8135)==0||frogVisual.Carrying)return;
        // ROM sprite 0x21 / 0xa1 is the actual patrol heading. X also drifts
        // with the log, so its sign can reverse after a left hop without a
        // new pink-frog hop. Neutral 0x1e keeps the last facing direction.
        // ROM path 0x279f walks EE, EC, EA... as the index grows: unflipped
        // 0x21 travels LEFT; flipped 0xa1 walks the table backward, RIGHT.
        int code=state.At(0x8041),direction=code==0x21?3:code==0xa1?1:0;
        if(ladyFacing==2)ladyFacing=(state.At(0x833d)&0x80)!=0?1:3;
        if(direction==0)return;
        if(ladyHopStartFrame==0||direction!=ladyFacing||state.frame-ladyHopStartFrame>=12)ladyHopStartFrame=state.frame;
        ladyFacing=direction;ladyLastMotionFrame=state.frame;
    }

    private void UpdateActors() {
        float fraction=RenderFraction;
        foreach(var actor in actors.Values)actor.Root.Visible=false;
        UpdateBonuses(fraction);
        UpdateLanes(fraction);
        UpdatePlayer(fraction);
        UpdateHomesAndHazards();
    }
    private void UpdatePlayer(float fraction) {
        var player=Actor("player","frog");
        player.Root.Scale=Vector3.One;
        player.SetSquashColor(0);
        bool holdingHome=homeArrival.Active(state.frame,fraction);
        player.Root.Visible=holdingHome||FrogVisualState.PlayerOnBoard(state);
        int hop=frogVisual.HopDirection;
        if(frogVisual.HopActive(state.frame))facing=hop switch{1=>0,2=>2,3=>1,_=>3};
        player.Root.Rotation=new Vector3(0,facing*Mathf.Pi/2,0);
        if(holdingHome){
            bool finishing=homeArrival.FinishingHop(state.frame,fraction);
            float homeRow=homeArrival.VisualRow(state.frame,fraction);
            player.Root.Position=Pos(homeArrival.VisualX(state.frame,fraction),homeRow,finishing?BoardVisuals.SurfaceHeight(homeRow):.08f);
            player.Root.Rotation=new Vector3(0,Mathf.Pi,0);
            if(finishing)player.Pose("Hop",homeArrival.HopPoseSeconds(state.frame,fraction));
            else player.Play("Celebrate");
        } else if(frogVisual.Dying) {
            // Freeze the impact point and sample one continuous clip. 0x8247
            // resets every 16 frames and must never be used as sink height.
            double age=frogVisual.DeathSeconds(state.frame,fraction);
            if(anchoredDeathFrame!=frogVisual.DeathFrame){
                deathInitialHeight=BoardVisuals.SurfaceHeight(frogVisual.DeathRow);
                if(frogVisual.Drowning){
                    deathInitialHeight-=TurtleRideDepth(frogVisual.DeathX,frogVisual.DeathRow);
                    if(lastLivePlayerFrame>=frogVisual.DeathFrame-2&&
                        Math.Abs(lastLivePlayerX-frogVisual.DeathX)<=12&&Math.Abs(lastLivePlayerRow-frogVisual.DeathRow)<=8)
                        deathInitialHeight=Math.Min(deathInitialHeight,lastLivePlayerHeight);
                }
                anchoredDeathFrame=frogVisual.DeathFrame;
            }
            float height=deathInitialHeight;
            if(frogVisual.Drowning)height=FrogVisualState.DrownHeight(BoardVisuals.SurfaceHeight(frogVisual.DeathRow),deathInitialHeight,age);
            else {
                float squash=FrogVisualState.SquashProgress(age);
                if(frogVisual.DeathRow<=144||frogVisual.DeathRow>=208)
                    height=Mathf.Lerp(height,Math.Max(height,.09f),squash);
                player.Root.Scale=new Vector3(1+.50f*squash,1-.92f*squash,1+.40f*squash);
                player.SetSquashColor(squash);
            }
            player.Root.Position=Pos(frogVisual.DeathX,frogVisual.DeathRow,height);
            player.Pose(frogVisual.Drowning?"Drown":"Squash",age);
        } else {
            float x=displayedFrogX,row=displayedFrogRow;
            player.Root.Position=Pos(x,row,BoardVisuals.SurfaceHeight(row)-TurtleRideDepth(x,row));
            lastLivePlayerHeight=player.Root.Position.Y;lastLivePlayerX=x;lastLivePlayerRow=row;lastLivePlayerFrame=state.frame;
            if(frogVisual.HopActive(state.frame))player.Pose("Hop",frogVisual.HopSeconds(state.frame,fraction));
            else player.Play("Idle");
        }
        if(frogVisual.Carrying||holdingHome&&homeArrival.Passenger) {
            if(player.PassengerSocket==null)throw new InvalidOperationException("The Blender frog rig has no PassengerSocket");
            if(!actors.TryGetValue("passenger",out var passenger)) {
                passenger=new ModelActor(player.PassengerSocket,"lady_frog");actors.Add("passenger",passenger);
            }
            passenger.Root.Visible=true;passenger.Root.Position=Vector3.Zero;
            passenger.Root.Scale=Vector3.One*PassengerScale;
            passenger.Root.GlobalRotation=new Vector3(0,player.Root.GlobalRotation.Y,0);
            if(holdingHome){
                if(homeArrival.FinishingHop(state.frame,fraction))passenger.Pose("Hop",homeArrival.HopPoseSeconds(state.frame,fraction));
                else passenger.Play("Celebrate");
            }
            else if(frogVisual.HopActive(state.frame))passenger.Pose("Hop",frogVisual.HopSeconds(state.frame,fraction));
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
                float center=movingVisuals.Step(lane*16+index,rawCenter,state.frame,presentationDelta,paused);
                int members=lane==1?2:lane==4?3:1;
                if(turtle&&BoardVisuals.TurtlePhase(state,rawCenter,row)>0)knownDivingGroups.Add((lane,index));
                float depth=turtle?BoardVisuals.TurtleDepth(state,rawCenter,row,fraction,knownDivingGroups.Contains((lane,index))):0;
                bool crocodile=lane==0&&BoardVisuals.GatorOnLog(state,rawCenter,row);
                for(int member=0;member<members;member++)for(int wrap=-1;wrap<=1;wrap++) {
                    float x=center+wrap*256+(member-(members-1)/2f)*16;
                    if(turtle)turtleSupports.Add((x,row,depth));
                    float halfWidth=lane<5&&!turtle?(width-3)*.52f:turtle?9:lane==6?15:10;
                    var gatorFit=crocodile?BoardVisuals.FitRiverGator(width):default;
                    // Native 0x8101 is the river gator's front collision edge.
                    // The lethal interval is the preceding 16 pixels; align the
                    // model tip there instead of centering it on the log.
                    float renderX=x+gatorFit.CenterOffsetPixels;
                    // Keep a wrapping replica until the LAST visible part leaves
                    // the board. The shader clips the portion beyond the rail.
                    if(!BoardVisuals.IntersectsPlayfield(renderX,halfWidth))continue;
                    var obj=Actor($"lane{lane}.{index}.{member}.{wrap}.{crocodile}",crocodile?"river_gator":models[lane]);
                    obj.Root.Visible=true;obj.Root.Position=Pos(renderX,row,lane<5?(turtle?-.22f:-.18f):.02f);
                    if(crocodile){
                        // The river model keeps the home gator's sculpted upper
                        // jaw, with a shorter snout and the original long tail.
                        obj.Root.Rotation=new Vector3(0,Mathf.Pi/2,0);
                        obj.Root.Scale=new Vector3(gatorFit.WidthScale,.9f,gatorFit.LengthScale);
                        obj.Play("Bite");
                    }
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
            bool heldHere=homeArrival.Active(state.frame,RenderFraction)&&Math.Abs(homeArrival.X-(24+48*i))<=8;
            if(state.At(homeBase+i)!=0&&!heldHere){var a=Actor($"home{i}","frog");a.Root.Visible=true;a.Root.Position=new Vector3(-6+3*i,.08f,-6);a.Root.Rotation=new Vector3(0,Mathf.Pi,0);a.Play("Celebrate");}
            int tile=state.At(0xab64-i*0xc0);
            if(tile>=44&&tile<=47){var a=Actor($"homefly{i}","fly");a.Root.Visible=true;a.Root.Position=new Vector3(-6+3*i,.12f,-6);}
            bool nativeGator=tile==208||state.At(0xab64-i*0xc0+32)==208;
            float nativeReveal=nativeGator?BoardVisuals.HomeGatorReveal(state,tile==208,RenderFraction):0;
            float? reveal=homeGatorVisuals[i].Reveal(state.frame+RenderFraction,nativeGator,nativeReveal);
            if(reveal.HasValue){
                var a=Actor($"homegator{i}","gator");a.Root.Visible=true;
                // On ROM tile removal, quickly reverse the same hedge path.
                // Only presentation lingers; the native home hazard is gone.
                a.Root.Position=new Vector3(-6+3*i,.10f,-7.03f+.75f*reveal.Value);
                a.Root.Scale=Vector3.One*.6f;a.Play(nativeGator?"Bite":"Idle");
            }
        }
        foreach(int addr in new[]{0x8048,0x8050,0x8058}) {
            int x=state.At(addr),y=state.At(addr+3);if(x<8||x>235||y<32||y>136||state.At(addr+1)==0)continue;
            bool snake=addr!=0x8058;
            var a=Actor($"hazard{addr}",snake?"snake":"otter");a.Root.Visible=true;
            // The snake's continuous body rests on the inset log surface,
            // including its actual mesh bottom, rather than hovering above it.
            float surface=snake&&y<128?-.18f+ModelFootprints.LogTopTiles:BoardVisuals.SurfaceHeight(y);
            float height=snake?surface-.75f*ModelFootprints.SnakeBottomTiles+.008f:surface+.01f;
            int motionId=1000+addr;
            a.Root.Position=Pos(movingVisuals.Step(motionId,x,state.frame,presentationDelta,paused),y,height);
            int heading=1;
            if(snake){
                // ROM 0x29f9 flips the sprite's own drift bit, but lane scroll
                // can still carry the rendered snake the other way. Face the
                // visible travel direction and retain it through brief stalls.
                if(!snakeFacing.TryGetValue(addr,out heading))heading=(state.At(addr+1)&0x80)!=0?1:-1;
                float speed=movingVisuals.VelocityX(motionId);
                if(Math.Abs(speed)>.06f)heading=Math.Sign(speed);
                snakeFacing[addr]=heading;
            }
            // The modeled head is local -Y, which becomes screen-right at +90.
            a.Root.Rotation=new Vector3(0,heading*Mathf.Pi/2,0);
            a.Root.Scale=Vector3.One*.75f;a.Play("Move");
        }
        int bx=state.At(0x8040),by=state.At(0x8043);
        if(!frogVisual.Carrying&&state.At(0x8135)!=0&&bx>7&&bx<235&&by>=32&&by<128&&state.At(0x8041)!=0x19) {
            var a=Actor("lady","lady_frog");a.Root.Visible=true;a.Root.Position=Pos(movingVisuals.Step(50000,bx,state.frame,presentationDelta,paused),by,BoardVisuals.SurfaceHeight(by));
            a.Root.Scale=Vector3.One*LadyInRiverScale;
            a.Root.Rotation=new Vector3(0,ladyFacing*Mathf.Pi/2,0);
            if(ladyHopStartFrame>0&&ladyLastMotionFrame>=ladyHopStartFrame&&state.frame-ladyHopStartFrame<12)
                a.Pose("Hop",Math.Clamp((state.frame-ladyHopStartFrame+RenderFraction)/12.0,0,1)*(10.0/60.0));
            else a.Play("Idle");
        }
    }
    private void UpdateBonuses(float fraction) {
        var incoming=new List<BonusAward>();
        while(simulation.BonusAwards.TryDequeue(out var award))incoming.Add(award);
        bool rescued=false;foreach(var award in incoming)rescued|=award.Kind==BonusKind.Rescue;
        Vector2 viewport=GetViewport().GetVisibleRect().Size;
        foreach(var award in incoming){
            Control view;string displayText;
            Vector2 screen;
            if(award.Kind==BonusKind.Time){
                homeArrival.Begin(award,rescued,frogVisual.HopSeconds(award.Frame,0));
                displayText=$"TIME BONUS +{award.Amount}";
                var panel=new PanelContainer{Size=new Vector2(Math.Min(370,viewport.X-24),106),MouseFilter=Control.MouseFilterEnum.Ignore};
                var style=Style(new Color(.06f,.10f,.20f,.94f),12,2);style.BorderColor=new Color("ffce57");
                style.ContentMarginTop=8;style.ContentMarginBottom=8;
                panel.AddThemeStyleboxOverride("panel",style);
                var content=new VBoxContainer{Alignment=BoxContainer.AlignmentMode.Center};content.AddThemeConstantOverride("separation",4);panel.AddChild(content);
                var heading=Text("TIME BONUS",20,Cream);heading.AddThemeFontOverride("font",displayFont);content.AddChild(heading);
                content.AddChild(Text($"+{award.Amount}",29,new Color("7beaff")));
                view=panel;screen=viewport*.5f;
            } else {
                displayText=$"+{award.Amount}";
                var label=Text(displayText,28,award.Kind==BonusKind.Rescue?new Color("ff75da"):new Color("fff32f"));
                label.Size=new Vector2(180,50);label.MouseFilter=Control.MouseFilterEnum.Ignore;
                label.AddThemeColorOverride("font_shadow_color",Colors.Black);
                label.AddThemeConstantOverride("shadow_offset_y",3);
                view=label;
                screen=camera.UnprojectPosition(Pos(award.X,Math.Max(32,award.Row),.83f));
                screen.X+=award.Kind==BonusKind.Bug?-65:65;
                screen.X=Math.Clamp(screen.X,90,Math.Max(90,viewport.X-90));
                screen.Y=Math.Clamp(screen.Y,Math.Min(140,viewport.Y*.2f),Math.Max(140,viewport.Y-120));
            }
            bonusOverlay.AddChild(view);
            popups.Add(new BonusPopup(award,view,new Vector2(screen.X/viewport.X,screen.Y/viewport.Y),displayText));
        }
        for(int i=popups.Count-1;i>=0;i--){
            var popup=popups[i];float age=(float)Math.Max(0,(state.frame-popup.Award.Frame+fraction)*ArcadeSimulation.FrameSeconds);
            bool timeBonus=popup.Award.Kind==BonusKind.Time;
            float lifetime=timeBonus?1.55f:1.7f;
            if(age>=lifetime){popup.View.QueueFree();popups.RemoveAt(i);continue;}
            if(timeBonus)popup.View.Size=new Vector2(Math.Min(370,viewport.X-24),106);
            Vector2 origin=popup.AnchorUv*viewport;
            popup.View.Position=origin-popup.View.Size*.5f+(timeBonus?Vector2.Zero:new Vector2(0,-age*36));
            float opacity=timeBonus?Math.Min(1,age/.08f):1;
            opacity*=Math.Clamp((lifetime-age)/(timeBonus?.22f:.35f),0,1);
            popup.View.Modulate=new Color(1,1,1,opacity);
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
