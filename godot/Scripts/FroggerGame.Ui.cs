using Godot;
using System;
namespace FroggerRemake;
public partial class FroggerGame
{
    private Label score=null!,high=null!,level=null!,lives=null!,message=null!,playerHeader=null!;
    private ProgressBar timer=null!;
    private PanelContainer menu=null!,messagePanel=null!;
    private VBoxContainer menuItems=null!;
    private Control bonusOverlay=null!;
    private Font displayFont=null!,bodyFont=null!;
    private static readonly Color Cream=new("eee6c9"),Red=new("ff6655"),Lime=new("b6df56");
    private StyleBoxFlat Style(Color color,int radius=12,int border=0){
        var s=new StyleBoxFlat{BgColor=color,CornerRadiusTopLeft=radius,CornerRadiusTopRight=radius,CornerRadiusBottomLeft=radius,CornerRadiusBottomRight=radius,ContentMarginLeft=22,ContentMarginRight=22,ContentMarginTop=14,ContentMarginBottom=14};s.SetBorderWidthAll(border);s.BorderColor=new Color("70614e");return s;
    }
    private StyleBoxFlat EdgePanelStyle(Color color,bool atTop){
        var style=Style(color,0,2);
        if(atTop){style.CornerRadiusBottomLeft=16;style.CornerRadiusBottomRight=16;}
        else{style.CornerRadiusTopLeft=16;style.CornerRadiusTopRight=16;}
        return style;
    }
    private Label Text(string text,int size,Color color){
        var l=new Label{Text=text,HorizontalAlignment=HorizontalAlignment.Center};l.AddThemeFontSizeOverride("font_size",size);
        l.AddThemeFontOverride("font",size>=23?displayFont:bodyFont);l.AddThemeColorOverride("font_color",color);
        if(size>=23){l.AddThemeColorOverride("font_shadow_color",new Color(0,0,0,.45f));l.AddThemeConstantOverride("shadow_offset_y",2);}
        return l;
    }
    private void SetupUi(){
        displayFont=GD.Load<FontFile>("res://Fonts/PressStart2P-Regular.ttf");bodyFont=GD.Load<FontFile>("res://Fonts/Silkscreen-Regular.ttf");
        var ui=new CanvasLayer();AddChild(ui);
        var root=new Control();ui.AddChild(root);root.MouseFilter=Control.MouseFilterEnum.Ignore;
        void LayoutRoot(){var size=GetViewport().GetVisibleRect().Size;root.Size=new Vector2(1100,size.Y);root.Position=new Vector2((size.X-1100)/2,0);}
        LayoutRoot();GetViewport().SizeChanged+=LayoutRoot;
        root.Theme=new Theme{DefaultFont=bodyFont,DefaultFontSize=17};
        var frameColor=new Color("202733");
        var header=new PanelContainer{Position=new Vector2(115,0),Size=new Vector2(870,94)};header.AddThemeStyleboxOverride("panel",EdgePanelStyle(frameColor,true));root.AddChild(header);
        var row=new HBoxContainer{Alignment=BoxContainer.AlignmentMode.Center};row.AddThemeConstantOverride("separation",110);header.AddChild(row);
        foreach(string title in new[]{"1-UP","FROGGER","HI-SCORE"}){
            var col=new VBoxContainer{Alignment=BoxContainer.AlignmentMode.Center};row.AddChild(col);
            var heading=Text(title,title.Contains('F')?23:16,Cream);col.AddChild(heading);
            if(title=="1-UP"){playerHeader=heading;score=Text("00000",26,Red);col.AddChild(score);}
            else if(title=="HI-SCORE"){high=Text("00000",26,Red);col.AddChild(high);}
            else{level=Text("THE GREAT CROSSING",12,Lime);col.AddChild(level);}
        }
        var bottom=new PanelContainer{Size=new Vector2(870,64)};bottom.AddThemeStyleboxOverride("panel",EdgePanelStyle(frameColor,false));root.AddChild(bottom);
        var foot=new HBoxContainer{Alignment=BoxContainer.AlignmentMode.Center};foot.AddThemeConstantOverride("separation",26);bottom.AddChild(foot);
        lives=Text("FROGS   ●●●",19,Lime);lives.CustomMinimumSize=new Vector2(225,0);foot.AddChild(lives);
        timer=new ProgressBar{MinValue=0,MaxValue=100,Value=100,ShowPercentage=false,CustomMinimumSize=new Vector2(330,18),SizeFlagsVertical=Control.SizeFlags.ShrinkCenter};timer.AddThemeStyleboxOverride("background",Style(new Color("121a29"),4));timer.AddThemeStyleboxOverride("fill",Style(new Color("9ac75d"),4));foot.AddChild(timer);foot.AddChild(Text("TIME",19,Cream));
        messagePanel=new PanelContainer{Visible=false,MouseFilter=Control.MouseFilterEnum.Ignore};
        var messageStyle=Style(new Color(.06f,.10f,.17f,.93f),12,2);messageStyle.BorderColor=new Color("ffce57");
        messagePanel.AddThemeStyleboxOverride("panel",messageStyle);root.AddChild(messagePanel);
        message=Text("",30,Cream);message.AutowrapMode=TextServer.AutowrapMode.WordSmart;message.VerticalAlignment=VerticalAlignment.Center;
        message.MouseFilter=Control.MouseFilterEnum.Ignore;
        message.AddThemeColorOverride("font_shadow_color",Colors.Black);message.AddThemeConstantOverride("shadow_offset_y",3);messagePanel.AddChild(message);
        menu=new PanelContainer{Size=new Vector2(440,484)};menu.AddThemeStyleboxOverride("panel",Style(new Color(.075f,.10f,.15f,.97f),18,2));root.AddChild(menu);
        menuItems=new VBoxContainer();menuItems.AddThemeConstantOverride("separation",14);menu.AddChild(menuItems);
        void LayoutOverlay(){
            var viewport=GetViewport().GetVisibleRect().Size;float height=root.Size.Y;
            bottom.Position=new Vector2(115,height-64);
            float messageWidth=Math.Min(840,Math.Max(120,viewport.X-48));
            messagePanel.Size=new Vector2(messageWidth,180);
            messagePanel.Position=new Vector2((1100-messageWidth)/2,(height-180)/2);
            message.CustomMaximumSize=new Vector2(messageWidth-44,150);
            message.AddThemeFontSizeOverride("font_size",viewport.X<620?18:viewport.X<900?24:30);
            menu.Position=new Vector2(330,(height-484)/2);
        }
        LayoutOverlay();GetViewport().SizeChanged+=LayoutOverlay;
        bonusOverlay=new Control{Size=GetViewport().GetVisibleRect().Size,MouseFilter=Control.MouseFilterEnum.Ignore};ui.AddChild(bonusOverlay);
        GetViewport().SizeChanged+=()=>bonusOverlay.Size=GetViewport().GetVisibleRect().Size;
    }
    private void ShowMenu(bool resume){
        menu.Visible=true;Input.MouseMode=Input.MouseModeEnum.Visible;
        foreach(Node n in menuItems.GetChildren()){menuItems.RemoveChild(n);n.QueueFree();}
        menuItems.AddChild(Text(resume?"PAUSED":"ONE SMALL HOP.",23,Cream));menuItems.AddChild(Text(resume?"The crossing can wait.":"Five homes. A road. A river. You.",15,Lime));
        if(resume)Button("RESUME",Resume);Button(resume?"NEW GAME":"START CROSSING",()=>StartGame(1));if(!resume)Button("TWO PLAYERS · TAKE TURNS",()=>StartGame(2));
        var collision=new CheckButton{Text="Classic collision (original ROM)",ButtonPressed=!modern};collision.Toggled+=value=>{modern=!value;simulation?.Modern(modern);SavePreferences();};menuItems.AddChild(collision);
        var perspective=new CheckButton{Text="Perspective view",ButtonPressed=perspectiveView};perspective.Toggled+=value=>{perspectiveView=value;SavePreferences();};menuItems.AddChild(perspective);
        var follow=new CheckButton{Text="Follow frog (closer view)",ButtonPressed=followCamera};follow.Toggled+=value=>{followCamera=value;SavePreferences();};menuItems.AddChild(follow);
        var sound=new CheckButton{Text="Sound",ButtonPressed=!muted};sound.Toggled+=value=>{muted=!value;SavePreferences();};menuItems.AddChild(sound);
        menuItems.AddChild(Text("Arrow keys / WASD / D-pad to hop\nReach all five homes. Avoid cars and open water.",13,new Color("adb8cc")));
    }
    private void Button(string text,Action action){var b=new Button{Text=text,CustomMinimumSize=new Vector2(350,43)};b.AddThemeStyleboxOverride("normal",Style(new Color("526e3e"),7));b.AddThemeStyleboxOverride("hover",Style(new Color("6f914d"),7));b.Pressed+=action;menuItems.AddChild(b);if(menuItems.GetChildCount()==3)b.GrabFocus();}
    private void UpdateHud(){
        int player=state.At(0x83fd)==2?2:1;playerHeader.Text=$"{player}-UP";score.Text=state.BcdScore(player==1?0x83ed:0x83eb).ToString("D5");
        int best=Math.Max(state.BcdScore(0x83ef),Math.Max(state.BcdScore(0x83ed),state.BcdScore(0x83eb)));if(best>highScore){highScore=best;SavePreferences();}high.Text=highScore.ToString("D5");
        level.Text=$"PLAYER {player}     •     LEVEL {Math.Max(1,state.At(0x83b7)):00}";
        // These are spare frogs, despite the upstream TIME_REMAINING names.
        int count=state.At(player==1?0x83e5:0x83e6);lives.Text=$"FROGS   {new string('●',Math.Clamp(count,0,12))}";
        timer.Value=Math.Clamp(state.At(0x83dd)/60.0,0,1)*100;
        if(started&&!paused){if(state.At(0x83fe)==0)message.Text="GAME OVER\nPress Enter for another crossing";else if(state.At(0x8297)>0&&state.At(0x842f)>=5)message.Text="ALL FROGS HOME!";else message.Text="";}
        messagePanel.Visible=message.Text.Length>0;
    }
}
