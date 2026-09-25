"""Four actual Blender renders of each source asset, with a fixed spatial reference."""
from pathlib import Path
import bpy,math,os
from mathutils import Vector
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'docs/evidence/models';OUT.mkdir(parents=True,exist_ok=True)
selected=os.environ.get('FROGGER_REVIEW_ASSETS')
names=selected.split(',') if selected else ['frog','lady_frog','turtle','gator','river_gator','fly','snake','otter','car','truck','racecar','dozer','sport','log']
for name in names:
    bpy.ops.wm.open_mainfile(filepath=str(ROOT/'art/source'/f'{name}.blend'))
    scene=bpy.context.scene
    for obj in scene.objects:
        if obj.animation_data:
            for track in obj.animation_data.nla_tracks:track.mute=True
    scene.frame_set(0)
    scene.render.engine='BLENDER_EEVEE';scene.render.resolution_x=320;scene.render.resolution_y=320;scene.render.resolution_percentage=100
    scene.render.image_settings.file_format='PNG';scene.world.color=(.22,.22,.22)
    scene.view_settings.view_transform='Standard'
    bpy.ops.mesh.primitive_plane_add(size=200,location=(0,0,-.06))
    mat=bpy.data.materials.new('Neutral reference ground');mat.diffuse_color=(.065,.075,.095,1);bpy.context.object.data.materials.append(mat)
    for pos,power,size in [((2,-3,5),400,4),((-3,1,3),180,3)]:
        bpy.ops.object.light_add(type='AREA',location=pos);bpy.context.object.data.energy=power;bpy.context.object.data.shape='DISK';bpy.context.object.data.size=size
    bpy.ops.object.camera_add();camera=bpy.context.object;scene.camera=camera;camera.data.type='ORTHO';camera.data.ortho_scale=3.2 if name=='gator' else 2.4 if name in ['river_gator','truck','snake'] else 1.8
    for view,pos in [('hero',(3,-4,3)),('front',(0,-5,1.8)),('side',(5,0,1.8)),('top',(0,-.001,6))]:
        camera.location=pos;camera.rotation_euler=(Vector((0,0,.22))-camera.location).to_track_quat('-Z','Y').to_euler()
        scene.render.filepath=str(OUT/f'{name}-{view}.png');bpy.ops.render.render(write_still=True)
print('FOUR_VIEW_REVIEW_COMPLETE')
