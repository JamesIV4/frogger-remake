"""Backdrop vegetation around the board. Run with Blender --background --python.

Every model here shares the game's own detail level and vocabulary: untextured
flat palette colors, flat-shaded facets, and the board's hedge construction
(art/scripts/build_assets.py board(): angular faceted canopy masses broken up by
large pointed spear leaves). Following the foliage method in docs/ART.md
("Backdrop vegetation"):
- Wood is one continuous skeleton: a Skin modifier over trunk/limb/branch edges,
  with roots flaring out of the same surface, subdivided once and decimated to
  budget, then faceted like the logs.
- Crowns are irregular: unequal faceted lobes at the limb ends with gaps
  between them, never one smooth dome or evenly spaced balls; spear leaves
  splay outward from the lobe surfaces with random roll.
- COLOR_0 carries the palette color per part, with small value jitter and
  darker interior lobes. One material; Godot multiplies the vertex color.
Outputs glTF-separate files in godot/Models/Scenery, art/source/scenery_<name>
.blend, art/scenery.json and four-view review renders in docs/evidence/models.
Coordinates: Blender Z up, -Y forward; one game tile = one Blender unit.
"""
from pathlib import Path
import json
import math
import os
import random
import bpy
import bmesh
from mathutils import Vector

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'godot/Models/Scenery'
SOURCE = ROOT / 'art/source'
REVIEW = ROOT / 'docs/evidence/models'
for path in (OUT, SOURCE, REVIEW): path.mkdir(parents=True, exist_ok=True)

# Board palette (build_assets.py PALETTE) and in-between greens.
HEDGE, HEDGE_LIGHT, HEDGE_DARK = '216a0a', '70b91c', '082f0d'
CANOPY = ['216a0a', '2f7d14', '3f8f17', '2b6c20']
LEAF = ['70b91c', '4f9e1a', '3f8f17']
NEEDLE = ['13652e', '1d6e2a', '0f5a24']
NEEDLE_TIP = ['348f34', '62bc42']
GRASS = ['2b6c20', '3f8f17', '4f9e1a']
MOSS = ['5d7a3a', '6b8442']
WOOD, WOOD_DARK = '94501f', '462c23'
PETALS = ['ff269c', 'ffdd00', 'ff9acb', 'ff7a16']
BUDGET = 6000


def linear(hexcode, value=1.0):
    c = [int(hexcode[i:i + 2], 16) / 255 for i in (0, 2, 4)]
    return [min(1.0, value * (v / 12.92 if v < .04045 else ((v + .055) / 1.055) ** 2.4)) for v in c]


def reset():
    bpy.ops.wm.read_factory_settings(use_empty=True)


def material():
    mat = bpy.data.materials.get('Scenery')
    if mat: return mat
    mat = bpy.data.materials.new('Scenery')
    mat.use_nodes = True
    nt = mat.node_tree
    bsdf = nt.nodes.get('Principled BSDF')
    col = nt.nodes.new('ShaderNodeVertexColor')
    col.layer_name = 'Col'
    nt.links.new(col.outputs['Color'], bsdf.inputs['Base Color'])
    bsdf.inputs['Roughness'].default_value = .72
    return mat


class Parts:
    """Accumulates flat-shaded parts with one palette color per face."""

    def __init__(self):
        self.verts, self.faces, self.colors = [], [], []
        self.leaves = 0

    def add(self, verts, faces, color, two_sided=False):
        base = len(self.verts)
        self.verts += [tuple(v) for v in verts]
        for f in faces:
            self.faces.append(tuple(base + i for i in f)); self.colors.append(color)
            if two_sided:
                self.faces.append(tuple(base + i for i in reversed(f))); self.colors.append(color)

    def add_object(self, obj, color):
        me = obj.data
        self.add([obj.matrix_world @ v.co for v in me.vertices], [tuple(p.vertices) for p in me.polygons], color)
        bpy.data.objects.remove(obj, do_unlink=True)

    def build(self, name):
        me = bpy.data.meshes.new(name)
        me.from_pydata(self.verts, [], self.faces)
        col = me.color_attributes.new('Col', 'FLOAT_COLOR', 'CORNER')
        for poly, color in zip(me.polygons, self.colors):
            poly.use_smooth = False
            for loop in poly.loop_indices: col.data[loop].color = (*color, 1.0)
        me.materials.append(material())
        obj = bpy.data.objects.new(name, me)
        bpy.context.collection.objects.link(obj)
        return obj


