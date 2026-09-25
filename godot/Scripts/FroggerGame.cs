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
    private int facing=2,coinFrames,renderFrames,highScore;
    private string screenshot="";
    private ShaderMaterial water=null!;
    private Camera3D camera=null!;
    public override void _Ready() {
        try {
            foreach(string arg in OS.GetCmdlineUserArgs())if(arg.StartsWith("--screenshot="))screenshot=arg[13..];
            ReadReviewArgs();
            SetupWorld();SetupUi();LoadPreferences();ResetMachine();
            for(int i=0;i<180;i++)simulation.Step();ObserveFrame();ShowMenu(false);
            if(screenshot!=""&&!Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--menu-shot")){StartGame(1);for(int i=0;i<50;i++)simulation.Step();ObserveFrame();}
            if(screenshot!=""&&Array.Exists(OS.GetCmdlineUserArgs(),a=>a=="--later-board")){
                for(int round=0;round<2;round++){
                    for(int bay=0;bay<5;bay++){
                        simulation.Poke(0x8044,24+48*bay);simulation.Poke(0x8047,32);simulation.Poke(0x8004,0);simulation.Poke(0x83cd,0);simulation.Poke(0x8122,1);
                        for(int f=0;f<180;f++)simulation.Step();
                    }
                    for(int f=0;f<500;f++)simulation.Step();
                }
                ObserveFrame();simulation.Sound?.Samples.Clear();
            }
        } catch(Exception e){GD.PushError(e.ToString());if(message!=null){message.Text="Setup needed: run tools/setup.ps1\n"+e.Message;messagePanel.Visible=true;}SetProcess(false);}
    }
    private void ResetMachine(){simulation?.Dispose();simulation=new ArcadeSimulation(FileAccess.GetFileAsBytes("res://rom/maincpu.bin"),modern,FileAccess.GetFileAsBytes("res://rom/audiocpu.bin"));accumulator=0;ClearPresentation();}
    private void StartGame(int players) {
        ResetMachine();for(int i=0;i<180;i++)simulation.Step();
        for(int c=0;c<players;c++){for(int i=0;i<6;i++)simulation.Step(16);for(int i=0;i<10;i++)simulation.Step();}
        for(int i=0;i<6;i++)simulation.Step(players==1?32:64);
        for(int i=0;i<45;i++)simulation.Step();
        if(highScore>0){int n=highScore/10;simulation.Poke(0x83ef,(n%10)|((n/10%10)<<4));simulation.Poke(0x83f0,(n/100%10)|((n/1000%10)<<4));}
        ObserveFrame();simulation.Sound?.Samples.Clear();audioPlayer?.Stop();started=true;paused=false;menu.Visible=false;Input.MouseMode=Input.MouseModeEnum.Hidden;message.Text="";messagePanel.Visible=false;
    }
    public override void _Process(double delta) {
        if(simulation==null)return;
        presentationDelta=(float)Math.Clamp(delta,0,.1);
        if(review!=""){if(reviewCapturePending)return;StepReview();}
        else if(!paused){accumulator+=Math.Min(delta,.2);int steps=0;
            while(accumulator>=ArcadeSimulation.FrameSeconds&&steps++<12){int input=started?ReadDirection():0;if(coinFrames>0){input|=16;coinFrames--;}simulation.Step(input);ObserveFrame();accumulator-=ArcadeSimulation.FrameSeconds;}
        }
        UpdateFrogMotion();
        UpdateCamera(delta);
        FeedAudio();
        UpdateActors();UpdateHud();water.SetShaderParameter("clock",state.frame*(float)ArcadeSimulation.FrameSeconds);
        UpdateReviewCamera();
        if(review!="")CaptureReview();
        else if(screenshot!=""&&++renderFrames==10)Capture();
    }
    private async void Capture(){await ToSignal(RenderingServer.Singleton,RenderingServer.SignalName.FramePostDraw);var error=GetViewport().GetTexture().GetImage().SavePng(screenshot);GD.Print($"SCREENSHOT {screenshot} {error}");GetTree().Quit(error==Error.Ok?0:1);}
    private int ReadDirection(){
        var pads=Input.GetConnectedJoypads();int joy=pads.Count>0?pads[0]:-1;
        bool K(Key k)=>Input.IsPhysicalKeyPressed(k);bool P(JoyButton b)=>joy>=0&&Input.IsJoyButtonPressed(joy,b);
        float x=joy>=0?Input.GetJoyAxis(joy,JoyAxis.LeftX):0,y=joy>=0?Input.GetJoyAxis(joy,JoyAxis.LeftY):0;
        int direction=0;
        if(K(Key.Up)||K(Key.W)||P(JoyButton.DpadUp)||y<-.5f)direction=1;
        else if(K(Key.Down)||K(Key.S)||P(JoyButton.DpadDown)||y>.5f)direction=2;
        else if(K(Key.Left)||K(Key.A)||P(JoyButton.DpadLeft)||x<-.5f)direction=4;
        else if(K(Key.Right)||K(Key.D)||P(JoyButton.DpadRight)||x>.5f)direction=8;
        return AdaptDirection(direction);
    }
    private int AdaptDirection(int direction){
        if(direction!=0&&homeArrival.Active(state.frame,RenderFraction))homeArrival.Cancel();
        return InputPulse.ForHeldDirection(direction,state);
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
    private void Resume(){paused=false;menu.Visible=false;Input.MouseMode=Input.MouseModeEnum.Hidden;accumulator=0;}
    private ModelActor Actor(string key,string model){if(!actors.TryGetValue(key,out var actor)){actor=new ModelActor(this,model);actors.Add(key,actor);}return actor;}
    private static Vector3 Pos(float x,float row,float height=0)=>new((x-120)/16f,height,(row-128)/16f);
    private void SetupWorld(){
        AddChild(new WorldEnvironment{Environment=new Godot.Environment{BackgroundMode=Godot.Environment.BGMode.Color,BackgroundColor=new Color("171e29"),AmbientLightSource=Godot.Environment.AmbientSource.Color,AmbientLightColor=Colors.White,AmbientLightEnergy=.30f,TonemapMode=Godot.Environment.ToneMapper.Linear,SsaoEnabled=true,SsaoIntensity=1.45f,SsaoRadius=1.0f,SsaoSharpness=.55f,ReflectedLightSource=Godot.Environment.ReflectionSource.Disabled}});
        var sun=new DirectionalLight3D{LightColor=Colors.White,LightEnergy=1.05f,LightAngularDistance=1.5f,ShadowEnabled=true,ShadowBlur=1.8f,DirectionalShadowBlendSplits=true};
        AddChild(sun);
        // The source is above the board's top-left; shadows fall bottom-right.
        sun.LookAt(new Vector3(1.4f,-2f,1f),Vector3.Up);
        AddChild(new DirectionalLight3D{RotationDegrees=new Vector3(-40,140,0),LightColor=new Color("e8f0ff"),LightEnergy=.08f});
        AddChild(GD.Load<PackedScene>("res://Models/board.glb").Instantiate<Node3D>());
        camera=new Camera3D{Projection=Camera3D.ProjectionType.Orthogonal,Size=16.8f,Position=new Vector3(0,19,9.8f),KeepAspect=Camera3D.KeepAspectEnum.Height};AddChild(camera);camera.LookAt(new Vector3(0,0,-.12f));camera.Current=true;
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
    private void LoadPreferences(){if(screenshot!=""){muted=true;return;}var c=new ConfigFile();if(c.Load("user://settings.cfg")==Error.Ok){highScore=(int)c.GetValue("play","high_score",0);modern=(bool)c.GetValue("play","modern_collision",true);muted=(bool)c.GetValue("play","muted",false);perspectiveView=(bool)c.GetValue("play","perspective_view",false);followCamera=(bool)c.GetValue("play","follow_camera",false);}}
    private void SavePreferences(){if(screenshot!="")return;var c=new ConfigFile();c.SetValue("play","high_score",highScore);c.SetValue("play","modern_collision",modern);c.SetValue("play","muted",muted);c.SetValue("play","perspective_view",perspectiveView);c.SetValue("play","follow_camera",followCamera);c.Save("user://settings.cfg");}
    public override void _ExitTree(){simulation?.Dispose();}
}
