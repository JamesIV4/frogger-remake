"""Editable low-poly models, deform rigs and baked clips. Run with Blender --background --python.

Coordinate contract follows the PixelHack workflow: Z up, -Y forward; GLB Y up,
+Z forward. One game tile = one Blender unit. No external asset dependencies.
"""
from pathlib import Path
import math
import json
import random
import bpy
from mathutils import Vector

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'godot/Models'
SOURCE = ROOT / 'art/source'
REVIEW = ROOT / 'docs/evidence/models'
for path in (OUT, SOURCE, REVIEW): path.mkdir(parents=True, exist_ok=True)
PALETTE = {
    'green':'70b835', 'lime':'b1d645', 'darkgreen':'347d3f', 'cream':'f1dca0',
    'ink':'18252b','white':'fff1d0','eye':'eac342','rubber':'26353b','hub':'b8c4bc',
    'red':'d95546','yellow':'ebbd35','pink':'ac59bb','blue':'479db8','glass':'233f57',
    'orange':'dc7841','wood':'a86835','woodlight':'ce9552','wooddark':'714329',
    'shell':'518c3b','shelllight':'86b349','water':'349bab','road':'35434a',
    'grass':'548843','grasslight':'6e9c4b','grassdark':'467b42','sand':'bca674',
}
MATS={}
RECORDS=[]

def material(key):
    if key in MATS: return MATS[key]
    h=PALETTE[key]
    rgb=[int(h[i:i+2],16)/255 for i in (0,2,4)]
    linear=[v/12.92 if v<.04045 else ((v+.055)/1.055)**2.4 for v in rgb]
    mat=bpy.data.materials.new(key)
    mat.diffuse_color=(*linear,1)
    mat.use_nodes=True
    bsdf=mat.node_tree.nodes.get('Principled BSDF')
    bsdf.inputs['Base Color'].default_value=(*linear,1)
    bsdf.inputs['Roughness'].default_value=.72 if key!='glass' else .23
    MATS[key]=mat
    return mat

def clean():
    bpy.ops.object.select_all(action='SELECT'); bpy.ops.object.delete(use_global=False)

def finish(obj,name,color,bone='Body'):
    obj.name=name
    obj.data.materials.append(material(color))
    obj['part']=name; obj['rig_bone']=bone
    bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
    return obj

def ball(name,pos,scale,color,bone='Body',segments=12,rings=8):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=segments,ring_count=rings,location=pos)
    obj=bpy.context.object; obj.scale=scale
    return finish(obj,name,color,bone)

def box(name,pos,size,color,bone='Body',bevel=0):
    bpy.ops.mesh.primitive_cube_add(size=1,location=pos)
    obj=bpy.context.object; obj.scale=size
    finish(obj,name,color,bone)
    if bevel:
        mod=obj.modifiers.new('Small silhouette bevel','BEVEL');mod.width=bevel;mod.segments=1
        bpy.context.view_layer.objects.active=obj
        bpy.ops.object.modifier_apply(modifier=mod.name)
    return obj

def cylinder(name,a,b,r,color,bone='Body',vertices=10,r2=None):
    a,b=Vector(a),Vector(b);delta=b-a
    bpy.ops.mesh.primitive_cone_add(vertices=vertices,radius1=r,radius2=r if r2 is None else r2,depth=delta.length,location=(a+b)/2)
    obj=bpy.context.object;obj.rotation_euler=delta.to_track_quat('Z','Y').to_euler()
    return finish(obj,name,color,bone)

def mesh(name,verts,faces,color):
    data=bpy.data.meshes.new(name);data.from_pydata(verts,[],faces);data.update()
    obj=bpy.data.objects.new(name,data);bpy.context.collection.objects.link(obj)
    return finish(obj,name,color)

def eyes(y,z,x=.19,scale=.11,bone='Head'):
    for sign in [-1,1]:
        ball('Eye mound', (sign*x,y+.014,z-.03), (scale*1.26,scale*.86,scale*1.2),'green',bone)
        ball('Golden eye',(sign*x,y-.056,z+.016),(scale*.79,scale*.56,scale*.86),'eye',bone)
        ball('Pupil',(sign*x,y-.102,z+.02),(scale*.38,scale*.2,scale*.59),'ink',bone)
        ball('Eye glint',(sign*x-.02,y-.12,z+.053),(scale*.16,scale*.1,scale*.16),'white',bone,8,4)

