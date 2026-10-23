"""Inspect exported geometry, skin weights, real joints and baked animation channels."""
from pathlib import Path
import json,struct,math
ROOT=Path(__file__).resolve().parents[1]
SIZES={'SCALAR':1,'VEC2':2,'VEC3':3,'VEC4':4,'MAT4':16}
FORMATS={5120:'b',5121:'B',5122:'h',5123:'H',5125:'I',5126:'f'}
results=[]

def node_matrix(node):
    """Row-major 4x4 TRS matrix for one glTF node."""
    tx,ty,tz=node.get('translation',(0,0,0));x,y,z,w=node.get('rotation',(0,0,0,1));sx,sy,sz=node.get('scale',(1,1,1))
    r=[1-2*(y*y+z*z),2*(x*y-z*w),2*(x*z+y*w),2*(x*y+z*w),1-2*(x*x+z*z),2*(y*z-x*w),
       2*(x*z-y*w),2*(y*z+x*w),1-2*(x*x+y*y)]
    return [r[0]*sx,r[1]*sy,r[2]*sz,tx,r[3]*sx,r[4]*sy,r[5]*sz,ty,r[6]*sx,r[7]*sy,r[8]*sz,tz,0,0,0,1]

def multiply(a,b):
    return [sum(a[i*4+k]*b[k*4+j] for k in range(4)) for i in range(4) for j in range(4)]

def apply(m,v):
    return (m[0]*v[0]+m[1]*v[1]+m[2]*v[2]+m[3],m[4]*v[0]+m[5]*v[1]+m[6]*v[2]+m[7],m[8]*v[0]+m[9]*v[1]+m[10]*v[2]+m[11])

