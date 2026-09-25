"""Inspect both the closed and open jaw from Blender's actual deform rig."""
from pathlib import Path
import bpy
from mathutils import Vector
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'docs/evidence/models';OUT.mkdir(parents=True,exist_ok=True)
bpy.ops.wm.open_mainfile(filepath=str(ROOT/'art/source/gator.blend'))
rig=next(o for o in bpy.context.scene.objects if o.type=='ARMATURE')
for track in rig.animation_data.nla_tracks:track.mute=True
rig.animation_data.action=next(a for a in bpy.data.actions if a.name=='Bite')
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
for frame in [0,18]:
    bpy.context.scene.frame_set(frame)
    for view,pos in [('hero',(3,-4,2.5)),('side',(4,-1,1.3))]:
        camera.location=pos;camera.rotation_euler=(Vector((0,-.2,.2))-camera.location).to_track_quat('-Z','Y').to_euler()
        bpy.context.scene.render.filepath=str(OUT/f'gator-{view}-bite-{frame:02d}.png')
        bpy.ops.render.render(write_still=True)
print('GATOR_JAW_REVIEW_COMPLETE')