def activate(obj):
    bpy.ops.object.select_all(action='DESELECT')
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj


def skeleton(verts, edges, radii, target_tris):
    """Continuous wood surface: Skin modifier, one subdivision, decimate."""
    me = bpy.data.meshes.new('Wood')
    me.from_pydata(verts, edges, [])
    obj = bpy.data.objects.new('Wood', me)
    bpy.context.collection.objects.link(obj)
    activate(obj)
    bpy.ops.object.modifier_add(type='SKIN')
    skin = obj.modifiers[-1]
    skin.branch_smoothing = .55
    for i, r in enumerate(radii):
        me.skin_vertices[0].data[i].radius = (r, r)
    me.skin_vertices[0].data[0].use_root = True
    obj.modifiers.new('Smooth', 'SUBSURF').levels = 1
    for mod in list(obj.modifiers):
        bpy.ops.object.modifier_apply(modifier=mod.name)
    tris = sum(len(p.vertices) - 2 for p in me.polygons)
    if tris > target_tris:
        dec = obj.modifiers.new('Budget', 'DECIMATE')
        dec.ratio = target_tris / tris
        bpy.ops.object.modifier_apply(modifier=dec.name)
    return obj


def grow(rng, levels, start, direction, length, radius, verts, edges, radii, tips, parent):
    """Recursive limbs. Every child edge starts at its parent's vertex, so the
    Skin modifier builds one continuous flared surface."""
    prev, point = parent, start
    for s in range(2):
        bend = Vector((rng.uniform(-.15, .15), rng.uniform(-.15, .15), rng.uniform(0, .1)))
        direction = (direction + bend).normalized()
        point = point + direction * (length / 2)
        verts.append(tuple(point)); radii.append(radius * (1 - .25 * (s + 1)))
        edges.append((prev, len(verts) - 1)); prev = len(verts) - 1
    if not levels:
        tips.append((point, direction)); return
    count, spread, scale = levels[0]
    az = rng.uniform(0, math.tau)
    for k in range(count):
        az += math.tau / count + rng.uniform(-.4, .4)
        out = Vector((math.cos(az), math.sin(az), 0))
        child = (direction * math.cos(spread) + out * math.sin(spread)).normalized()
        child.z = max(child.z, .1)
        grow(rng, levels[1:], point, child, length * scale, radius * .65, verts, edges, radii, tips, prev)


def roots(rng, verts, edges, radii, count, reach, radius):
    for k in range(count):
        a = k * math.tau / count + rng.uniform(-.3, .3)
        verts.append((math.cos(a) * reach, math.sin(a) * reach, -.05)); radii.append(radius); edges.append((0, len(verts) - 1))


def lobe(parts, rng, center, radii, color, jitter=.08, segments=(7, 4)):
    """Angular faceted canopy mass, as the board's 'Dense angular canopy'."""
    bm = bmesh.new()
    bmesh.ops.create_uvsphere(bm, u_segments=segments[0], v_segments=segments[1], radius=1.0)
    verts = []
    for v in bm.verts:
        p = Vector((v.co.x * radii.x, v.co.y * radii.y, v.co.z * radii.z))
        verts.append(center + p + Vector((rng.uniform(-jitter, jitter), rng.uniform(-jitter, jitter), rng.uniform(-jitter, jitter))))
    faces = [tuple(v.index for v in f.verts) for f in bm.faces]
    bm.free()
    parts.add(verts, faces, color)