def frog():
    ball('Body',(0,.035,.23),(.27,.33,.22),'green')
    ball('Cream throat',(0,-.18,.2),(.24,.18,.13),'cream','Head')
    ball('Broad face',(0,-.215,.29),(.285,.21,.18),'green','Head')
    eyes(-.255,.45)
    cylinder('Smile',(-.17,-.378,.25),(.17,-.378,.25),.012,'darkgreen','Head')
    specs={'Body':((0,0,.12),(0,0,.35),None),'Head':((0,-.1,.25),(0,-.28,.38),'Body')}
    for sign,side in [(-1,'L'),(1,'R')]:
        hip=(sign*.19,.16,.21);knee=(sign*.39,.21,.15);heel=(sign*.33,.4,.085)
        front=(sign*.22,-.14,.2);wrist=(sign*.32,-.27,.075)
        specs[f'Thigh.{side}']=(hip,knee,'Body');specs[f'Shin.{side}']=(knee,heel,f'Thigh.{side}')
        specs[f'Arm.{side}']=(front,wrist,'Body')
        ball('Powerful hind thigh',knee,(.14,.20,.135),'green',f'Thigh.{side}')
        cylinder('Hind upper leg',hip,knee,.102,'green',f'Thigh.{side}')
        cylinder('Folded shin',knee,heel,.075,'lime',f'Shin.{side}')
        cylinder('Foreleg',front,wrist,.05,'green',f'Arm.{side}')
        for n in range(3):
            toe=(heel[0]+sign*(n-1)*.054,heel[1]-.16, .05)
            cylinder('Hind toe',heel,toe,.03,'lime',f'Shin.{side}',6)
            tip=(wrist[0]+(n-1)*.065,wrist[1]-.115,.045)
            cylinder('Front toe',wrist,tip,.025,'lime',f'Arm.{side}',6)
        for y in [.0,.13,.25]:
            ball('Back spot',(sign*.12,y,.424 if y<.2 else .37),(.035,.05,.009),'darkgreen')
    return specs

def turtle():
    ball('Shell',(0,.03,.21),(.36,.4,.22),'shell')
    ball('Shell cap',(0,.025,.28),(.28,.31,.16),'shelllight')
    # Broad polygon scutes read at gameplay size without dense detailing.
    for x,y in [(-.15,0),(.15,0),(0,.16),(0,-.13)]:
        ball('Shell scute',(x,y,.4),(.10,.12,.027),'shell',segments=6,rings=4)
    ball('Head',(0,-.43,.15),(.14,.18,.125),'green','Head')
    for x in [-.085,.085]:
        ball('Eye',(x,-.5,.22),(.034,.042,.035),'ink','Head',8,4)
    specs={'Body':((0,0,.08),(0,0,.3),None),'Head':((0,-.25,.15),(0,-.48,.16),'Body')}
    for side,s in [('L',-1),('R',1)]:
        for end,y in [('Front',-.22),('Back',.28)]:
            bone=end+side;specs[bone]=((s*.2,y,.13),(s*.47,y-.08,.07),'Body')
            ball('Paddle',(s*.37,y,.09),(.19,.10,.045),'lime',bone)
    return specs

def gator():
    ball('Armored body',(0,.15,.17),(.3,.54,.19),'shell')
    cylinder('Tail',(0,.54,.17),(0,1.1,.055),.19,'shell',r2=.025)
    box('Upper muzzle',(0,-.53,.19),(.43,.72,.17),'green','Head',.06)
    box('Jaw',(0,-.55,.08),(.42,.7,.085),'cream','Jaw',.025)
    for s in [-1,1]:
        for y in [-.29,-.46,-.63,-.78]:
            cylinder('Tooth',(s*.18,y,.15),(s*.18,y,.06),.035,'white','Head',5,r2=0)
        ball('Eye mound',(s*.19,-.23,.35),(.10,.13,.09),'green','Head')
        ball('Eye',(s*.20,-.29,.38),(.045,.036,.04),'ink','Head',8,4)
        for y in [.0,.37]: ball('Foot',(s*.36,y,.08),(.19,.11,.055),'green')
    for y,z in [(.03,.32),(.21,.34),(.39,.30),(.57,.24),(.74,.17)]:
        for x in [-.12,.12]: cylinder('Back spike',(x,y,z),(x,y,z+.13),.066,'darkgreen',vertices=4,r2=0)
    return {'Body':((0,0,.1),(0,0,.3),None),'Head':((0,-.1,.2),(0,-.5,.22),'Body'),
            'Jaw':((0,-.2,.09),(0,-.64,.09),'Head')}