for contract in json.loads((ROOT/'art/models.json').read_text()):
    footprint=contract['footprintTiles']
    assert all(math.isfinite(footprint[axis]) for axis in ('minX','maxX','minY','maxY','minZ','maxZ'))
    assert footprint['minX']<footprint['maxX'] and footprint['minY']<footprint['maxY'] and footprint['minZ']<footprint['maxZ']
    data=(ROOT/contract['glb']).read_bytes()
    import_text=(ROOT/(contract['glb']+'.import')).read_text()
    for setting in ('array_mesh/deduplicate_surfaces=true','meshes/generate_lods=true',
                    'meshes/create_shadow_meshes=true','meshes/force_disable_compression=false'):
        assert setting in import_text,(contract['asset'],'missing Godot mesh optimization',setting)
    magic,version,length=struct.unpack_from('<III',data)
    assert magic==0x46546c67 and version==2 and length==len(data)
    size=struct.unpack_from('<I',data,12)[0];doc=json.loads(data[20:20+size]);binary=data[28+size:]
    def values(index):
        a=doc['accessors'][index];v=doc['bufferViews'][a['bufferView']];fmt=FORMATS[a['componentType']];n=SIZES[a['type']]
        stride=v.get('byteStride',struct.calcsize(fmt)*n);start=v.get('byteOffset',0)+a.get('byteOffset',0)
        rows=[struct.unpack_from('<'+fmt*n,binary,start+i*stride) for i in range(a['count'])]
        if a.get('normalized'):
            scale={5121:255,5123:65535,5120:127,5122:32767}[a['componentType']]
            rows=[tuple(x/scale for x in row) for row in rows]
        return rows
    # Rigid parts (the board, log bodies, vehicle wheels) keep authored vertices
    # in their own node space, so their bounds only read correctly once the node
    # chain is applied. Skinned meshes stay in bind space, where the runtime
    # ignores the mesh node transform and the joints already place the vertices.
    world_matrices={}
    def visit(index,parent):
        world_matrices[index]=multiply(parent,node_matrix(doc['nodes'][index]))
        for child in doc['nodes'][index].get('children',[]):visit(child,world_matrices[index])
    identity=[1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1]
    for root in doc['scenes'][doc.get('scene',0)]['nodes']:visit(root,identity)
    mesh_nodes={}
    for index,node in enumerate(doc['nodes']):
        if 'mesh' in node and node['mesh'] not in mesh_nodes:mesh_nodes[node['mesh']]=index
    vertices=0;skinned=0;blended=0;triangles=0;max_error=0;material_surfaces=0
    skinned_materials=set()
    exported_x=[];exported_y=[];exported_z=[]
    for index,mesh in enumerate(doc['meshes']):
        node_index=mesh_nodes.get(index)
        node=doc['nodes'][node_index] if node_index is not None else {}
        node_space=identity if 'skin' in node else world_matrices.get(node_index,identity)
        if 'skin' in node:
            signature=tuple(sorted(doc['materials'][p['material']]['name'] for p in mesh['primitives']))
            overlap=skinned_materials.intersection(signature)
            assert not overlap,(contract['asset'],'unconsolidated skinned mesh materials',sorted(overlap))
            skinned_materials.update(signature)
        for p in mesh['primitives']:
            material_surfaces+=1
            positions=values(p['attributes']['POSITION']);vertices+=len(positions)
            assert all(math.isfinite(v) for row in positions for v in row)
            world=[apply(node_space,row) for row in positions]
            exported_x.extend(row[0] for row in world)
            exported_y.extend(-row[2] for row in world)
            exported_z.extend(row[1] for row in world)
            triangles+=doc['accessors'][p['indices']]['count']//3
            if 'WEIGHTS_0' in p['attributes']:
                weights=values(p['attributes']['WEIGHTS_0']);skinned+=len(weights)
                for row in weights:
                    if sum(weight>.001 for weight in row)>1:blended+=1
                    assert min(row)>=0;error=abs(sum(row)-1);max_error=max(max_error,error);assert error<.005
    clips={a['name']:a for a in doc.get('animations',[])}
    bones=max([len(s['joints']) for s in doc.get('skins',[])],default=0)
    # Blender +Y becomes GLB -Z. Compare independently read exported vertices
    # with the bounds that generated the native collision constants.
    if contract['asset'] in ('frog','truck','sport','car','dozer','racecar'):
        for key,actual in [('minX',min(exported_x)),('maxX',max(exported_x)),
                           ('minY',min(exported_y)),('maxY',max(exported_y)),
                           ('minZ',min(exported_z)),('maxZ',max(exported_z))]:
            assert abs(footprint[key]-actual)<.02,(contract['asset'],key,footprint[key],actual)
    if contract['asset'] in ('frog','lady_frog'):
        front=max(abs(x) for x,y in zip(exported_x,exported_y) if y<-.22)
        rear=max(abs(x) for x,y in zip(exported_x,exported_y) if y>.15)
        assert abs(front-rear)<.025,(contract['asset'],'front and rear leg spans differ',front,rear)
    if contract['asset']=='turtle':
        assert -1.20 < -.22-.70+footprint['minZ']-.12,'Maximum dive would clip the turtle paddles'
    assert triangles==contract['triangles'],(contract['asset'],triangles,contract['triangles'])
    if contract['bones']:
        assert bones==contract['bones'],(contract['asset'],bones,contract['bones'])
        assert skinned==vertices,(contract['asset'],'unbound vertices')
        for clip in contract['clips']:
            matches=[a for n,a in clips.items() if n.endswith(clip)]
            assert len(matches)==1,(contract['asset'],clip,list(clips))
            a=matches[0];moving=0
            for sampler in a['samplers']:
                samples=values(sampler['output'])
                if any(row!=samples[0] for row in samples[1:]):moving+=1
            assert moving>0,(contract['asset'],clip,'static clip')
    if contract['asset'] in ('frog','lady_frog'):
        sockets=[i for i,n in enumerate(doc['nodes']) if n.get('name')=='PassengerSocket']
        assert len(sockets)==1,'Missing passenger attachment'
        parent=next(n for n in doc['nodes'] if sockets[0] in n.get('children',[]))
        assert parent['name']=='Body','Passenger must inherit the animated body bone'
    if contract['asset'] in ('snake','otter'):
        assert blended>0,(contract['asset'],'custom blended skin weights were lost during mesh consolidation')
    result={'asset':contract['asset'],'vertices':vertices,'triangles':triangles,
        'mesh_instances':sum('mesh' in node for node in doc['nodes']),'material_surfaces':material_surfaces,
        'godot_mesh_optimizations':True,'bones':bones,'skinned_vertices':skinned,
        'blended_skin_vertices':blended,
        'max_weight_error':max_error,'clips':list(clips),'passed':True}
    if contract['asset'] in ('frog','lady_frog'):
        result['front_leg_span']=round(front*2,5)
        result['rear_leg_span']=round(rear*2,5)
    if contract['asset']=='turtle':
        result['max_dive_bed_clearance']=round((-.22-.70+footprint['minZ'])-(-1.20),5)
    results.append(result)