def spear(parts, base, up, length, half, roll, color):
    """The board's 'Pointed hedge leaf': a closed five-face spear."""
    up = up.normalized()
    ref = Vector((0, 0, 1)) if abs(up.z) < .9 else Vector((1, 0, 0))
    side = up.cross(ref).normalized()
    side = side * math.cos(roll) + up.cross(side) * math.sin(roll)
    depth = up.cross(side).normalized()
    verts = [base - side * half, base + side * half, base + up * length,
             base + up * length * .3 - depth * .1, base + up * length * .2 + depth * .035]
    parts.add(verts, [(0, 3, 2), (3, 1, 2), (0, 2, 4), (4, 2, 1), (0, 4, 1, 3)], color)
    parts.leaves += 1


def blade(parts, root, up, length, half, color):
    """A thin two-sided tapered blade (grass, reeds, moss strands)."""
    up = up.normalized()
    side = up.cross(Vector((0, 0, 1)) if abs(up.z) < .9 else Vector((1, 0, 0))).normalized()
    parts.add([root - side * half, root + side * half, root + up * length], [(0, 1, 2)], color, two_sided=True)
    parts.leaves += 1


def crown(parts, rng, groups, palette_lobe, palette_leaf, lobe_size, leaves_per_lobe, leaf_length):
    """Unequal faceted lobes at limb-end groups, gaps between them, and spear
    leaves splayed outward from each lobe's surface."""
    lobes = []
    for pts in groups:
        c = sum(pts, Vector()) / len(pts)
        spread = max((p - c).length for p in pts)
        r = max(lobe_size[0], min(lobe_size[1], spread * .8 + lobe_size[0] * .6)) * rng.uniform(.85, 1.15)
        lobes.append((c + Vector((0, 0, r * .25)), Vector((r * rng.uniform(.9, 1.15), r * rng.uniform(.9, 1.15), r * rng.uniform(.65, .8)))))
    pts = [p for g in groups for p in g]
    middle = sum(pts, Vector()) / len(pts)
    reach = max((p - middle).length for p in pts) + 1e-3
    for c, r in lobes:
        # Interior lobes sit in shade, like the board hedges' dark cores.
        value = .8 + .25 * min(1.0, (c - middle).length / reach)
        lobe(parts, rng, c, r, linear(rng.choice(palette_lobe), value))
        for _ in range(leaves_per_lobe):
            u = Vector((rng.gauss(0, 1), rng.gauss(0, 1), abs(rng.gauss(0, 1)) * .8 + .2)).normalized()
            base = c + Vector((u.x * r.x, u.y * r.y, u.z * r.z)) * .78
            out = (u + Vector((0, 0, .35))).normalized()
            spear(parts, base, out, rng.uniform(*leaf_length), rng.uniform(.06, .1), rng.uniform(0, math.tau),
                  linear(rng.choice(palette_leaf), rng.uniform(.9, 1.1)))
    return lobes


def broadleaf(seed):
    rng = random.Random(seed)
    parts = Parts()
    verts, edges, radii, tips = [(0, 0, 0)], [], [.15], []
    roots(rng, verts, edges, radii, 4, .26, .055)
    verts.append((0, 0, .55)); radii.append(.12); edges.append((0, len(verts) - 1))
    top = Vector((rng.uniform(-.08, .08), rng.uniform(-.08, .08), 1.05))
    verts.append(tuple(top)); radii.append(.1); edges.append((len(verts) - 2, len(verts) - 1))
    top_index = len(verts) - 1
    groups = []
    for k in range(4):
        az = k * math.tau / 4 + rng.uniform(-.35, .35)
        el = rng.uniform(.6, .85) if k else .15
        d = Vector((math.cos(az) * math.sin(el), math.sin(az) * math.sin(el), math.cos(el)))
        first = len(tips)
        grow(rng, [(2, .55, .65)], top, d, rng.uniform(.55, .75), .075, verts, edges, radii, tips, top_index)
        groups.append([p for p, _ in tips[first:]])
    parts.add_object(skeleton(verts, edges, radii, 300), linear(WOOD))
    crown(parts, rng, groups, CANOPY, LEAF, (.58, .8), 12, (.38, .55))
    # A smaller top lobe keeps the crown silhouette from reading as a dome.
    peak = max((p for g in groups for p in g), key=lambda p: p.z)
    lobe(parts, rng, peak + Vector((0, 0, .3)), Vector((.36, .36, .3)), linear(rng.choice(CANOPY), 1.05))
    return parts