def fly():
    ball('Body',(0,0,.28),(.15,.24,.13),'ink')
    ball('Head',(0,-.24,.3),(.15,.13,.12),'green','Head')
    for s in [-1,1]:
        ball('Huge eye',(s*.1,-.30,.34),(.09,.08,.095),'red','Head')
        ball('Wing',(s*.25,.06,.36),(.25,.15,.025),'white',f'Wing{s}')
    return {'Body':((0,0,.15),(0,0,.3),None),'Head':((0,-.14,.27),(0,-.3,.3),'Body'),
            'Wing-1':((-.09,0,.3),(-.45,0,.3),'Body'),'Wing1':((.09,0,.3),(.45,0,.3),'Body')}

def snake():
    specs={'Body':((0,0,.06),(0,0,.2),None)}
    for i in range(8):
        y=i*.19-.6;x=math.sin(i*.9)*.14
        bone='Body' if i==0 else f'Segment{i}'
        if i:specs[bone]=((x,y,.13),(x,y+.19,.13),'Body' if i==1 else f'Segment{i-1}')
        ball('Coil',(x,y,.13),(.12 if i<6 else .09,.17,.1),'yellow' if i%2 else 'wooddark',bone)
    ball('Head',(0,-.72,.17),(.17,.19,.13),'green')
    for s in [-1,1]:ball('Eye',(s*.09,-.82,.23),(.037,.04,.04),'ink',segments=8,rings=4)
    return specs

def otter():
    ball('Body',(0,.02,.17),(.21,.46,.17),'wooddark')
    ball('Head',(0,-.43,.2),(.22,.22,.17),'wooddark','Head')
    ball('Muzzle',(0,-.58,.16),(.15,.12,.09),'cream','Head')
    ball('Nose',(0,-.68,.18),(.055,.035,.035),'ink','Head')
    cylinder('Tail',(0,.38,.13),(0,.91,.035),.11,'wooddark','Tail',8,r2=.024)
    for s in [-1,1]:
        ball('Ear',(s*.16,-.30,.32),(.065,.065,.075),'wooddark','Head')
        ball('Eye',(s*.13,-.57,.28),(.037,.035,.038),'ink','Head',8,4)
        for y in [-.17,.31]:ball('Paddle foot',(s*.22,y,.06),(.12,.16,.05),'wooddark')
    return {'Body':((0,0,.07),(0,0,.3),None),'Head':((0,-.25,.2),(0,-.5,.21),'Body'),'Tail':((0,.36,.12),(0,.8,.06),'Body')}

def vehicle(kind):
    specs={'Body':((0,0,.1),(0,0,.5),None)}
    long=1.85 if kind=='truck' else .94
    color={'car':'pink','racecar':'yellow','truck':'red','dozer':'cream','sport':'red'}[kind]
    box('Chassis',(0,0,.18),(.66,long,.20),'rubber',bevel=.035)
    if kind=='truck':
        box('Cab',(0,-.62,.41),(.72,.48,.54),color,bevel=.06)
        box('Windshield',(0,-.877,.52),(.58,.025,.2),'glass')
        box('Cargo box',(0,.34,.52),(.76,1.3,.67),'white',bevel=.035)
        box('Cargo stripe',(0,.34,.866),(.62,1.16,.012),'blue')
    elif kind=='racecar':
        box('Nose',(0,-.36,.22),(.28,.55,.16),color,bevel=.04)
        box('Body',(0,.1,.28),(.46,.58,.25),color,bevel=.055)
        box('Cockpit',(0,.07,.43),(.26,.25,.06),'glass',bevel=.03)
        box('Front wing',(0,-.59,.17),(.8,.14,.08),color)
        box('Spoiler',(0,.40,.4),(.78,.16,.08),color)
        box('White racing stripe',(0,-.37,.312),(.10,.49,.01),'white')
    elif kind=='dozer':
        box('Engine',(0,-.21,.36),(.6,.45,.4),'yellow',bevel=.05)
        box('Cab',(0,.17,.48),(.44,.35,.45),'cream',bevel=.03)
        box('Windscreen',(0,-.012,.52),(.31,.02,.24),'glass')
        box('Blade',(0,-.58,.22),(.93,.13,.35),'hub',bevel=.03)
        cylinder('Exhaust',(.19,-.17,.55),(.19,-.17,.8),.026,'ink')
    else:
        box('Body',(0,0,.3),(.7,1.16,.32),color,bevel=.09)
        box('Cabin',(0,.04,.54),(.57,.54,.29),color,bevel=.065)
        box('Front windshield',(0,-.235,.56),(.48,.025,.19),'glass')
        box('Back window',(0,.32,.54),(.46,.021,.17),'glass')
        for s in [-1,1]:box('Side window',(s*.29,.04,.57),(.02,.41,.16),'glass')
        box('Bumper',(0,-.59,.22),(.62,.055,.09),'hub',bevel=.02)
    for s in [-1,1]:
        for y in [-long*.34,long*.34]:
            bone=f'Wheel{s}_{y:.2f}';specs[bone]=((s*.31,y,.17),(s*.44,y,.17),'Body')
            cylinder('Tire',(s*.29,y,.17),(s*.44,y,.17),.18,'rubber',bone,12)
            cylinder('Hub',(s*.443,y,.17),(s*.452,y,.17),.086,'hub',bone,8)
        box('Headlight',(s*.23,-long/2-.033,.32),(.14,.032,.10),'white')
        box('Tail lamp',(s*.23,long/2+.02,.3),(.12,.027,.075),'red')
    return specs

