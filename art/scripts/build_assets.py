"""Editable low-poly models, deform rigs and baked clips. Run with Blender --background --python.

Coordinate contract follows the PixelHack workflow: Z up, -Y forward; GLB Y up,
+Z forward. One game tile = one Blender unit. No external asset dependencies.
"""
from pathlib import Path
import math
import json
import os
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
    'shell':'13652e','shelllight':'62bc42','shellmid':'348f34','water':'091c60','road':'171d28',
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
    # Fake-user actions from the previous asset otherwise keep the same clip
    # names alive (Bite.001, Move.003) and can be picked up by review/import.
    for action in list(bpy.data.actions):bpy.data.actions.remove(action,do_unlink=True)

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
        inset=min(.035,(high-low)*.25)
        verts.extend([(-width*.78,y,high),(width*.78,y,high),
            (width,y,high-inset),(width,y,low+inset),
            (width*.78,y,low),(-width*.78,y,low),
            (-width,y,low+inset),(-width,y,high-inset)])
    faces=[]
    for ring in range(len(sections)-1):
        for side in range(8):
            next_side=(side+1)%8;a=ring*8+side;b=(ring+1)*8+side
            faces.append((a,b,(ring+1)*8+next_side,ring*8+next_side))
    # The section list runs from the rear toward Blender -Y (the snout tip).
    # Its ring winding faces +Y, so the front end needs the REVERSED winding
    # or Godot culls that cap and exposes a hole in the jaws.
    rear_cap=tuple(range(8))
    front_cap=tuple(reversed([(len(sections)-1)*8+i for i in range(8)]))
    a,b,c=(Vector(verts[i]) for i in front_cap[:3])
    assert (b-a).cross(c-a).y<0
    faces.extend([rear_cap,front_cap])
    obj=mesh(name,verts,faces,color);obj['rig_bone']=bone
    return obj

def eyes(y,z,x=.19,scale=.11,bone='Head'):
    for sign in [-1,1]:
        ball('Eye mound', (sign*x,y+.014,z-.03), (scale*1.26,scale*.86,scale*1.2),'green',bone)
        ball('Golden eye',(sign*x,y-.056,z+.016),(scale*.79,scale*.56,scale*.86),'eye',bone)
        ball('Pupil',(sign*x,y-.102,z+.02),(scale*.38,scale*.2,scale*.59),'ink',bone)
        ball('Eye glint',(sign*x-.02,y-.12,z+.053),(scale*.16,scale*.1,scale*.16),'white',bone,8,4)

def frog_back_height(x,y):
    """Top of the frog's ellipsoid body, shared by every painted back mark."""
    radius=(x/.27)**2+((y-.035)/.33)**2
    assert radius<1,(x,y,radius)
    return .23+.22*math.sqrt(1-radius)+.012

