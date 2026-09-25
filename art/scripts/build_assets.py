"""Editable low-poly models, deform rigs and baked clips. Run with Blender --background --python.

Coordinate contract follows the PixelHack workflow: Z up, -Y forward; GLB Y up,
+Z forward. One game tile = one Blender unit. No external asset dependencies.
"""
from pathlib import Path
import math
import json
import random
import bpy
from mathutils import Matrix, Vector

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'godot/Models'
SOURCE = ROOT / 'art/source'
REVIEW = ROOT / 'docs/evidence/models'
for path in (OUT, SOURCE, REVIEW): path.mkdir(parents=True, exist_ok=True)
PALETTE = {
    'green':'47e51b', 'lime':'b0ff39', 'darkgreen':'08732e', 'cream':'fff3bd',
    'ink':'101b32','white':'f5f7ff','eye':'ffc72b','rubber':'101522','hub':'b9c8dd',
    'red':'ed1237','yellow':'ffdd00','pink':'de05e8','blue':'00aafa','glass':'08265d',
    'orange':'ff7a16','wood':'94501f','woodlight':'cc8739','wooddark':'462c23',
    'shell':'13652e','shelllight':'419e2c','water':'091c60','road':'171d28',
    'grass':'1c501c','grasslight':'2b6c20','grassdark':'123913','sand':'b8a46e',
    'hedge':'216a0a','hedgelight':'70b91c','hedgedark':'082f0d',
    'lady':'ff269c','ladylight':'ff9acb','ladydark':'9b075f',
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

def muzzle(name,sections,color,bone):
    """A closed, tapered octagonal head section, with broad cheeks and a flared tip."""
    verts=[]
    for y,width,low,high in sections:
        verts.extend([(-width*.78,y,high),(width*.78,y,high),
            (width,y,high-.035),(width,y,low+.035),
            (width*.78,y,low),(-width*.78,y,low),
            (-width,y,low+.035),(-width,y,high-.035)])
    faces=[]
    for ring in range(len(sections)-1):
        for side in range(8):
            next_side=(side+1)%8;a=ring*8+side;b=(ring+1)*8+side
            faces.append((a,b,(ring+1)*8+next_side,ring*8+next_side))
    faces.append(tuple(reversed(range(8))))
    faces.append(tuple((len(sections)-1)*8+i for i in range(8)))
    obj=mesh(name,verts,faces,color);obj['rig_bone']=bone
    return obj

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
        front=(sign*.19,-.13,.21);wrist=(sign*.44,-.27,.075)
        # The shoulder must begin well inside the body ellipsoid, otherwise
        # widening the forearms leaves a visible seam at the torso.
        assert (front[0]/.27)**2+((front[1]-.035)/.33)**2+((front[2]-.23)/.22)**2<.85
        specs[f'Thigh.{side}']=(hip,knee,'Body');specs[f'Shin.{side}']=(knee,heel,f'Thigh.{side}')
        specs[f'Arm.{side}']=(front,wrist,'Body')
        # Body-weighted shoulder overlaps both the torso and the rotating arm.
        # This remains joined through the hop clip, not just in the rest pose.
        ball('Connected front shoulder',(sign*.265,-.145,.18),(.19,.20,.16),'green','Body',segments=9,rings=6)
        cylinder('Continuous front-leg bridge',(sign*.14,-.08,.19),(sign*.38,-.22,.12),.13,'green','Body',vertices=10)
        ball('Powerful hind thigh',knee,(.14,.20,.135),'green',f'Thigh.{side}')
        cylinder('Hind upper leg',hip,knee,.102,'green',f'Thigh.{side}')
        cylinder('Folded shin',knee,heel,.075,'lime',f'Shin.{side}')
        cylinder('Foreleg',front,wrist,.085,'green',f'Arm.{side}')
        ball('Broad front forearm',(sign*.38,-.23,.12),(.15,.14,.10),'green',f'Arm.{side}',segments=9,rings=6)
        ball('Broad front hand',wrist,(.10,.09,.065),'green',f'Arm.{side}',segments=8,rings=5)
        for n in range(3):
            toe=(heel[0]+sign*(n-1)*.054,heel[1]-.16, .05)
            cylinder('Hind toe',heel,toe,.03,'lime',f'Shin.{side}',6)
            tip=(wrist[0]+(n-1)*.065,wrist[1]-.115,.045)
            cylinder('Front toe',wrist,tip,.025,'lime',f'Arm.{side}',6)
        for y in [.0,.13,.25]:
            ball('Back spot',(sign*.12,y,.424 if y<.2 else .37),(.035,.05,.009),'darkgreen')
    # The PS1 reference's orange dorsal marking keeps the player readable on grass.
    mesh('Orange back stripe',[(-.038,-.06,.441),(.038,-.06,.441),(-.036,.13,.442),(.036,.13,.442),(-.02,.28,.366),(.02,.28,.366)],[(0,1,3,2),(2,3,5,4)],'orange')
    return specs

def lady_frog():
    specs=frog()
    swaps={'green':'lady','lime':'ladylight','darkgreen':'ladydark','orange':'white'}
    for obj in bpy.context.scene.objects:
        if obj.type=='MESH':
            for slot in obj.material_slots:
                if slot.material.name in swaps:slot.material=material(swaps[slot.material.name])
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
    ball('Armored trunk',(0,.15,.17),(.32,.54,.19),'shell',segments=12,rings=7)
    cylinder('Long tapering tail',(0,.51,.17),(0,1.12,.05),.21,'shell',vertices=10,r2=.018)
    muzzle('Tapered upper snout',[
        (-.18,.25,.12,.36),(-.39,.205,.13,.33),(-.70,.155,.115,.27),
        (-.91,.192,.11,.27),(-1.03,.147,.12,.235)],'green','Head')
    muzzle('Lower articulated jaw',[
        (-.17,.215,.055,.13),(-.44,.174,.045,.13),
        (-.81,.175,.045,.135),(-.99,.137,.055,.12)],'cream','Jaw')
    ball('Dark mouth',(0,-.67,.127),(.165,.36,.018),'ink','Head',segments=10,rings=4)
    mesh('Top snout ridge',[(-.085,-.23,.367),(.085,-.23,.367),
        (-.055,-.69,.279),(.055,-.69,.279),(-.05,-.94,.274),(.05,-.94,.274)],
        [(0,1,3,2),(2,3,5,4)],'lime')['rig_bone']='Head'
    for s in [-1,1]:
        for y,width in [(-.39,.19),(-.56,.17),(-.73,.155),(-.87,.185)]:
            cylinder('Triangular tooth',(s*width,y,.14),(s*width,y,.055),.031,'white','Head',5,r2=0)
        ball('Raised eye ridge',(s*.18,-.23,.36),(.12,.13,.10),'darkgreen','Head',10,6)
        ball('Golden eye',(s*.19,-.305,.406),(.057,.049,.044),'eye','Head',10,6)
        ball('Narrow pupil',(s*.196,-.34,.414),(.026,.022,.033),'ink','Head',8,4)
        ball('Nostril',(s*.104,-.94,.279),(.035,.045,.019),'darkgreen','Head',8,4)
        for y in [-.11,.38]:
            ball('Webbed foot',(s*.34,y,.077),(.21,.13,.065),'green')
            for t in [-1,0,1]:
                cylinder('Claw',(s*(.40+t*.045),y-.08,.08),(s*(.44+t*.045),y-.18,.06),.025,'cream',vertices=5,r2=0)
    for y,z in [(.03,.32),(.21,.34),(.39,.30),(.57,.24),(.74,.17)]:
        for x in [-.12,.12]: cylinder('Back spike',(x,y,z),(x,y,z+.15),.067,'darkgreen',vertices=5,r2=0)
    return {'Body':((0,0,.1),(0,0,.3),None),'Head':((0,-.1,.2),(0,-.5,.22),'Body'),
            'Jaw':((0,-.2,.09),(0,-.72,.08),'Head')}

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
    color={'car':'pink','racecar':'red','truck':'red','dozer':'cream','sport':'blue'}[kind]
    box('Chassis',(0,0,.18),(.66,long,.20),'rubber',bevel=.035)
    if kind=='truck':
        box('Cab',(0,-.62,.41),(.72,.48,.54),color,bevel=.06)
        box('Windshield',(0,-.877,.52),(.58,.025,.2),'glass')
        box('Cargo box',(0,.34,.52),(.76,1.3,.67),'yellow',bevel=.035)
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
    # Native rows 0xe0 and 0xf0 are both playable. Leave a full grass tile
    # beyond the spawn row and keep the lip behind the frog's feet at 0xf0.
    # Split the wooden substrate around the river. A solid slab here would sit
    # in front of the lowered bed and turn translucent blue water purple.
    for name,y,length in [('Lower',-3.70,8.40),('Upper',6.55,2.10)]:
        box(name+' walnut base',(0,y,-.54),(14.85,length,.8),'wooddark',bevel=.10)
        box(name+' inlay',(0,y,-.14),(14.63,length-.12,.15),'woodlight',bevel=.04)
    for x in [-7.20,7.20]:box('River wall',(x,3,-.54),(.45,5.10,.8),'wooddark',bevel=.04)
    box('Road',(0,-3,-.036),(14,5,.10),'road')
    # The bed clears even the paddles at the turtles' maximum .70-unit dive.
    # A deep open cavity keeps the whole animal visible through blue water.
    box('River bed',(0,3,-1.31),(14,5,.22),'water')
    for y,width in [(-6.5,2),(0,1)]:
        box('Flat grassy bank',(0,y,.0),(14,width,.15),'grass',bevel=.04)
        rng=random.Random(int(y)+24)
        for i in range(130*width):
            x=rng.uniform(-6.9,6.9);yy=y+rng.uniform(-width*.43,width*.43);w=rng.uniform(.06,.20)
            mesh('Painted grass patch',[(x-w,yy,.078),(x+w,yy+.055,.078),(x+.04,yy+.11,.078)],[(0,1,2)],'grasslight' if i%2 else 'grassdark')
    for y in [-.54,-5.46]:box('Curb',(0,y,.06),(14,.065,.1),'sand')
    for y in [-1.5,-2.5,-3.5,-4.5]:
        for n in range(19):box('Lane dash',(-6.7+n*.74,y,.021),(.39,.042,.011),'white')
    # Five actual entrances, no rail or vegetation spanning their mouths.
    for i in range(5):
        x=-6+i*3
        box('Home bay',(x,6,-.01),(1.23,1.0,.14),'sand',bevel=.05)
        box('Home back lip',(x,6.48,.08),(1.25,.10,.12),'woodlight',bevel=.03)
    rng=random.Random(1981)
    for x in [-6.9,-4.5,-1.5,1.5,4.5,6.9]:
        w=.7 if abs(x)>6 else 1.68
        box('Dense impassable hedge',(x,6,.22),(w,1,.48),'hedgedark',bevel=.10)
        for dx in [-w*.22,w*.22]:
            ball('Dense angular canopy',(x+dx,6.03,.51),(w*.29,.47,.40),'hedge',segments=7,rings=4)
        # Folded spear-shaped leaves, contained inside the non-goal columns.
        # Large pointed silhouettes read as a barrier without tall bank grass.
        for i in range(32 if w>1 else 16):
            lx=x+rng.uniform(-w*.43,w*.43);ly=rng.uniform(5.62,6.38)
            tipx=max(x-w*.49,min(x+w*.49,lx+rng.uniform(-.24,.24)))
            tipy=ly+rng.uniform(-.10,.12);height=rng.uniform(.72,1.17)
            half=min(rng.uniform(.09,.16),w*.49-abs(lx-x))
            verts=[(lx-half,ly,.26),(lx+half,ly,.26),(tipx,tipy,height),
                   (lx,ly-.10,.50+(height-.72)*.35),(lx,ly+.035,.42)]
            mesh('Pointed hedge leaf',verts,[(0,3,2),(3,1,2),(0,2,4),(4,2,1),(0,4,1,3)],'hedgelight' if i%4==0 else 'hedge')
    # Rear silhouette sits beyond the five mouths. Dense, low-poly masses and
    # large spikes read as an impassable hedge without occupying a home slot.
    box('Back hedge trunk',(0,7.02,.23),(14.2,.72,.50),'hedgedark',bevel=.08)
    for n in range(19):
        x=-6.8+n*.75
        ball('Back hedge crown',(x,7.03,.55),(.48,.40,.39),'hedge',segments=7,rings=4)
        for k in range(5):
            px=x+rng.uniform(-.35,.35);py=7.02+rng.uniform(-.24,.24)
            mesh('Back hedge spear',[(px-.17,py,.47),(px+.17,py,.47),(px+rng.uniform(-.14,.14),py+rng.uniform(-.08,.08),rng.uniform(.92,1.30))],[(0,1,2)],'hedgelight' if (n+k)%4==0 else 'hedge')
    for x in [-7.25,7.25]:box('Side rail',(x,-.15,.04),(.28,15.25,.32),'wood',bevel=.05)
    box('Top rail',(0,7.55,.0),(14.8,.23,.27),'wood',bevel=.04)
    box('Bottom lip',(0,-7.72,-.13),(14.8,.22,.18),'wood',bevel=.04)
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
    clips=['Idle','Hop','Squash','Drown','Celebrate'] if kind in ('frog','lady_frog') else ['Idle','Swim','Dive'] if kind=='turtle' else ['Idle','Bite'] if kind=='gator' else ['Idle','Move']
    bpy.context.scene.render.fps=60
    rig.animation_data_create()
    for clip in clips:
        duration=10 if clip=='Hop' else 48 if clip=='Squash' else 64 if clip=='Drown' else 60
        action=bpy.data.actions.new(clip);action.use_fake_user=True;rig.animation_data.action=action
        for frame in range(duration+1):
            p=frame/duration;cycle=math.sin(p*math.tau);h=math.sin(p*math.pi)
            for b in rig.pose.bones:
                b.rotation_mode='XYZ';b.location=(0,0,0);b.rotation_euler=(0,0,0);b.scale=(1,1,1)
                if b.name=='Body':
                    b.scale=(1,1,1+.014*cycle) if clip=='Idle' else (1,1,1)
                    if clip=='Hop':b.location.y=.24*h;b.rotation_euler.x=-.16*h
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
                if clip=='Squash':
                    # Impact, then HOLD the flattened pose. No bobbing/recovery loop.
                    s=min(1,p/.14);s=s*s*(3-2*s)
                    b.rotation_euler=(0,0,0)
                    # Overall flattening is applied to the entire GLB root in
                    # Godot, so every weighted part compresses together. Bone
                    # non-uniform scale has incompatible inheritance in importers.
                    if b.name=='Body':b.scale=(1,1,1)
                    if b.name=='Head':b.rotation_euler.x=.18*s
                    if 'Thigh' in b.name:b.rotation_euler.x=.38*s
                    if 'Shin' in b.name:b.rotation_euler.x=-.38*s
                    if 'Arm.' in b.name:b.rotation_euler.z=(.25 if b.name.endswith('L') else -.25)*s
                if clip=='Drown':
                    # One monotonic descent; the game samples this clip from a
                    # continuous death clock, never the ROM's resetting subcounter.
                    sink=p*p*(3-2*p)
                    # Whole-character descent has one owner in presentation;
                    # the baked rig supplies the struggling head and limb motion.
                    if b.name=='Body':b.rotation_euler.x=.12*math.sin(p*math.pi)
                    if 'Arm.' in b.name:b.rotation_euler.x=-.6*math.sin(p*math.tau*2)*math.sin(p*math.pi)
                    if 'Thigh' in b.name:b.rotation_euler.x=.22*math.sin(p*math.tau*2)*math.sin(p*math.pi)
                for channel in ['location','rotation_euler','scale']:b.keyframe_insert(channel,frame=frame,group=b.name)
        track=rig.animation_data.nla_tracks.new();track.name=clip
        strip=track.strips.new(clip,0,action);strip.action_frame_start=0;strip.action_frame_end=duration
        track.mute=True
    rig.animation_data.action=None
    for track in rig.animation_data.nla_tracks:track.mute=False
    bpy.context.scene.frame_set(0)
    if kind in ('frog','lady_frog'):
        socket=bpy.data.objects.new('PassengerSocket',None);bpy.context.collection.objects.link(socket)
        socket.parent=rig;socket.parent_type='BONE';socket.parent_bone='Body'
        bpy.context.view_layer.update();socket.matrix_world=Matrix.Translation((0,.075,.46))
        bpy.context.view_layer.update()
        assert (socket.matrix_world.translation-Vector((0,.075,.46))).length<.0001
    return clips

def export(kind,specs):
    clips=rig_and_clips(specs,kind) if specs else []
    bpy.context.scene.frame_set(0)
    bpy.context.view_layer.update()
    meshes=[o for o in bpy.context.scene.objects if o.type=='MESH']
    # These are the actual unanimated world-space mesh bounds. The native
    # collision hook consumes the same authored silhouettes seen in Godot.
    vertices=[o.matrix_world @ v.co for o in meshes for v in o.data.vertices]
    footprint={'minX':round(min(p.x for p in vertices),6),'maxX':round(max(p.x for p in vertices),6),
               'minY':round(min(p.y for p in vertices),6),'maxY':round(max(p.y for p in vertices),6),
               'minZ':round(min(p.z for p in vertices),6),'maxZ':round(max(p.z for p in vertices),6)}
    bpy.ops.wm.save_as_mainfile(filepath=str(SOURCE / (kind+'.blend')))
    bpy.ops.export_scene.gltf(filepath=str(OUT/(kind+'.glb')),export_format='GLB',export_animations=True,
        export_animation_mode='NLA_TRACKS',export_force_sampling=True,export_yup=True,export_apply=False)
    triangles=sum(sum(len(p.vertices)-2 for p in o.data.polygons) for o in meshes)
    RECORDS.append({'asset':kind,'triangles':triangles,'bones':len(specs) if specs else 0,'clips':clips,
        'source':f'art/source/{kind}.blend','glb':f'godot/Models/{kind}.glb','footprintTiles':footprint})

for kind,builder in [('frog',frog),('lady_frog',lady_frog),('turtle',turtle),('gator',gator),('fly',fly),('snake',snake),('otter',otter),
                     ('log',log),('car',lambda:vehicle('car')),('truck',lambda:vehicle('truck')),
                     ('racecar',lambda:vehicle('racecar')),('dozer',lambda:vehicle('dozer')),
                     ('sport',lambda:vehicle('sport')),('board',board)]:
    clean();specs=builder();export(kind,specs)
(ROOT/'art/models.json').write_text(json.dumps(RECORDS,indent=2)+'\n')
footprints={r['asset']:r['footprintTiles'] for r in RECORDS}
assert -1.20 < -.22-.70+footprints['turtle']['minZ']-.12, 'Maximum dive clips turtle feet against the river bed'
frog=footprints['frog']
source=['// Generated from the Blender mesh bounds in art/scripts/build_assets.py.',
    'namespace FroggerRemake;',
    'public readonly record struct ModelFootprint(float MinAlongX,float MaxAlongX,float AcrossRow);',
    'public static class ModelFootprints {',
    f'    public const float FrogAlongX={max(abs(frog["minX"]),abs(frog["maxX"]))*16:.5f}f;',
    f'    public const float FrogAcrossRow={max(abs(frog["minY"]),abs(frog["maxY"]))*16:.5f}f;',
    '    // Vehicles rotate their local length axis into the ROM X direction.',
    '    // Presentation scales every vehicle root to 0.84.',
    '    public static ModelFootprint Vehicle(int lane)=>lane switch {']
for lane,kind in [(6,'truck'),(7,'sport'),(8,'car'),(9,'dozer'),(10,'racecar')]:
    b=footprints[kind]
    # In GLB, Blender +Y maps to Godot -Z. Odd lanes rotate +90 degrees,
    # reversing the model's longitudinal direction in ROM X coordinates.
    min_x,max_x=(b['minY'],b['maxY']) if lane%2==0 else (-b['maxY'],-b['minY'])
    cross=max(abs(b['minX']),abs(b['maxX']))
    source.append(f'        {lane}=>new({min_x*16*.84:.5f}f,{max_x*16*.84:.5f}f,{cross*16*.84:.5f}f), // {kind}')
source+=['        _=>throw new System.ArgumentOutOfRangeException(nameof(lane))','    };','}']
(ROOT/'godot/Scripts/ModelFootprints.cs').write_text('\n'.join(source)+'\n')
print('FROGGER_ASSETS_COMPLETE',len(RECORDS))
