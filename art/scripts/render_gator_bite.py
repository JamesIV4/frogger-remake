"""Inspect both the closed and open jaw from Blender's actual deform rig."""
from pathlib import Path
import os
import bpy
from mathutils import Vector
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'docs/evidence/models';OUT.mkdir(parents=True,exist_ok=True)
asset=os.environ.get('FROGGER_GATOR_REVIEW_ASSET','gator')
assert asset in ('gator','river_gator')
bpy.ops.wm.open_mainfile(filepath=str(ROOT/'art/source'/f'{asset}.blend'))
rig=next(o for o in bpy.context.scene.objects if o.type=='ARMATURE')
for track in rig.animation_data.nla_tracks:track.mute=True
rig.animation_data.action=next(track for track in rig.animation_data.nla_tracks if track.name=='Bite').strips[0].action
bpy.context.scene.render.engine='BLENDER_EEVEE'
bpy.context.scene.render.resolution_x=480;bpy.context.scene.render.resolution_y=360
bpy.context.scene.render.resolution_percentage=100
bpy.context.scene.render.image_settings.file_format='PNG'
bpy.context.scene.view_settings.view_transform='Standard'
bpy.ops.mesh.primitive_plane_add(size=200,location=(0,0,-.06))
mat=bpy.data.materials.new('Review ground');mat.diffuse_color=(.045,.055,.075,1)
bpy.context.object.data.materials.append(mat)
for position,power in [((3,-3,5),600),((-3,1,3),300)]:
    bpy.ops.object.light_add(type='AREA',location=position)
    bpy.context.object.data.energy=power;bpy.context.object.data.size=4
bpy.ops.object.camera_add();camera=bpy.context.object;bpy.context.scene.camera=camera
camera.data.type='ORTHO';camera.data.ortho_scale=2.7
tip_heights={}
tail_tips={}
for frame in [0,18]:
    bpy.context.scene.frame_set(frame)
    print('GATOR_BITE_POSE',frame,
          'Head',tuple(round(v,4) for v in rig.pose.bones['Head'].rotation_euler),
          'Jaw',tuple(round(v,4) for v in rig.pose.bones['Jaw'].rotation_euler))
    depsgraph=bpy.context.evaluated_depsgraph_get()
    for label in ['Tapered upper snout','Lower resting jaw']:
        obj=next(o for o in bpy.context.scene.objects if o.type=='MESH' and o.name.startswith(label))
        evaluated=obj.evaluated_get(depsgraph)
        mesh=evaluated.to_mesh()
        tip=min(mesh.vertices,key=lambda v:v.co.y)
        tip_position=evaluated.matrix_world @ tip.co
        tip_heights[frame,label]=tip_position.z
        print('GATOR_BITE_TIP',frame,label,tuple(round(v,4) for v in tip_position))
        evaluated.to_mesh_clear()
    if asset=='river_gator':
        tail=next(o for o in bpy.context.scene.objects if o.type=='MESH' and o.name.startswith('Long tapering tail'))
        evaluated=tail.evaluated_get(depsgraph)
        mesh=evaluated.to_mesh()
        tail_tips[frame]=max((evaluated.matrix_world @ v.co for v in mesh.vertices),key=lambda point:point.y)
        print('GATOR_TAIL_TIP',frame,tuple(round(v,4) for v in tail_tips[frame]))
        evaluated.to_mesh_clear()
    for view,pos in [('hero',(3,-4,2.5)),('side',(4,-1,1.3))]:
        camera.location=pos;camera.rotation_euler=(Vector((0,-.2,.2))-camera.location).to_track_quat('-Z','Y').to_euler()
        bpy.context.scene.render.filepath=str(OUT/f'{asset}-{view}-bite-{frame:02d}.png')
        bpy.ops.render.render(write_still=True)
assert tip_heights[18,'Tapered upper snout']-tip_heights[0,'Tapered upper snout']>.2,tip_heights
assert abs(tip_heights[18,'Lower resting jaw']-tip_heights[0,'Lower resting jaw'])<.001,tip_heights
assert tip_heights[18,'Lower resting jaw']>0,tip_heights
if asset=='river_gator':assert (tail_tips[18]-tail_tips[0]).length<.01,tail_tips
print('GATOR_JAW_REVIEW_COMPLETE')
