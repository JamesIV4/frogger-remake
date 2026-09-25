using Godot;
using System;
using System.Collections.Generic;
namespace FroggerRemake;

public partial class FroggerGame : Node3D
{
    private ArcadeSimulation simulation=null!;
    private FrameState state=new();
    private readonly Dictionary<string,ModelActor> actors=new();
    private AudioStreamPlayer audioPlayer=null!;
    private AudioStreamGeneratorPlayback audioPlayback=null!;
    private bool started,paused,modern=true,muted;
    private double accumulator;
    private int facing=2,wasHop,coinFrames,renderFrames,highScore;
    private string screenshot="";
    private ShaderMaterial water=null!;
    private Camera3D camera=null!;
    public override void _Ready() {
        try {
            foreach(string arg in OS.GetCmdlineUserArgs())if(arg.StartsWith("--screenshot="))screenshot=arg[13..];
            SetupWorld();SetupUi();LoadPreferences();ResetMachine();
            for(int i=0;i<180;i++)simulation.Step();state=simulation.Snapshot();ShowMenu(false);
            if(screenshot!=""&&!Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--menu-shot")){StartGame(1);for(int i=0;i<50;i++)simulation.Step();state=simulation.Snapshot();}
            if(screenshot!=""&&Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--later-board")){
                for(int round=0;round<2;round++){
                    for(int bay=0;bay<5;bay++){
                        simulation.Poke(0x8044,24+48*bay);simulation.Poke(0x8047,32);simulation.Poke(0x8004,0);simulation.Poke(0x83cd,0);simulation.Poke(0x8122,1);
                        for(int f=0;f<180;f++)simulation.Step();
                    }
                    for(int f=0;f<500;f++)simulation.Step();
                }
                state=simulation.Snapshot();simulation.Sound?.Samples.Clear();
            }
        } catch(Exception e){GD.PushError(e.ToString());if(message!=null)message.Text="Setup needed: run tools/setup.ps1\n"+e.Message;SetProcess(false);}
    }
    private void ResetMachine(){simulation?.Dispose();simulation=new ArcadeSimulation(FileAccess.GetFileAsBytes("res://rom/maincpu.bin"),modern,FileAccess.GetFileAsBytes("res://rom/audiocpu.bin"));accumulator=0;wasHop=0;}
    private void StartGame(int players) {
        ResetMachine();for(int i=0;i<180;i++)simulation.Step();
        for(int c=0;c<players;c++){for(int i=0;i<6;i++)simulation.Step(16);for(int i=0;i<10;i++)simulation.Step();}
        for(int i=0;i<6;i++)simulation.Step(players==1?32:64);
        for(int i=0;i<45;i++)simulation.Step();
        if(highScore>0){int n=highScore/10;simulation.Poke(0x83ef,(n%10)|((n/10%10)<<4));simulation.Poke(0x83f0,(n/100%10)|((n/1000%10)<<4));}
        state=simulation.Snapshot();simulation.Sound?.Samples.Clear();audioPlayer?.Stop();started=true;paused=false;menu.Visible=false;message.Text="";
    }
    public override void _Process(double delta) {
        if(simulation==null)return;
        var view=GetViewport().GetVisibleRect().Size;camera.Size=Math.Max(17.4f,16.2f/(view.X/view.Y));
        if(!paused){accumulator+=Math.Min(delta,.2);int steps=0;
            while(accumulator>=ArcadeSimulation.FrameSeconds&&steps++<12){int input=started?ReadDirection():0;if(coinFrames>0){input|=16;coinFrames--;}simulation.Step(input);accumulator-=ArcadeSimulation.FrameSeconds;}
            state=simulation.Snapshot();
        }
        FeedAudio();
        UpdateActors();UpdateHud();water.SetShaderParameter("clock",state.frame*(float)ArcadeSimulation.FrameSeconds);
        if(screenshot!=""&&++renderFrames==10)Capture();
    }
    private async void Capture(){await ToSignal(RenderingServer.Singleton,RenderingServer.SignalName.FramePostDraw);var error=GetViewport().GetTexture().GetImage().SavePng(screenshot);GD.Print($"SCREENSHOT {screenshot} {error}");GetTree().Quit(error==Error.Ok?0:1);}
    private int ReadDirection(){
        var pads=Input.GetConnectedJoypads();int joy=pads.Count>0?pads[0]:-1;
        bool K(Key k)=>Input.IsPhysicalKeyPressed(k);bool P(JoyButton b)=>joy>=0&&Input.IsJoyButtonPressed(joy,b);
        float x=joy>=0?Input.GetJoyAxis(joy,JoyAxis.LeftX):0,y=joy>=0?Input.GetJoyAxis(joy,JoyAxis.LeftY):0;
        if(K(Key.Up)||K(Key.W)||P(JoyButton.DpadUp)||y<-.5f)return 1;
        if(K(Key.Down)||K(Key.S)||P(JoyButton.DpadDown)||y>.5f)return 2;
        if(K(Key.Left)||K(Key.A)||P(JoyButton.DpadLeft)||x<-.5f)return 4;
        if(K(Key.Right)||K(Key.D)||P(JoyButton.DpadRight)||x>.5f)return 8;return 0;
    }
    public override void _UnhandledInput(InputEvent input){
        if(input is InputEventKey k&&k.Pressed&&!k.Echo)switch(k.PhysicalKeycode){
            case Key.Enter:case Key.Space:if(!started||state.At(0x83fe)==0)StartGame(1);else if(paused)Resume();break;
            case Key.Key1:StartGame(1);break;case Key.Key2:StartGame(2);break;
            case Key.Escape:case Key.P:if(started){paused=!paused;if(paused)ShowMenu(true);else Resume();}break;
            case Key.C:case Key.Key5:coinFrames=6;break;case Key.R:if(started)StartGame(1);break;
            case Key.M:muted=!muted;SavePreferences();break;
            case Key.F11:DisplayServer.WindowSetMode(DisplayServer.WindowGetMode()==DisplayServer.WindowMode.Fullscreen?DisplayServer.WindowMode.Windowed:DisplayServer.WindowMode.Fullscreen);break;
        }
        if(input is InputEventJoypadButton b&&b.Pressed){if(b.ButtonIndex==JoyButton.A&&!started)StartGame(1);if(b.ButtonIndex==JoyButton.Start){if(!started)StartGame(1);else{paused=!paused;if(paused)ShowMenu(true);else Resume();}}}
    }
    private void Resume(){paused=false;menu.Visible=false;accumulator=0;}
    private ModelActor Actor(string key,string model){if(!actors.TryGetValue(key,out var actor)){actor=new ModelActor(this,model);actors.Add(key,actor);}return actor;}
    private static Vector3 Pos(float x,float row,float height=0)=>new((x-120)/16f,height,(row-128)/16f);
    private void UpdateActors(){
        foreach(var a in actors.Values)a.Root.Visible=false;
        int fx=state.At(0x8044),fy=state.At(0x8047);var frog=Actor("player","frog");
        frog.Root.Visible=fx>=8&&fx<=240&&fy>=26&&fy<=232;frog.Root.Position=Pos(fx,fy,.11f);
        bool dead=state.At(0x8004)!=0&&state.At(0x83cd)==0;int hop=0;for(int i=0;i<4;i++)if(state.At(0x8248+i)!=0)hop=i+1;
        if(hop>0)facing=hop switch{1=>0,2=>2,3=>1,_=>3};frog.Root.Rotation=new Vector3(0,facing*Mathf.Pi/2,0);
        if(dead){frog.Play("Death",false);if(state.At(0x829c)!=0)frog.Root.Position+=new Vector3(0,-.025f*state.At(0x8247),0);}
        else if(hop>0){if(hop!=wasHop)frog.Play("Hop",false,1.12f);}else frog.Play("Idle");wasHop=hop;
        int[] widths={60,31,92,44,47,0,34,18,18,18,18};string[] models={"log","turtle","log","log","turtle","","truck","sport","car","dozer","racecar"};
        for(int index=0;index<11;index++){
            if(index==5)continue;int table=0x8100+index*9,count=Math.Min(8,state.At(table)),row=(index+3)*16,width=widths[index];
            for(int j=0;j<count;j++){
                float center=state.At(table+j+1)-(index<5?12:3)-width/2f;int members=index==1?2:index==4?3:1;
                for(int member=0;member<members;member++)for(int wrap=-1;wrap<=1;wrap++){
                    float x=center+wrap*256+(member-(members-1)/2f)*16;if(x<4||x>236)continue;
                    bool crocodile=index==0&&BoardVisuals.GatorOnLog(state,x,row);
                    var obj=Actor($"lane{index}.{j}.{member}.{wrap}.{crocodile}",crocodile?"gator":models[index]);obj.Root.Visible=true;obj.Root.Position=Pos(x,row,index<5?.01f:.02f);
                    if(crocodile){obj.Root.Rotation=new Vector3(0,Mathf.Pi/2,0);obj.Root.Scale=new Vector3(1,1,(width-3)/30f);obj.Play("Bite");continue;}
                    if(models[index]=="log")obj.Root.Scale=new Vector3((width-3)/16f,1,1);
                    else if(models[index]=="turtle"){int phase=BoardVisuals.TurtlePhase(state,x,row);obj.Root.Rotation=new Vector3(0,-Mathf.Pi/2,0);obj.Root.Position+=new Vector3(0,phase==2?-.63f:phase==1?-.18f:0,0);obj.Play(phase>0?"Dive":"Swim");}
                    else{obj.Root.Rotation=new Vector3(0,(index%2==0?-1:1)*Mathf.Pi/2,0);obj.Root.Scale=Vector3.One*.84f;obj.Play("Move");}
                }
            }
        }
        int homeBase=state.At(0x83fd)==2?0x8263:0x825e;
        for(int i=0;i<5;i++){
            if(state.At(homeBase+i)!=0){var a=Actor($"home{i}","frog");a.Root.Visible=true;a.Root.Position=new Vector3(-6+3*i,.12f,-6);a.Root.Rotation=new Vector3(0,Mathf.Pi,0);a.Play("Celebrate");}
            int tile=state.At(0xab64-i*0xc0);
            if(tile>=44&&tile<=47){var a=Actor($"homefly{i}","fly");a.Root.Visible=true;a.Root.Position=new Vector3(-6+3*i,.1f,-6);}
            if(tile==208||state.At(0xab64-i*0xc0+32)==208){var a=Actor($"homegator{i}","gator");a.Root.Visible=true;a.Root.Position=new Vector3(-6+3*i,tile==208?0:-.15f,-6);a.Root.Scale=Vector3.One*.6f;a.Play("Bite");}
        }
        foreach(int addr in new[]{0x8048,0x8050,0x8058}){
            int x=state.At(addr),y=state.At(addr+3);if(x<8||x>235||y<32||y>136||state.At(addr+1)==0)continue;
            var a=Actor($"hazard{addr}",addr==0x8058?"otter":"snake");a.Root.Visible=true;a.Root.Position=Pos(x,y,.08f);a.Root.Rotation=new Vector3(0,Mathf.Pi/2,0);a.Root.Scale=Vector3.One*.75f;a.Play("Move");
        }
        int bx=state.At(0x8040),by=state.At(0x8043);
        if(bx>7&&bx<235&&by>=32&&by<128&&state.At(0x8041)!=0){var a=Actor("lady","frog");a.Root.Visible=true;a.Root.Position=Pos(bx,by,.15f);a.Root.Scale=Vector3.One*.76f;a.Root.Rotation=new Vector3(0,Mathf.Pi,0);}
    }
    private void SetupWorld(){
        AddChild(new WorldEnvironment{Environment=new Godot.Environment{BackgroundMode=Godot.Environment.BGMode.Color,BackgroundColor=new Color("171e29"),AmbientLightSource=Godot.Environment.AmbientSource.Color,AmbientLightColor=Colors.White,AmbientLightEnergy=.25f,TonemapMode=Godot.Environment.ToneMapper.Aces,ReflectedLightSource=Godot.Environment.ReflectionSource.Disabled}});
        AddChild(new DirectionalLight3D{RotationDegrees=new Vector3(-58,-30,0),LightColor=Colors.White,LightEnergy=1.05f,ShadowEnabled=true});
        AddChild(new DirectionalLight3D{RotationDegrees=new Vector3(-40,140,0),LightColor=new Color("e8f0ff"),LightEnergy=.14f});
        AddChild(GD.Load<PackedScene>("res://Models/board.glb").Instantiate<Node3D>());
        camera=new Camera3D{Projection=Camera3D.ProjectionType.Orthogonal,Size=17.4f,Position=new Vector3(0,19,9.8f),KeepAspect=Camera3D.KeepAspectEnum.Height};AddChild(camera);camera.LookAt(new Vector3(0,0,.15f));camera.Current=true;
        var surface=new MeshInstance3D{Mesh=new PlaneMesh{Size=new Vector2(14,5),SubdivideWidth=64,SubdivideDepth=32},Position=new Vector3(0,-.025f,-3)};
        water=new ShaderMaterial{Shader=GD.Load<Shader>("res://Shaders/river.gdshader")};surface.MaterialOverride=water;AddChild(surface);
    }
    private void FeedAudio(){
        if(audioPlayer==null){audioPlayer=new AudioStreamPlayer{Stream=new AudioStreamGenerator{MixRate=48000,BufferLength=.12f},VolumeDb=-9};AddChild(audioPlayer);}
        if(simulation.Sound==null)return;
        var samples=simulation.Sound.Samples;
        // Godot forbids ClearBuffer while a generator is active. Stop releases
        // the playback, preventing old notes from leaking through on resume.
        if(muted||paused||!started){samples.Clear();if(audioPlayer.Playing)audioPlayer.Stop();return;}
        if(!audioPlayer.Playing){audioPlayer.Play();audioPlayback=(AudioStreamGeneratorPlayback)audioPlayer.GetStreamPlayback();}
        int n=Math.Min(samples.Count,audioPlayback.GetFramesAvailable());
        if(n>0){var buffer=new Vector2[n];for(int i=0;i<n;i++){float v=samples.Dequeue();buffer[i]=new Vector2(v,v);}audioPlayback.PushBuffer(buffer);}
    }
    private void LoadPreferences(){if(screenshot!=""){muted=true;return;}var c=new ConfigFile();if(c.Load("user://settings.cfg")==Error.Ok){highScore=(int)c.GetValue("play","high_score",0);modern=(bool)c.GetValue("play","modern_collision",true);muted=(bool)c.GetValue("play","muted",false);}}
    private void SavePreferences(){if(screenshot!="")return;var c=new ConfigFile();c.SetValue("play","high_score",highScore);c.SetValue("play","modern_collision",modern);c.SetValue("play","muted",muted);c.Save("user://settings.cfg");}
    public override void _ExitTree(){simulation?.Dispose();}
}
