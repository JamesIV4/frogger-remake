using Godot;
using System;
using System.Collections.Generic;
namespace FroggerRemake;
public sealed class ModelActor
{
    public readonly Node3D Root;
    public readonly AnimationPlayer? Animator;
    public readonly Node3D? PassengerSocket;
    public readonly Skeleton3D? Skeleton;
    private string current="";
    private static readonly Dictionary<ulong,ShaderMaterial> Clipped=new();
    private static readonly Shader ClipShader=new(){Code="shader_type spatial; uniform vec4 tint : source_color = vec4(1.); varying vec3 world; void vertex(){world=(MODEL_MATRIX*vec4(VERTEX,1.0)).xyz;} void fragment(){if(abs(world.x)>7.0)discard;ALBEDO=tint.rgb;ROUGHNESS=.75;}"};
    public ModelActor(Node3D parent,string asset) {
        Root=GD.Load<PackedScene>($"res://Models/{asset}.glb").Instantiate<Node3D>();
        parent.AddChild(Root);ClipAtBoardEdge(Root);Animator=Find<AnimationPlayer>(Root);
        PassengerSocket=Root.FindChild("PassengerSocket",true,false) as Node3D;Skeleton=Find<Skeleton3D>(Root);Play("Idle");
    }
    private static void ClipAtBoardEdge(Node node){
        if(node is MeshInstance3D mesh&&mesh.Mesh!=null)for(int i=0;i<mesh.Mesh.GetSurfaceCount();i++){
            var original=mesh.Mesh.SurfaceGetMaterial(i);if(original is not StandardMaterial3D standard)continue;
            ulong id=original.GetInstanceId();if(!Clipped.TryGetValue(id,out var shader)){shader=new ShaderMaterial{Shader=ClipShader};shader.SetShaderParameter("tint",standard.AlbedoColor);Clipped[id]=shader;}
            mesh.SetSurfaceOverrideMaterial(i,shader);
        }
        foreach(Node child in node.GetChildren())ClipAtBoardEdge(child);
    }
    public void Play(string clip,bool loop=true,float speed=1) {
        if(Animator==null)return;
        foreach(var name in Animator.GetAnimationList())if(name.ToString().EndsWith(clip,StringComparison.OrdinalIgnoreCase)) {
            if(current==name&&Animator.IsPlaying())return;
            Animator.GetAnimation(name).LoopMode=loop?Animation.LoopModeEnum.Linear:Animation.LoopModeEnum.None;
            Animator.SpeedScale=speed;Animator.Play(name,.035);current=name;return;
        }
    }
    public void Pose(string clip,double seconds) {
        if(Animator==null)return;
        foreach(var name in Animator.GetAnimationList())if(name.ToString().EndsWith(clip,StringComparison.OrdinalIgnoreCase)) {
            var animation=Animator.GetAnimation(name);
            if(current!=name){Animator.Play(name,0);current=name;}
            Animator.Seek(Math.Clamp(seconds,0,animation.Length),true,true);
            Animator.Pause();return;
        }
        throw new InvalidOperationException($"Missing required clip {clip} on {Root.Name}");
    }
    private static T? Find<T>(Node root) where T:Node {
        if(root is T item)return item;
        foreach(Node child in root.GetChildren()){var found=Find<T>(child);if(found!=null)return found;}return null;
    }
}