def pine(seed):
    rng = random.Random(seed)
    parts = Parts()
    height = rng.uniform(2.6, 3.0)
    verts, edges, radii = [(0, 0, 0)], [], [.13]
    roots(rng, verts, edges, radii, 3, .2, .045)
    prev = 0
    for z in (.5, 1.2, 1.9, height - .2):
        verts.append((rng.uniform(-.03, .03), rng.uniform(-.03, .03), z)); radii.append(.11 * (1 - .75 * z / height) + .02)
        edges.append((prev, len(verts) - 1)); prev = len(verts) - 1
    parts.add_object(skeleton(verts, edges, radii, 260), linear(WOOD_DARK))
    tiers = 5
    for i in range(tiers):
        t = i / (tiers - 1)
        base_z = .55 + t * (height - 1.15)
        radius = .82 * (1 - .72 * t)
        tier_h = .85 - .25 * t
        # A jagged faceted cone skirt: drooping rim points around one apex.
        sides = 7
        apex = Vector((0, 0, base_z + tier_h))
        rim = []
        for k in range(sides):
            a = k * math.tau / sides + rng.uniform(-.15, .15) + i * .5
            r = radius * rng.uniform(.85, 1.1)
            rim.append(Vector((math.cos(a) * r, math.sin(a) * r, base_z + rng.uniform(-.08, .04))))
        inner = Vector((0, 0, base_z + .12))
        verts_t = [apex, inner] + rim
        faces = [(0, 2 + k, 2 + (k + 1) % sides) for k in range(sides)] + [(1, 2 + (k + 1) % sides, 2 + k) for k in range(sides)]
        parts.add(verts_t, faces, linear(rng.choice(NEEDLE), .9 + .15 * t))
        for p in rim:
            # Needle spears break each skirt's rim into a jagged silhouette.
            out = Vector((p.x, p.y, 0)).normalized() + Vector((0, 0, -.35))
            spear(parts, p.lerp(apex, .15), out, rng.uniform(.22, .34), .06, rng.uniform(0, math.tau), linear(rng.choice(NEEDLE_TIP)))
    spear(parts, Vector((0, 0, height - .1)), Vector((0, 0, 1)), .35, .07, 0, linear(NEEDLE_TIP[0]))
    return parts


def shrub(seed, lobes_count=3):
    rng = random.Random(seed)
    parts = Parts()
    groups = []
    for k in range(lobes_count):
        a = k * math.tau / lobes_count + rng.uniform(-.4, .4)
        r = rng.uniform(.15, .3)
        groups.append([Vector((math.cos(a) * r, math.sin(a) * r, rng.uniform(.25, .4)))])
    crown(parts, rng, groups, CANOPY, LEAF, (.3, .4), 7, (.28, .4))
    return parts