def frog_back_spot(x,y):
    verts=[(x,y,frog_back_height(x,y))]
    for index in range(10):
        angle=index*math.tau/10
        px=x+.035*math.cos(angle);py=y+.047*math.sin(angle)
        verts.append((px,py,frog_back_height(px,py)))
    mesh('Contoured back spot',verts,[(0,1+i,1+(i+1)%10) for i in range(10)],'darkgreen')

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
            frog_back_spot(sign*.12,y)
    # The PS1 reference's orange dorsal marking keeps the player readable on grass.
    stripe=[]
    for y,half_width in [(-.08,.018),(-.025,.033),(.035,.038),(.095,.037),(.155,.031),(.215,.023),(.275,.010)]:
        stripe.extend([(-half_width,y,frog_back_height(-half_width,y)),
                       (half_width,y,frog_back_height(half_width,y))])
    mesh('Contoured orange back stripe',stripe,[(2*i,2*i+1,2*i+3,2*i+2) for i in range(len(stripe)//2-1)],'orange')
    return specs

def lady_frog():
    specs=frog()
    swaps={'green':'lady','lime':'ladylight','darkgreen':'ladydark','orange':'white'}
    for obj in bpy.context.scene.objects:
        if obj.type=='MESH':
            for slot in obj.material_slots:
                if slot.material.name in swaps:slot.material=material(swaps[slot.material.name])
    return specs

def turtle_shell_height(x,y):
    radius=(x/.36)**2+((y-.03)/.4)**2
    assert radius<1,(x,y,radius)
    return .21+.22*math.sqrt(1-radius)+.012

def turtle_shell_plate(x,y,rx,ry,color):
    verts=[(x,y,turtle_shell_height(x,y))]
    for index in range(6):
        angle=math.tau*(index+.5)/6
        px=x+rx*math.cos(angle);py=y+ry*math.sin(angle)
        verts.append((px,py,turtle_shell_height(px,py)))
    mesh('Contoured shell plate',verts,[(0,1+i,1+(i+1)%6) for i in range(6)],color)

def turtle_webbed_paddle(side,fore_aft,y,bone):
    # Local u points outward from the shell; v points away from the nearest
    # body corner. Inset the whole fan diagonally toward the turtle's center,
    # while preserving three projecting tips and two web notches.
    outline=[(.10,-.08,.13),(.43,.045,.10),(.64,.11,.060),(.57,.16,.058),
             (.66,.22,.055),(.57,.235,.057),(.59,.335,.055),(.37,.28,.075),(.12,.13,.12)]
    center_u,center_v=.42,.17
    inset_u=.30*center_u
    inset_y=.30*(.03-(y+fore_aft*center_v))-fore_aft*.05
    def point(u,v,z):
        smaller_u=center_u+(u-center_u)*.86-inset_u
        smaller_v=center_v+(v-center_v)*.75
        flatter_z=.055+(z-.055)*.72
        return (side*smaller_u,y+fore_aft*smaller_v+inset_y,flatter_z)
    top=[point(.42,.17,.09)]+[point(*p) for p in outline]
    bottom=[(x,yy,z-.023) for x,yy,z in top]
    vertices=top+bottom;count=len(outline);bottom_center=count+1
    faces=[]
    for i in range(count):
        a=1+i;b=1+(i+1)%count;ba=bottom_center+a;bb=bottom_center+b
        if side*fore_aft>0:
            faces.extend([(0,a,b),(bottom_center,bb,ba),(a,ba,bb,b)])
        else:
            faces.extend([(0,b,a),(bottom_center,ba,bb),(b,bb,ba,a)])
    paddle=mesh('Angled webbed paddle',vertices,faces,'green');paddle['rig_bone']=bone
    for index in [2,4,6]:
        u,v,z=outline[index]
        cylinder('Webbed toe ray',point(.39,.15,.096),point(u,v,z+.007),.011,'lime',bone,6,r2=.004)

def turtle():
    ball('Shell',(0,.03,.21),(.36,.4,.22),'shell')
    # Separate faceted plates follow the same dome as the dark shell. The
    # exposed dark seams echo the larger PS1 shell markings at gameplay size.
    for x,y,rx,ry,color in [(0,.025,.115,.13,'shelllight'),
                           (-.19,.02,.085,.115,'shellmid'),(.19,.02,.085,.115,'shellmid'),
                           (0,-.205,.105,.075,'shelllight'),(0,.245,.105,.075,'shelllight')]:
        turtle_shell_plate(x,y,rx,ry,color)
    ball('Head',(0,-.43,.15),(.14,.18,.125),'green','Head')
    for x in [-.085,.085]:
        ball('Eye',(x,-.5,.22),(.034,.042,.035),'ink','Head',8,4)
    specs={'Body':((0,0,.08),(0,0,.3),None),'Head':((0,-.25,.15),(0,-.48,.16),'Body')}
    for side,s in [('L',-1),('R',1)]:
        for end,y in [('Front',-.22),('Back',.28)]:
            bone=end+side;direction=-1 if end=='Front' else 1
            specs[bone]=((s*.15,y,.12),(s*.52,y+direction*.19,.07),'Body')
            turtle_webbed_paddle(s,direction,y,bone)
    def limb_edge(prefix,fn):
        return fn((o.matrix_world @ vertex.co).y for o in bpy.context.scene.objects
                  if o.type=='MESH' and o.get('rig_bone','').startswith(prefix)
                  for vertex in o.data.vertices)
    # Two turtles in a group have centers one tile apart after the model turns
    # into the lane. Their facing front/back fans must leave a visible gap.
    pair_gap=1-(limb_edge('Back',max)-limb_edge('Front',min))
    assert pair_gap>.20,pair_gap
    print('TURTLE_PAIR_GAP',round(pair_gap,4))
    return specs

def gator(river=False):
    ball('Armored trunk',(0,.15,.17),(.32,.54,.19),'shell',segments=12,rings=7)
    if river:
        cylinder('Long tapering tail',(0,.51,.17),(0,1.12,.05),.21,'shell',vertices=10,r2=.018)
    else:
        # A straight rearward tail vanishes behind the home bay's back hedge.
        # Curl the same long tapered silhouette around the gator's right side,
        # inside the open bay, without pushing its snout into the river.
        tail_path=[((0,.51,.17),(.31,.49,.19),.21,.16),
                   ((.31,.49,.19),(.65,.28,.20),.16,.09),
                   ((.65,.28,.20),(.84,.04,.16),.09,.012)]
        for a,b,radius,tip_radius in tail_path:
            cylinder('Curved tapering tail',a,b,radius,'shell',vertices=10,r2=tip_radius)
    upper=[(-.425 if river else -.18,.25,.12,.36),
           (-.58 if river else -.39,.205,.13,.33),
           (-.78 if river else -.70,.155,.115,.27),
           (-.93 if river else -.91,.192,.11,.27),(-1.03,.147,.12,.235)]
    if river:
        # The dark bridge belongs visually to the rideable back, ending where
        # the bright 16px dangerous snout begins. It closes the eye/body gap.
        muzzle('River cheek and neck bridge',
               [(-.17,.25,.12,.33),(-.43,.25,.12,.36)],'darkgreen','Head')
        muzzle('River throat bridge',
               [(-.17,.215,.055,.13),(-.43,.215,.055,.13)],'cream','Jaw')
    muzzle('Tapered upper snout',upper,'green','Head')
    muzzle('Lower resting jaw',[
        (-.42 if river else -.17,.215,.055,.13),(-.61 if river else -.44,.174,.045,.13),
        (-.83 if river else -.81,.175,.045,.135),(-.99,.137,.055,.12)],'cream','Jaw')
    ball('Dark mouth',(0,-.74 if river else -.67,.127),(.165,.25 if river else .36,.018),'ink','Jaw',segments=10,rings=4)
    # Contour the stripe to the actual snout rings. Winding faces UP so Godot
    # renders it with backface culling, unlike the old Blender-only appearance.
    ridge=[]
    for y,width,low,high in upper:
        ridge.extend([(-width*.36,y,high+.018),(width*.36,y,high+.018)])
    ridge_faces=[(2*i,2*i+2,2*i+3,2*i+1) for i in range(len(upper)-1)]
    stripe=mesh('Raised center snout accent',ridge,ridge_faces,'lime');stripe['rig_bone']='Head'
    assert all(face.normal.z>.5 for face in stripe.data.polygons),'Snout accent faces must be visible from above'
    if river:
        assert 15.5 < (.425-1.03)*-1*(57/2.153474) < 16.5,'River snout must match the 16px ROM kill window'
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
    tail_spikes=[(.03,.32),(.21,.34),(.39,.30)]
    if river:tail_spikes.extend([(.57,.24),(.74,.17)])
    for y,z in tail_spikes:
        for x in [-.12,.12]: cylinder('Back spike',(x,y,z),(x,y,z+.15),.067,'darkgreen',vertices=5,r2=0)
    if not river:
        for x,y,z in [(.25,.50,.22),(.44,.40,.25),(.63,.29,.24),(.77,.14,.19)]:
            for offset in [-.05,.05]:
                cylinder('Curved tail spike',(x+offset,y,z),(x+offset,y,z+.13),.05,'darkgreen',vertices=5,r2=0)
    return {'Body':((0,0,.1),(0,0,.3),None),'Head':((0,-.1,.2),(0,-.5,.22),'Body'),
            'Jaw':((0,-.2,.09),(0,-.72,.08),'Body')}

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
        if i:specs[f'Segment{i}']=((x,y,.13),(x,y+.19,.13),'Body' if i==1 else f'Segment{i-1}')
    # One watertight, skinned tube: its rings bend on the eight original rig
    # sections, while alternating material bands keep segmentation legible.
    stations=[(-.92,.072,.055),(-.85,.13,.095),(-.76,.17,.13),
              (-.67,.16,.12),(-.58,.125,.10),(-.43,.12,.095),
              (-.25,.118,.09),(-.06,.113,.085),(.13,.105,.08),
              (.32,.095,.075),(.51,.078,.068),(.70,.055,.052),(.88,.02,.025)]
    sides=10;verts=[];weights=[];faces=[];materials=[]
    def skin_at(y):
        index=max(0,min(7,int(math.floor((y+.6)/.19))))
        if index==7:return {'Segment7':1.0}
        mix=max(0,min(1,(y+.6)/.19-index))
        first='Body' if index==0 else f'Segment{index}'
        return {first:1-mix,f'Segment{index+1}':mix} if mix>.0001 else {first:1.0}
    for y,width,height in stations:
        drift=0 if y<-.58 else math.sin((y+.58)*4.5)*.14
        for side in range(sides):
            angle=math.tau*side/sides
            verts.append((drift+width*math.cos(angle),y,.145+height*math.sin(angle)))
            weights.append(skin_at(y))
    for ring in range(len(stations)-1):
        for side in range(sides):
            next_side=(side+1)%sides
            faces.append((ring*sides+side,(ring+1)*sides+side,
                          (ring+1)*sides+next_side,ring*sides+next_side))
            materials.append(2 if ring<4 else 1 if ring%2 else 0)
    faces.append(tuple(range(sides)));materials.append(2)
    faces.append(tuple(reversed([(len(stations)-1)*sides+i for i in range(sides)])));materials.append(0)
    tube=mesh('Continuous segmented snake',verts,faces,'wooddark')
    tube.data.materials.append(material('yellow'));tube.data.materials.append(material('green'))
    for polygon,color in zip(tube.data.polygons,materials):polygon.material_index=color
    tube['rig_weights_json']=json.dumps(weights)
    for s in [-1,1]:ball('Eye',(s*.105,-.79,.245),(.035,.04,.038),'ink',segments=8,rings=4)
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
    # Faceted trunk with subtle bends and taper. Its authored length remains
    # one tile, so the lane's ROM-sized width scale still fits every log.
    stations=[(-.5,-.006,.213,.22),(-.39,.008,.235,.25),
              (-.2,-.012,.25,.268),(.02,.014,.245,.263),
              (.23,-.005,.24,.255),(.40,.004,.228,.242),(.5,-.008,.208,.218)]
    sides=10
    def profile(x):
        for index in range(len(stations)-1):
            left,right=stations[index:index+2]
            if x<=right[0]:
                t=max(0,min(1,(x-left[0])/(right[0]-left[0])))
                return tuple(left[k]+(right[k]-left[k])*t for k in range(1,4))
        return stations[-1][1:]
    def surface(x,angle,lift=1):
        drift,ry,rz=profile(x)
        return (x,drift+ry*math.cos(angle)*lift,.16+rz*math.sin(angle)*lift)
    verts=[surface(x,math.tau*j/sides) for x,*_ in stations for j in range(sides)]
    faces=[]
    for ring in range(len(stations)-1):
        for side in range(sides):
            a=ring*sides+side;b=ring*sides+(side+1)%sides
            c=(ring+1)*sides+(side+1)%sides;d=(ring+1)*sides+side
            faces.append((a,b,c,d))
    faces.append(tuple(reversed(range(sides))))
    faces.append(tuple((len(stations)-1)*sides+i for i in range(sides)))
    mesh('Irregular faceted bark',verts,faces,'wood')
    def bark_strip(name,points,color):
        vertices=[]
        for x,angle,half in points:
            vertices.extend([surface(x,angle-half,1.018),surface(x,angle+half,1.018)])
        polygons=[(2*i,2*i+1,2*i+3,2*i+2) for i in range(len(points)-1)]
        mesh(name,vertices,polygons,color)
    for name,points,color in [
        ('Broken dark bark',[(-.46,.62,.015),(-.29,.59,.065),(-.1,.70,.05),(.1,.61,.045),(.31,.69,.065),(.44,.65,.012)],'wooddark'),
        ('Broken upper bark',[(-.42,1.96,.01),(-.27,1.89,.045),(-.08,1.99,.055),(.13,1.91,.04),(.28,1.97,.055),(.42,1.90,.008)],'wooddark'),
        ('Warm split bark',[(-.36,1.36,.012),(-.22,1.34,.035),(-.05,1.42,.028),(.1,1.37,.04),(.22,1.41,.01)],'woodlight'),
        ('Lower side bark',[(-.42,2.49,.01),(-.24,2.45,.055),(-.02,2.53,.04),(.19,2.46,.055),(.4,2.5,.012)],'wooddark')]:
        bark_strip(name,points,color)
    def knot(x,angle):
        outer=[];inner=[]
        for i in range(9):
            a=math.tau*i/8
            outer.append(surface(x+.085*math.cos(a),angle+.23*math.sin(a),1.028))
            inner.append(surface(x+.043*math.cos(a),angle+.12*math.sin(a),1.035))
        mesh('Bark knot rim',outer+inner,
             [(i,i+1,9+i+1,9+i) for i in range(8)],'wooddark')
        mesh('Bark knot center',inner,[(0,i,i+1) for i in range(1,7)],'woodlight')
    knot(-.14,1.62);knot(.29,1.20)
    for sign in [-1,1]:
        def grain(radius,x,color,name):
            points=[(sign*x,radius*math.cos(math.tau*i/sides),.16+radius*math.sin(math.tau*i/sides)) for i in range(sides)]
            polygons=[tuple(reversed(range(sides))) if sign<0 else tuple(range(sides))]
            mesh(name,points,polygons,color)
        grain(.19,.503,'woodlight','Pale cut end')
        grain(.145,.506,'wooddark','End grain ring')
        grain(.108,.509,'woodlight','End grain core')
        grain(.045,.511,'wood','Small heartwood')
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
        # Cards used to sit only .003 above the bank and could overlap other
        # coplanar cards. Give each triangle its own jittered cell and a real
        # depth gap while keeping the detail as flat painted marks.
        columns,rows=36,4*width
        cell_x,cell_y=14/columns,width/rows
        for column in range(columns):
            for row in range(rows):
                if rng.random()>.90:continue
                x=-7+(column+.5)*cell_x+rng.uniform(-.025,.025)
                yy=y-width/2+(row+.5)*cell_y+rng.uniform(-.015,.015)
                half_x=rng.uniform(.08,.145);half_y=rng.uniform(.04,.075)
                tip=rng.uniform(-.03,.03)
                mesh('Painted grass patch',[(x-half_x,yy-half_y,.094),(x+half_x,yy-half_y,.094),(x+tip,yy+half_y,.094)],[(0,1,2)],
                     'grasslight' if (column+row)%3 else 'grassdark')
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
        if 'rig_weights_json' in obj:
            groups={name:obj.vertex_groups.new(name=name) for name in specs}
            for vertex,assignments in enumerate(json.loads(obj['rig_weights_json'])):
                for bone,weight in assignments.items():
                    if weight>.0001:groups[bone].add([vertex],weight,'REPLACE')
        else:
            group=obj.vertex_groups.new(name=obj['rig_bone']);group.add(list(range(len(obj.data.vertices))),1,'REPLACE')
        mod=obj.modifiers.new('Deform rig','ARMATURE');mod.object=rig;obj.parent=rig
    clips=['Idle','Hop','Squash','Drown','Celebrate'] if kind in ('frog','lady_frog') else ['Idle','Swim','Dive'] if kind=='turtle' else ['Idle','Bite'] if kind in ('gator','river_gator') else ['Idle','Move']
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
                if 'Segment' in b.name:b.rotation_euler.z=.07*math.sin(p*math.tau+int(b.name[-1]))
                if b.name=='Head' and clip=='Bite':b.rotation_euler.x=-.55*h
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

selected=set(filter(None,os.environ.get('FROGGER_BUILD_ASSETS','').split(',')))
for kind,builder in [('frog',frog),('lady_frog',lady_frog),('turtle',turtle),('gator',gator),('river_gator',lambda:gator(True)),('fly',fly),('snake',snake),('otter',otter),
                     ('log',log),('car',lambda:vehicle('car')),('truck',lambda:vehicle('truck')),
                     ('racecar',lambda:vehicle('racecar')),('dozer',lambda:vehicle('dozer')),
                     ('sport',lambda:vehicle('sport')),('board',board)]:
    if selected and kind not in selected:continue
    clean();specs=builder();export(kind,specs)
if selected:
    previous=json.loads((ROOT/'art/models.json').read_text())
    replacements={record['asset']:record for record in RECORDS}
    assert replacements.keys()==selected,(selected,replacements.keys())
    RECORDS[:]=[replacements.get(record['asset'],record) for record in previous]
(ROOT/'art/models.json').write_text(json.dumps(RECORDS,indent=2)+'\n')
footprints={r['asset']:r['footprintTiles'] for r in RECORDS}
assert -1.20 < -.22-.70+footprints['turtle']['minZ']-.12, 'Maximum dive clips turtle feet against the river bed'
frog=footprints['frog']
gator_bounds=footprints['river_gator']
source=['// Generated from the Blender mesh bounds in art/scripts/build_assets.py.',
    'namespace FroggerRemake;',
    'public readonly record struct ModelFootprint(float MinAlongX,float MaxAlongX,float AcrossRow);',
    'public static class ModelFootprints {',
    f'    public const float FrogAlongX={max(abs(frog["minX"]),abs(frog["maxX"]))*16:.5f}f;',
    f'    public const float FrogAcrossRow={max(abs(frog["minY"]),abs(frog["maxY"]))*16:.5f}f;',
    f'    public const float RiverGatorLengthTiles={gator_bounds["maxY"]-gator_bounds["minY"]:.6f}f;',
    f'    public const float RiverGatorWidthTiles={gator_bounds["maxX"]-gator_bounds["minX"]:.6f}f;',
    f'    public const float RiverGatorFrontTiles={-gator_bounds["minY"]:.6f}f;',
    '    public const float RiverGatorSnoutTiles=0.605000f;',
    f'    public const float LogTopTiles={footprints["log"]["maxZ"]:.6f}f;',
    f'    public const float SnakeBottomTiles={footprints["snake"]["minZ"]:.6f}f;',
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