def log():
    cylinder('Bark',(-.5,0,.16),(.5,0,.16),.25,'wood',vertices=9)
    for sign in [-1,1]:
        cylinder('End grain',(sign*.501,0,.16),(sign*.506,0,.16),.215,'woodlight',vertices=9)
        cylinder('Heartwood',(sign*.508,0,.16),(sign*.511,0,.16),.125,'wood',vertices=9)
    for y,z in [(-.11,.37),(.07,.4),(.20,.28)]:
        cylinder('Bark ridge',(-.44,y,z),(.45,y,z),.018,'wooddark',vertices=5)
    return None

def board():
    box('Walnut base',(0,0,-.54),(14.85,13.35,.8),'wooddark',bevel=.14)
    box('Inlay rim',(0,0,-.14),(14.63,13.18,.15),'woodlight',bevel=.06)
    box('Road',(0,-3,-.036),(14,5,.10),'road')
    box('River bed',(0,3,-.17),(14,5,.22),'water')
    for y in [-6,0]:
        box('Flat grassy bank',(0,y,.0),(14,1,.15),'grass',bevel=.04)
        rng=random.Random(int(y)+24)
        for i in range(130):
            x=rng.uniform(-6.9,6.9);yy=y+rng.uniform(-.43,.43);w=rng.uniform(.06,.20)
            mesh('Painted grass patch',[(x-w,yy,.078),(x+w,yy+.055,.078),(x+.04,yy+.11,.078)],[(0,1,2)],'grasslight' if i%2 else 'grassdark')
    for y in [-.54,-5.46]:box('Curb',(0,y,.06),(14,.065,.1),'sand')
    for y in [-1.5,-2.5,-3.5,-4.5]:
        for n in range(19):box('Lane dash',(-6.7+n*.74,y,.021),(.39,.042,.011),'white')
    # Five actual entrances, no rail or vegetation spanning their mouths.
    for i in range(5):
        x=-6+i*3
        box('Home bay',(x,6,-.01),(1.23,1.0,.14),'sand',bevel=.05)
        box('Home back lip',(x,6.48,.08),(1.25,.10,.12),'woodlight',bevel=.03)
    for x in [-6.9,-4.5,-1.5,1.5,4.5,6.9]:
        w=.7 if abs(x)>6 else 1.68
        box('Low home hedge',(x,6,.14),(w,1,.37),'grassdark',bevel=.1)
        for dx in [-.25,0,.25]:ball('Hedge facets',(x+dx,6.12,.38),(.27,.25,.15),'grass',segments=6,rings=4)
    for x in [-7.25,7.25]:box('Side rail',(x,0,.04),(.28,13.15,.32),'wood',bevel=.05)
    box('Top rail',(0,6.65,.0),(14.8,.23,.27),'wood',bevel=.04)
    # Bottom lip is below the walkable start bank; it never cuts row 0 off.
    box('Bottom lip',(0,-6.65,-.13),(14.8,.22,.18),'wood',bevel=.04)
    return None