def edge_hedge(seed, length):
    """Dense impassable hedge at a board edge, with the same proportions as the
    board's home-row 'Dense impassable hedge': a dark core 0.48 tall, big
    angular canopy masses every 0.84 tile (0.49 x 0.47 x 0.40 radii, two per
    1.68-tile block) and about 19 pointed spear leaves per tile. Local +X
    points away from the lanes; nothing may cross X < 0 (the clip line)."""
    rng = random.Random(seed)
    parts = Parts()
    w = 1.0
    bm = bmesh.new()
    bmesh.ops.create_cube(bm, size=1.0)
    core = [Vector((w / 2 + v.co.x * (w - .06), v.co.y * length, .24 + v.co.z * .48)) for v in bm.verts]
    faces = [tuple(v.index for v in f.verts) for f in bm.faces]
    bm.free()
    parts.add(core, faces, linear(HEDGE_DARK))
    balls = max(1, round(length / .84))
    for k in range(balls):
        y = -length / 2 + (k + .5) * length / balls
        # Radius plus jitter stays behind the clip line (x >= .02).
        lobe(parts, rng, Vector((.52, y, .51)), Vector((.47, min(.49, length / balls * .58), .40)), linear(HEDGE), .02)
    for i in range(int(length * 19)):
        lx = rng.uniform(.1, w - .08); ly = rng.uniform(-length / 2 + .06, length / 2 - .06)
        tip = Vector((max(.04, lx + rng.uniform(-.24, .24)), ly + rng.uniform(-.1, .12), rng.uniform(.72, 1.17)))
        half = min(rng.uniform(.09, .16), lx - .02)
        verts = [(lx - half, ly, .26), (lx + half, ly, .26), tuple(tip), (lx, ly - .1, .5 + (tip.z - .72) * .35), (lx, ly + .035, .42)]
        parts.add(verts, [(0, 3, 2), (3, 1, 2), (0, 2, 4), (4, 2, 1), (0, 4, 1, 3)], linear(HEDGE_LIGHT if i % 4 == 0 else HEDGE))
        parts.leaves += 1
    return parts

def swamp_cypress(seed):
    """Bald cypress: a buttressed trunk flaring into its knees, near-horizontal
    limbs with flat faceted needle lobes, and hanging moss strands."""
    rng = random.Random(seed)
    parts = Parts()
    height = rng.uniform(2.6, 3.0)
    verts, edges, radii, tips = [(0, 0, .05)], [], [.22], []
    for k in range(6):
        a = k * math.tau / 6 + rng.uniform(-.25, .25)
        verts.append((math.cos(a) * .3, math.sin(a) * .3, -.02)); radii.append(.09); edges.append((0, len(verts) - 1))
        verts.append((math.cos(a) * .55, math.sin(a) * .55, -.12)); radii.append(.04); edges.append((len(verts) - 2, len(verts) - 1))
    prev = 0
    nodes = []
    for z in (.5, 1.1, 1.7, height):
        verts.append((rng.uniform(-.05, .05), rng.uniform(-.05, .05), z)); radii.append(.16 - .1 * z / height + .02)
        edges.append((prev, len(verts) - 1)); prev = len(verts) - 1; nodes.append(prev)
    groups = []
    for node in nodes[1:]:
        for k in range(2):
            az = rng.uniform(0, math.tau)
            d = Vector((math.cos(az), math.sin(az), rng.uniform(.05, .25))).normalized()
            first = len(tips)
            grow(rng, [], Vector(verts[node]), d, rng.uniform(.6, .85), .05, verts, edges, radii, tips, node)
            groups.append([p for p, _ in tips[first:]])
    parts.add_object(skeleton(verts, edges, radii, 450), linear(WOOD_DARK))
    lobes = crown(parts, rng, groups, NEEDLE, NEEDLE_TIP, (.46, .6), 10, (.28, .4))
    for c, r in lobes:
        # Moss hangs straight down in thin two-sided strands from each lobe.
        for _ in range(rng.randint(3, 5)):
            a = rng.uniform(0, math.tau)
            root = c + Vector((math.cos(a) * r.x * .7, math.sin(a) * r.y * .7, -r.z * .5))
            blade(parts, root, Vector((rng.uniform(-.1, .1), rng.uniform(-.1, .1), -1)), rng.uniform(.5, .85), .045,
                  linear(rng.choice(MOSS)))
    return parts