(ROOT/'docs/evidence/asset-validation.json').write_text(json.dumps(results,indent=2)+'\n')
print(f'{len(results)} Blender exports validated: finite geometry, triangle counts, consolidated meshes, Godot mesh optimizations, all vertices bound, normalized weights, expected joints and moving clips.')
# Backdrop vegetation (art/scripts/build_scenery.py): glTF-separate files in the
# board's own untextured, faceted style. Same cheap gates, plus: one vertex-
# colored material, unit normals, bounded COLOR_0 on every primitive, and edge
# hedges and reed walls that never cross the board's clip line.
scenery=[]
for contract in json.loads((ROOT/'art/scenery.json').read_text()):
    path=ROOT/contract['gltf']
    import_text=(ROOT/(contract['gltf']+'.import')).read_text()
    for setting in ('array_mesh/deduplicate_surfaces=true','meshes/generate_lods=true',
                    'meshes/create_shadow_meshes=true','meshes/force_disable_compression=false'):
        assert setting in import_text,(contract['asset'],'missing Godot mesh optimization',setting)
    doc=json.loads(path.read_text())
    assert not doc.get('images'),(contract['asset'],'vegetation is untextured like the board models')
    binary=(path.parent/doc['buffers'][0]['uri']).read_bytes()
    def read(index):
        a=doc['accessors'][index];v=doc['bufferViews'][a['bufferView']];fmt=FORMATS[a['componentType']];n=SIZES[a['type']]
        stride=v.get('byteStride',struct.calcsize(fmt)*n);start=v.get('byteOffset',0)+a.get('byteOffset',0)
        return [struct.unpack_from('<'+fmt*n,binary,start+i*stride) for i in range(a['count'])]
    triangles=0;min_x=float('inf')
    for mesh in doc['meshes']:
        for p in mesh['primitives']:
            assert doc['materials'][p['material']]['name']=='Scenery',(contract['asset'],'one shared vertex-colored material')
            positions=read(p['attributes']['POSITION']);normals=read(p['attributes']['NORMAL'])
            assert all(math.isfinite(v) for row in positions for v in row),(contract['asset'],'non-finite position')
            assert all(abs(math.sqrt(sum(c*c for c in n))-1)<.01 for n in normals),(contract['asset'],'non-unit normal')
            colors=read(p['attributes']['COLOR_0'])
            assert all(0<=c<=1.0001 for row in colors for c in row[:3]),(contract['asset'],'COLOR_0 out of range')
            min_x=min(min_x,min(row[0] for row in positions))
            triangles+=doc['accessors'][p['indices']]['count']//3
    assert triangles==contract['triangles'] and triangles<=6000,(contract['asset'],triangles,contract['triangles'])
    if contract['asset'].startswith('edge_hedge') or contract['asset']=='reed_wall':
        assert min_x>=-.001,(contract['asset'],'crosses the clip line',min_x)
    scenery.append({'asset':contract['asset'],'triangles':triangles,'leaves':contract['leaves'],'passed':True})
(ROOT/'docs/evidence/scenery-validation.json').write_text(json.dumps(scenery,indent=2)+'\n')
print(f'{len(scenery)} backdrop vegetation exports validated: budgets, unit normals, one vertex-colored material with bounded COLOR_0, clip-safe hedges and reed walls.')