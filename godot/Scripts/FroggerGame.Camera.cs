using Godot;
using System;
namespace FroggerRemake;

public partial class FroggerGame
{
    // The classic overhead board remains the fresh-install default. Each mode
    // can be toggled independently, including follow with the orthographic view.
    private bool perspectiveView;
    private bool followCamera;
    private Vector3 cameraLookTarget=new(0,0,.15f);

    private void UpdateCamera(double delta) {
        var view=GetViewport().GetVisibleRect().Size;
        float aspect=Math.Max(.55f,view.X/Math.Max(1,view.Y));
        camera.Projection=perspectiveView?Camera3D.ProjectionType.Perspective:Camera3D.ProjectionType.Orthogonal;
        camera.Size=followCamera&&started?9.8f:Math.Max(17.4f,16.2f/aspect);
        camera.Fov=followCamera&&started?54f:52f;

        bool tracking=followCamera&&started&&FrogVisualState.PlayerOnBoard(state);
        float tx=0,tz=0;
        if(tracking){
            float x=frogVisual.Dying?frogVisual.DeathX:BlendCoordinate(0x8044,RenderFraction);
            float row=frogVisual.Dying?frogVisual.DeathRow:BlendCoordinate(0x8047,RenderFraction);
            Vector3 player=Pos(x,row);
            tx=Math.Clamp(player.X,-2.5f,2.5f);
            tz=Math.Clamp(player.Z,-4f,4f);
        } else if(followCamera&&started){
            tx=Math.Clamp((camera.Position.X),-2.5f,2.5f);
            tz=Math.Clamp(camera.Position.Z-(perspectiveView?5.8f:6.5f),-4f,4f);
        }
        Vector3 desired=tracking||followCamera&&started
            ?new Vector3(tx,perspectiveView?9f:11.2f,tz+(perspectiveView?5.8f:6.5f))
            :perspectiveView?new Vector3(0,15.5f,9.5f):new Vector3(0,19,9.8f);
        float responsiveness=1f-(float)Math.Exp(-6*Math.Min(delta,.1));
        camera.Position=camera.Position.Lerp(desired,responsiveness);
        Vector3 desiredLook=tracking||followCamera&&started?new Vector3(tx,0,tz-.55f):new Vector3(0,0,.15f);
        cameraLookTarget=cameraLookTarget.Lerp(desiredLook,responsiveness);
        camera.LookAt(cameraLookTarget);
    }
}