def tuft(seed, kind):
    rng = random.Random(seed)
    parts = Parts()
    blades = {'grass': 7, 'flowers': 5, 'reeds': 10}[kind]
    for k in range(blades):
        root = Vector((rng.uniform(-.14, .14), rng.uniform(-.14, .14), -.02))
        up = Vector((rng.uniform(-.35, .35), rng.uniform(-.35, .35), 1))
        length = rng.uniform(.75, 1.1) if kind == 'reeds' else rng.uniform(.24, .4)
        blade(parts, root, up, length, .05 if kind == 'reeds' else .045, linear(rng.choice(GRASS), rng.uniform(.9, 1.1)))
    if kind == 'flowers':
        for k in range(rng.randint(2, 3)):
            root = Vector((rng.uniform(-.12, .12), rng.uniform(-.12, .12), 0))
            head = root + Vector((rng.uniform(-.06, .06), rng.uniform(-.06, .06), rng.uniform(.3, .42)))
            blade(parts, root, head - root, (head - root).length, .02, linear(GRASS[0]))
            petal = rng.choice(PETALS)
            for j in range(5):
                a = j * math.tau / 5
                lobe(parts, rng, head + Vector((math.cos(a) * .055, math.sin(a) * .055, 0)), Vector((.045, .045, .02)), linear(petal), .005, (4, 2))
            lobe(parts, rng, head + Vector((0, 0, .012)), Vector((.03, .03, .02)), linear('ff7a16' if petal != 'ff7a16' else 'ffdd00'), .004, (4, 2))
    if kind == 'reeds':
        for k in range(rng.randint(3, 4)):
            root = Vector((rng.uniform(-.12, .12), rng.uniform(-.12, .12), -.02))
            top = root + Vector((rng.uniform(-.05, .05), rng.uniform(-.05, .05), rng.uniform(.75, 1.0)))
            blade(parts, root, top - root, (top - root).length, .018, linear(GRASS[0], .8))
            # A low-poly cattail head on each stalk.
            bm = bmesh.new()
            bmesh.ops.create_cone(bm, cap_ends=True, segments=6, radius1=.045, radius2=.045, depth=.18)
            verts = [top - Vector((0, 0, .14)) + v.co for v in bm.verts]
            faces = [tuple(v.index for v in f.verts) for f in bm.faces]
            bm.free()
            parts.add(verts, faces, linear(WOOD_DARK, 1.2))
    return parts


def reed_wall(seed, length):
    """The river's border: an ordered wall of reeds and cattails standing in
    the water along the clip line, as clear a boundary as the edge hedges.
    Local +X points away from the river lanes; nothing may cross X < 0."""
    rng = random.Random(seed)
    parts = Parts()
    # Six staggered rows of broad, leaning blades: a wall that reads as solid
    # from the overhead cameras as well as from the side.
    rows = [(.05, .07, (.8, .95)), (.22, .07, (.9, 1.08)), (.4, .07, (1.0, 1.18)), (.58, .07, (1.0, 1.2)), (.76, .07, (.95, 1.12)), (.94, .08, (.85, 1.0))]
    for row, (x, spacing, heights) in enumerate(rows):
        y = -length / 2 + spacing * (.5 if row % 2 else 0)
        while y < length / 2:
            # Regular staggered rows with only slight jitter: a deliberate wall.
            root = Vector((x + rng.uniform(-.02, .02), y + rng.uniform(-.012, .012), -.05))
            up = Vector((rng.uniform(.1, .35), rng.uniform(-.18, .18), 1))
            blade(parts, root, up, rng.uniform(*heights), .075, linear(GRASS[row % len(GRASS)], rng.uniform(.88, 1.08)))
            y += spacing
    # Cattails stand above the blades in the back rows.
    y = -length / 2 + .12
    while y < length / 2 - .06:
        root = Vector((rng.uniform(.25, .95), y, -.05))
        top = root + Vector((rng.uniform(0, .05), rng.uniform(-.03, .03), rng.uniform(1.1, 1.3)))
        blade(parts, root, top - root, (top - root).length, .018, linear(GRASS[0], .8))
        bm = bmesh.new()
        bmesh.ops.create_cone(bm, cap_ends=True, segments=6, radius1=.045, radius2=.045, depth=.18)
        verts = [top - Vector((0, 0, .14)) + v.co for v in bm.verts]
        faces = [tuple(v.index for v in f.verts) for f in bm.faces]
        bm.free()
        parts.add(verts, faces, linear(WOOD_DARK, 1.2))
        y += rng.uniform(.14, .2)
    # A dark rush core at the water line keeps the wall from showing through.
    bm = bmesh.new()
    bmesh.ops.create_cube(bm, size=1.0)
    core = [Vector((.52 + v.co.x * .9, v.co.y * length, .18 + v.co.z * .36)) for v in bm.verts]
    faces = [tuple(v.index for v in f.verts) for f in bm.faces]
    bm.free()
    parts.add(core, faces, linear(HEDGE_DARK))
    return parts