def rig_and_clips(specs,kind):
    meshes=[o for o in bpy.context.scene.objects if o.type=='MESH']
    bpy.ops.object.select_all(action='DESELECT')
    data=bpy.data.armatures.new(kind+'Rig');rig=bpy.data.objects.new(kind+'Rig',data);bpy.context.collection.objects.link(rig)
    rig.select_set(True);bpy.context.view_layer.objects.active=rig
    bpy.ops.object.mode_set(mode='EDIT')
    for name,(head,tail,parent) in specs.items():
        bone=data.edit_bones.new(name);bone.head=head;bone.tail=tail
        if parent:bone.parent=data.edit_bones[parent]
    bpy.ops.object.mode_set(mode='OBJECT')
    for obj in meshes:
        group=obj.vertex_groups.new(name=obj['rig_bone']);group.add(list(range(len(obj.data.vertices))),1,'REPLACE')
        mod=obj.modifiers.new('Deform rig','ARMATURE');mod.object=rig;obj.parent=rig
    clips=['Idle','Hop','Death','Celebrate'] if kind=='frog' else ['Idle','Swim','Dive'] if kind=='turtle' else ['Idle','Bite'] if kind=='gator' else ['Idle','Move']
    bpy.context.scene.render.fps=60
    rig.animation_data_create()
    for clip in clips:
        duration=10 if clip=='Hop' else 42 if clip=='Death' else 60
        action=bpy.data.actions.new(clip);action.use_fake_user=True;rig.animation_data.action=action
        for frame in range(duration+1):
            p=frame/duration;cycle=math.sin(p*math.tau);h=math.sin(p*math.pi)
            for b in rig.pose.bones:
                b.rotation_mode='XYZ';b.location=(0,0,0);b.rotation_euler=(0,0,0);b.scale=(1,1,1)
                if b.name=='Body':
                    b.scale=(1,1,1+.014*cycle) if clip=='Idle' else (1,1,1)
                    if clip=='Hop':b.location.y=.24*h;b.rotation_euler.x=-.16*h
                    if clip=='Death':b.scale=(1+.35*h,1-.65*p,1+.2*h);b.rotation_euler.y=.35*p
                    if clip=='Celebrate':b.location.y=.09*(1-math.cos(p*math.tau*2))
                if 'Thigh' in b.name:b.rotation_euler.x=(-.9*h if clip=='Hop' else .018*cycle)
                if 'Shin' in b.name:b.rotation_euler.x=(1.5*h if clip=='Hop' else .015*cycle)
                if 'Arm.' in b.name:b.rotation_euler.x=(-.45*h if clip=='Hop' else .08*cycle if clip=='Celebrate' else .015*cycle)
                if 'Wing' in b.name:b.rotation_euler.x=.7*math.sin(p*math.tau*4)
                if 'Front' in b.name or 'Back' in b.name:b.rotation_euler.z=.22*cycle
                if 'Segment' in b.name:b.rotation_euler.x=.13*math.sin(p*math.tau+int(b.name[-1]))
                if b.name=='Jaw' and clip=='Bite':b.rotation_euler.x=.65*h
                if b.name=='Tail':b.rotation_euler.z=.25*cycle
                if 'Wheel' in b.name and clip=='Move':b.rotation_euler.y=p*math.tau*2
                for channel in ['location','rotation_euler','scale']:b.keyframe_insert(channel,frame=frame,group=b.name)
        track=rig.animation_data.nla_tracks.new();track.name=clip
        strip=track.strips.new(clip,0,action);strip.action_frame_start=0;strip.action_frame_end=duration
        track.mute=True
    rig.animation_data.action=None
    for track in rig.animation_data.nla_tracks:track.mute=False
    bpy.context.scene.frame_set(0)
    return clips

def export(kind,specs):
    clips=rig_and_clips(specs,kind) if specs else []
    bpy.context.scene.frame_set(0)
    bpy.ops.wm.save_as_mainfile(filepath=str(SOURCE / (kind+'.blend')))
    bpy.ops.export_scene.gltf(filepath=str(OUT/(kind+'.glb')),export_format='GLB',export_animations=True,
        export_animation_mode='NLA_TRACKS',export_force_sampling=True,export_yup=True,export_apply=False)
    meshes=[o for o in bpy.context.scene.objects if o.type=='MESH']
    triangles=sum(sum(len(p.vertices)-2 for p in o.data.polygons) for o in meshes)
    RECORDS.append({'asset':kind,'triangles':triangles,'bones':len(specs) if specs else 0,'clips':clips,
        'source':f'art/source/{kind}.blend','glb':f'godot/Models/{kind}.glb'})

for kind,builder in [('frog',frog),('turtle',turtle),('gator',gator),('fly',fly),('snake',snake),('otter',otter),
                     ('log',log),('car',lambda:vehicle('car')),('truck',lambda:vehicle('truck')),
                     ('racecar',lambda:vehicle('racecar')),('dozer',lambda:vehicle('dozer')),
                     ('sport',lambda:vehicle('sport')),('board',board)]:
    clean();specs=builder();export(kind,specs)
(ROOT/'art/models.json').write_text(json.dumps(RECORDS,indent=2)+'\n')
print('FROGGER_ASSETS_COMPLETE',len(RECORDS))