def lily_pads(seed):
    """Low-poly notched pads floating at local Z = 0, one with a bloom."""
    rng = random.Random(seed)
    parts = Parts()
    for k in range(3):
        c = Vector((rng.uniform(-.35, .35), rng.uniform(-.35, .35), .006 * k))
        r = rng.uniform(.22, .34)
        notch = rng.uniform(0, math.tau)
        rim = [c + Vector((math.cos(notch + .35 + j * (math.tau - .7) / 9) * r, math.sin(notch + .35 + j * (math.tau - .7) / 9) * r, 0)) for j in range(10)]
        parts.add([c] + rim, [(0, j, j + 1) for j in range(1, 10)], linear(rng.choice(['2f7d14', '3f8f17', '4f9e1a'])), two_sided=True)
        if k == 0:
            for j in range(6):
                a = j * math.tau / 6
                lobe(parts, rng, c + Vector((math.cos(a) * .07, math.sin(a) * .07, .04)), Vector((.06, .06, .035)), linear('ff9acb'), .005, (4, 2))
            lobe(parts, rng, c + Vector((0, 0, .06)), Vector((.04, .04, .03)), linear('ff269c'), .004, (4, 2))
    return parts


def bounds(obj):
    pts = [obj.matrix_world @ v.co for v in obj.data.vertices]
    return {k: round(f(getattr(p, a) for p in pts), 5) for k, f, a in
            [('minX', min, 'x'), ('maxX', max, 'x'), ('minY', min, 'y'), ('maxY', max, 'y'), ('minZ', min, 'z'), ('maxZ', max, 'z')]}


def render_views(name, obj):
    scene = bpy.context.scene
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x = scene.render.resolution_y = 360
    scene.view_settings.view_transform = 'Standard'
    if scene.world is None: scene.world = bpy.data.worlds.new('World')
    scene.world.color = (.42, .62, .85)
    bpy.ops.mesh.primitive_plane_add(size=60, location=(0, 0, -.02))
    ground = bpy.context.object
    gm = bpy.data.materials.new('Review ground'); gm.use_nodes = True
    gm.node_tree.nodes.get('Principled BSDF').inputs['Base Color'].default_value = (*linear('1c501c'), 1)
    ground.data.materials.append(gm)
    bpy.ops.object.light_add(type='SUN', location=(0, 0, 5))
    sun = bpy.context.object; sun.data.energy = 3.2; sun.rotation_euler = (math.radians(50), 0, math.radians(-35))
    b = bounds(obj)
    size = max(b['maxX'] - b['minX'], b['maxY'] - b['minY'], b['maxZ'] - b['minZ']) * 1.25
    target = Vector(((b['minX'] + b['maxX']) / 2, (b['minY'] + b['maxY']) / 2, (b['minZ'] + b['maxZ']) / 2))
    bpy.ops.object.camera_add(); cam = bpy.context.object; scene.camera = cam
    cam.data.type = 'ORTHO'; cam.data.ortho_scale = size
    for view, offset in [('hero', (3, -4, 3)), ('front', (0, -5, .6)), ('side', (5, 0, .6)), ('game', (0, -2.2, 5))]:
        cam.location = target + Vector(offset)
        cam.rotation_euler = (target - cam.location).to_track_quat('-Z', 'Y').to_euler()
        scene.render.filepath = str(REVIEW / f'scenery-{name}-{view}.png')
        bpy.ops.render.render(write_still=True)
    for o in (ground, sun, cam): bpy.data.objects.remove(o, do_unlink=True)


ASSETS = [
    ('broadleaf_a', lambda: broadleaf(11)),
    ('broadleaf_b', lambda: broadleaf(23)),
    ('pine_a', lambda: pine(5)),
    ('pine_b', lambda: pine(17)),
    ('shrub_a', lambda: shrub(3)),
    ('shrub_b', lambda: shrub(29, 4)),
    ('edge_hedge_long', lambda: edge_hedge(41, 2.34)),
    ('edge_hedge_short', lambda: edge_hedge(43, .92)),
    ('edge_hedge_front', lambda: edge_hedge(71, 5.88)),
    ('grass_tuft', lambda: tuft(7, 'grass')),
    ('flower_tuft', lambda: tuft(13, 'flowers')),
    ('reeds_a', lambda: tuft(31, 'reeds')),
    ('reeds_b', lambda: tuft(37, 'reeds')),
    ('reed_wall', lambda: reed_wall(61, 5.3)),
    ('lily_pads', lambda: lily_pads(47)),
    ('swamp_cypress_a', lambda: swamp_cypress(53)),
    ('swamp_cypress_b', lambda: swamp_cypress(59)),
]

records = []
selected = set(filter(None, os.environ.get('FROGGER_BUILD_SCENERY', '').split(',')))
for name, builder in ASSETS:
    if selected and name not in selected: continue
    reset()
    parts = builder()
    obj = parts.build(name)
    footprint = bounds(obj)
    if name.startswith('edge_hedge') or name == 'reed_wall':
        assert footprint['minX'] >= -1e-4, (name, 'hedge crosses the board clip line', footprint)
    bpy.ops.wm.save_as_mainfile(filepath=str(SOURCE / f'scenery_{name}.blend'))
    bpy.ops.export_scene.gltf(filepath=str(OUT / f'{name}.gltf'), export_format='GLTF_SEPARATE',
                              export_vertex_color='MATERIAL', export_normals=True, export_yup=True, export_apply=True,
                              export_animations=False)
    # Record what ships: the exporter merges vertex-identical corners and drops
    # any triangle that collapses, so count the exported index accessors.
    exported = json.loads((OUT / f'{name}.gltf').read_text())
    triangles = sum(exported['accessors'][p['indices']]['count'] // 3 for m in exported['meshes'] for p in m['primitives'])
    assert triangles <= BUDGET, (name, triangles)
    render_views(name, obj)
    records.append({'asset': name, 'triangles': triangles, 'leaves': parts.leaves, 'gltf': f'godot/Models/Scenery/{name}.gltf',
                    'source': f'art/source/scenery_{name}.blend', 'footprintTiles': footprint})
manifest = ROOT / 'art/scenery.json'
if selected and manifest.exists():
    previous = {r['asset']: r for r in json.loads(manifest.read_text())}
    previous.update({r['asset']: r for r in records})
    records = [previous[name] for name, _ in ASSETS if name in previous]
manifest.write_text(json.dumps(records, indent=2) + '\n')
print('FROGGER_SCENERY_COMPLETE', len(records))
