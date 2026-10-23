class_name Scenery
extends Node3D

## Backdrop around the board. Nothing here is gameplay: it never enters the
## lanes, never shades them and never covers the start bank in any camera.
## Lanes leave the board through the world instead of a frame: road traffic
## into tunnel mouths fading to black, the river through a wall of reeds into
## and out of a misty swamp, and the grass strips into dense hedges. The terrain is one smooth
## heightfield: a hill over each tunnel slopes down across the median into the
## sunken swamp basin, and rises again at the swamp's banks.
##
## Vegetation is authored in Blender by art/scripts/build_scenery.py in the
## board's own detail level: continuous faceted wood skeletons, angular canopy
## lobes and spear leaves like the home hedges, flat palette colors.
## Each asset draws through one or two MultiMeshes (shadow-casting or not).

const GroundHeight: float = -0.02
const WaterLevel: float = -0.025
const BoardBack: float = -7.66
const BoardFront: float = 7.9
const Seed: int = 1981

## Road tunnel and grass-strip spans along the board edge (Godot z).
const RoadSpan: Vector2 = Vector2(0.38, 5.62)
const MedianSpan: Vector2 = Vector2(-0.46, 0.46)
const BottomSpan: Vector2 = Vector2(5.56, 7.9)
const RiverSpan: Vector2 = Vector2(-5.5, -0.5)
## The bottom hedge wall: flush against the start bank's grass edge (z 7.5),
## 1.0 tile deep like the home-row hedges.
const FrontHedgeFront: float = 7.48
const FrontHedgeBack: float = 8.48
## The grove behind the home row's left half (x, z).
const BackGrove: Rect2 = Rect2(-9.5, -15.0, 9.0, 6.2)


## The hill's extent along the board edge. Its flanks broaden as it climbs
## away from the board (d tiles out), so it reads as a hillside, not a wall.
static func hill_weight(z: float, d: float) -> float:
	# Past the tunnel interior (d > 2.5) the hill's foot steps back from the
	# river channel, leaving room for the upper bank's reed wall in the shallows.
	var foot: float = 0.35 * maxf(d - 2.5, 0.0)
	return smoothstep(-0.6 + foot, 0.7 + foot, z) * (1.0 - smoothstep(5.2, 8.6 + 0.5 * d, z))

## The river runs on past both board edges as a channel of exactly its board
## width (RiverSpan), sunk below the banks, until it fades into the distance.
static func swamp_weight(x: float, z: float) -> float:
	var d: float = absf(x) - 7.0
	return smoothstep(RiverSpan.x - 0.7, RiverSpan.x, z) * (1.0 - smoothstep(RiverSpan.y, RiverSpan.y + 0.7, z)) * (1.0 - smoothstep(14.0, 17.0, d))

## 1 along the road tunnel, where the hill must stand over the portal from the
## board edge outward; 0 along the grass strips, where it ramps up behind the
## hedges.
static func road_band(z: float) -> float:
	return smoothstep(0.1, 0.5, z) * (1.0 - smoothstep(5.5, 5.9, z))

## Behind the bottom hedge wall the ground climbs away from the board. Its
## slope stays under the lowest camera sightline to the start row (the
## orthographic follow camera, about 1.5 rise per tile).
static func front_rise(x: float, z: float) -> float:
	# Gentle rolling hills climb away from the board at both ends: one soft
	# rise, with broad swells and dips that grow with distance.
	var t: float = maxf(BoardBack - 0.25 - z, 0.0) if z < 0.0 else maxf(z - FrontHedgeBack, 0.0)
	var rise: float = 1.5 * (1.0 - exp(-t / 2.4))
	var swell: float = 0.55 * sin(x * 0.42 + z * 0.18 + 0.7) * sin(z * 0.38 - x * 0.12 + 1.9)
	swell += 0.25 * sin(x * 0.85 - 1.1) * sin(z * 0.75 + 0.4)
	return GroundHeight + rise + swell * smoothstep(0.4, 3.5, t)

static func terrain_height(x: float, z: float) -> float:
	var d: float = absf(x) - 7.0
	if d < 0.0:
		return maxf(GroundHeight, front_rise(x, z)) if (z > BoardFront or z < BoardBack) else GroundHeight
	# Over each tunnel the hill keeps climbing away from the board, so the
	# tunnel always runs on under rising ground, with a gentle undulation so
	# no slope reads as a flat slab.
	var hill: float = 1.4 + 2.8 * (1.0 - exp(-d / 5.0))
	hill += 0.16 * sin(x * 0.9 + z * 0.6) * sin(z * 0.7 - x * 0.4) * smoothstep(0.6, 2.0, d)
	hill *= lerpf(smoothstep(0.0, 0.5, d), 1.0, road_band(z))
	var side: float = GroundHeight + hill_weight(z, d) * hill - swamp_weight(x, z) * 0.34
	# The end hills apply only above and below the board, never over the
	# river channel, whose floor must stay under the water.
	return maxf(side, front_rise(x, z)) if (z > BoardFront or z < BoardBack) else side

## The tallest prop allowed at a point, or 0 where nothing may stand.
static func height_limit(x: float, z: float) -> float:
	var d: float = absf(x) - 7.0
	if d < 0.05 and z > BoardBack - 0.3 and z < BoardFront + 0.2:
		return 0.0
	# The bottom hedge wall owns its strip. Behind it a prop's top must stay
	# under the lowest camera sightline to the start row, and the portrait
	# follow camera passes low over the near slope, so only low growth stands.
	if z >= BoardFront and absf(x) < 13.0:
		if z < FrontHedgeBack + 0.15:
			return 0.0
		var limit: float = 1.4 * (z - 7.6) - terrain_height(x, z)
		return maxf(0.0, minf(limit, 2.0 if z < 13.0 else INF))
	return INF

## Vegetation is untextured and flat-faceted like every board model: COLOR_0
## carries each part's palette color and every renderer multiplies in the
## MultiMesh instance color (a small value variation). Custom data alpha holds
## a breeze phase. The Compatibility (web) renderer also multiplies a zero
## instance color when a MultiMesh declares none, so instance colors are always
## set. Thin blades and moss
## strands are single faces, so both sides draw.
const VegetationShader: String = """shader_type spatial;
render_mode cull_disabled;
#include "res://Shaders/curve.gdshaderinc"
void vertex(){
    // A gentle breeze that grows with height, so wood and leaves stay joined.
    float phase = INSTANCE_CUSTOM.a * 6.2831;
    float bend = max(VERTEX.y - 0.3, 0.0);
    bend *= bend * 0.3;
    VERTEX.x += sin(TIME * 1.2 + phase) * 0.03 * bend;
    VERTEX.z += cos(TIME * 0.9 + phase) * 0.02 * bend;
    VERTEX += curve_offset(MODEL_MATRIX, VERTEX);
}
void fragment(){
    vec3 color = COLOR.rgb;
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
    // The Compatibility (web) renderer decodes vertex colors as sRGB a second
    // time; re-encode the authored linear palette so both renderers match.
    color = pow(max(color, vec3(0.0)), vec3(1.0 / 2.2));
#endif
    ALBEDO = color;
    ROUGHNESS = 0.72;
}"""
const GroundShader: String = """shader_type spatial;
#include "res://Shaders/curve.gdshaderinc"
uniform vec4 grass_dark : source_color = vec4(0.071, 0.224, 0.075, 1.0);
uniform vec4 grass_light : source_color = vec4(0.169, 0.424, 0.125, 1.0);
uniform vec4 mud : source_color = vec4(0.22, 0.19, 0.09, 1.0);
// The board's river bed color: swamp water keeps the river's own blue.
uniform vec4 bed : source_color = vec4(0.035, 0.11, 0.376, 1.0);
varying vec3 world;
varying vec3 world_normal;
void vertex(){
    world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
    world_normal = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
    VERTEX += curve_offset(MODEL_MATRIX, VERTEX);
}
float hash(vec2 p){ return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453); }
float noise(vec2 p){
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i), hash(i + vec2(1.0, 0.0)), f.x), mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), f.x), f.y);
}
void fragment(){
    // Unpatterned grass in the board's palette, so it never reads as a lane.
    float n = noise(world.xz * 0.16) * 0.65 + noise(world.xz * 0.6) * 0.35;
    vec3 color = mix(grass_dark.rgb, grass_light.rgb, smoothstep(0.15, 0.9, n));
    // Steep banks show darker earth through the turf.
    float slope = 1.0 - clamp(world_normal.y, 0.0, 1.0);
    color = mix(color, mix(color, mud.rgb, 0.55), smoothstep(0.18, 0.5, slope));
    // COLOR.r marks the swamp basin: wet mud along the water line, and the
    // river bed's blue under the water so the swamp keeps the river's color.
    float wet = COLOR.r * (1.0 - smoothstep(0.0, 0.1, world.y)) * 0.5;
    color = mix(color, mud.rgb * (0.8 + 0.3 * n), wet);
    float under = COLOR.r * (1.0 - smoothstep(-0.05, -0.01, world.y));
    color = mix(color, bed.rgb, under);
    float cloud = noise(world.xz * 0.06 + vec2(TIME * 0.025, TIME * 0.015));
    color *= 1.0 - 0.18 * smoothstep(0.55, 0.85, cloud);
    ALBEDO = color;
    ROUGHNESS = mix(0.72, 0.35, wet);
}"""

## Soft drifting veils: low swamp mist lying over the water at the river
## mouths (horizontal sheets read as fog from every overhead camera), and
## vertical black layers that fade each road tunnel into darkness. Each set of
## layers is one mesh; COLOR.a carries each layer's density.
const VeilShader: String = """shader_type spatial;
render_mode unshaded, cull_disabled, depth_draw_never, blend_mix, fog_disabled;
#include "res://Shaders/curve.gdshaderinc"
uniform vec4 tint : source_color = vec4(1.0);
uniform bool mist = false;
varying vec3 world;
void vertex(){
    world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
    VERTEX += curve_offset(MODEL_MATRIX, VERTEX);
}
float hash(vec2 p){ return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453); }
float noise(vec2 p){
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i), hash(i + vec2(1.0, 0.0)), f.x), mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), f.x), f.y);
}
void fragment(){
    ALBEDO = tint.rgb;
    if (!mist) {
        // Tunnel layers stay uniform: the portal itself frames them.
        ALPHA = COLOR.a;
    } else {
        vec2 q = world.xz;
        float wisps = noise(q * 0.9 + vec2(TIME * 0.10, -TIME * 0.04)) * 0.6
            + noise(q * 2.3 + vec2(-TIME * 0.16, TIME * 0.06)) * 0.4;
        float edge = 1.0 - smoothstep(0.45, 1.0, length((UV - 0.5) * 2.0));
        ALPHA = COLOR.a * edge * (0.3 + 0.9 * smoothstep(0.15, 0.85, wisps));
    }
}"""
const Variants: Dictionary = {
	"tree": ["broadleaf_a", "broadleaf_b"],
	"pine": ["pine_a", "pine_b"],
	"shrub": ["shrub_a", "shrub_b"],
	"cypress": ["swamp_cypress_a", "swamp_cypress_b"],
	"reeds": ["reeds_a", "reeds_b"],
	"grass": ["grass_tuft"],
	"flowers": ["flower_tuft"],
	"lily": ["lily_pads"],
}

## Set by the game before this node enters the tree: the river's own material,
## so the swamp water continues the river surface, color and all, without a seam.
var water_material: ShaderMaterial = null
var vegetation_material: ShaderMaterial = null
var meshes: Dictionary = {}
var rng := RandomNumberGenerator.new()
var clouds: Node3D = null
var cloud_material: ShaderMaterial = null

func _ready() -> void:
	rng.seed = Seed
	vegetation_material = ShaderMaterial.new()
	vegetation_material.shader = Shader.new()
	vegetation_material.shader.code = VegetationShader
	add_child(build_terrain())
	if water_material != null:
		for side in [-1.0, 1.0]:
			add_child(build_swamp_water(side))
	for side in [-1.0, 1.0]:
		add_veils(side)
	var placements: Dictionary = scatter()
	for key in placements:
		var parts: PackedStringArray = key.split(":")
		var node := make_multimesh(asset_mesh(parts[0]), placements[key])
		node.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_ON if parts[1] == "shadow" else GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
		add_child(node)
	clouds = build_clouds()
	add_child(clouds)

## Imported Blender mesh with the runtime vegetation shader. The mesh keeps its
## imported LODs and vertex colors; only its surface material is replaced.
func asset_mesh(asset: String) -> Mesh:
	if meshes.has(asset):
		return meshes[asset]
	var scene: Node = (load("res://Models/Scenery/%s.gltf" % asset) as PackedScene).instantiate()
	var found: Array = scene.find_children("*", "MeshInstance3D", true, false)
	var mesh: Mesh = (found[0] as MeshInstance3D).mesh
	for surface in range(mesh.get_surface_count()):
		mesh.surface_set_material(surface, vegetation_material)
	scene.free()
	meshes[asset] = mesh
	return mesh

static func grid_lines(from: float, to: float, step: float, extra: Array) -> PackedFloat32Array:
	var values: Array = extra.duplicate()
	var v: float = from
	while v <= to + 0.001:
		values.append(v)
		v += step
	values.sort()
	var out := PackedFloat32Array()
	for value in values:
		if out.is_empty() or absf(value - out[out.size() - 1]) > 0.05:
			out.append(value)
	return out

func build_terrain() -> MeshInstance3D:
	# One smooth heightfield around a hole for the board. Grid lines run along
	# the board's own edges, so the hole matches the board with no gap.
	var xs := grid_lines(-30.0, 30.0, 0.5, [-7.0, 7.0])
	var zs := grid_lines(-28.0, 26.0, 0.5, [BoardBack, BoardFront])
	var st := SurfaceTool.new()
	st.begin(Mesh.PRIMITIVE_TRIANGLES)
	var count: int = 0
	for j in range(zs.size()):
		for i in range(xs.size()):
			var x: float = xs[i]
			var z: float = zs[j]
			var e: float = 0.05
			var normal := Vector3(terrain_height(x - e, z) - terrain_height(x + e, z), 2.0 * e,
				terrain_height(x, z - e) - terrain_height(x, z + e)).normalized()
			st.set_normal(normal)
			st.set_color(Color(swamp_weight(x, z), 0.0, 0.0))
			st.add_vertex(Vector3(x, terrain_height(x, z), z))
			count += 1
	for j in range(zs.size() - 1):
		for i in range(xs.size() - 1):
			var cx: float = (xs[i] + xs[i + 1]) * 0.5
			var cz: float = (zs[j] + zs[j + 1]) * 0.5
			if absf(cx) < 7.0 and cz > BoardBack and cz < BoardFront:
				continue
			var a: int = j * xs.size() + i
			var b: int = a + 1
			var c: int = a + 1 + xs.size()
			var d: int = a + xs.size()
			for k in [a, b, c, a, c, d]:
				st.add_index(k)
	# A flat skirt carries the meadow out past the fog in every direction. It
	# is a coarse grid rather than four large quads, so the curved-world bend
	# follows it smoothly instead of hinging on distant corners.
	var inner := Rect2(Vector2(xs[0], zs[0]), Vector2(xs[xs.size() - 1] - xs[0], zs[zs.size() - 1] - zs[0]))
	var sx := grid_lines(-90.0, 90.0, 6.0, [inner.position.x, inner.end.x])
	var sz := grid_lines(-90.0, 90.0, 6.0, [inner.position.y, inner.end.y])
	var base: int = count
	for j in range(sz.size()):
		for i in range(sx.size()):
			st.set_normal(Vector3.UP)
			st.set_color(Color.BLACK)
			st.add_vertex(Vector3(sx[i], GroundHeight, sz[j]))
			count += 1
	for j in range(sz.size() - 1):
		for i in range(sx.size() - 1):
			if inner.has_point(Vector2((sx[i] + sx[i + 1]) * 0.5, (sz[j] + sz[j + 1]) * 0.5)):
				continue
			var a: int = base + j * sx.size() + i
			for k in [a, a + 1, a + 1 + sx.size(), a, a + 1 + sx.size(), a + sx.size()]:
				st.add_index(k)
	var terrain := MeshInstance3D.new()
	terrain.name = "Terrain"
	terrain.mesh = st.commit()
	var material := ShaderMaterial.new()
	material.shader = Shader.new()
	material.shader.code = GroundShader
	terrain.material_override = material
	# The hill may shade the woods but never reaches across the lanes: it
	# rises only past the board edge, and the sun travels toward +x and +z.
	terrain.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	return terrain

func build_swamp_water(side: float) -> MeshInstance3D:
	# Continues the river's surface (same material, same local coordinates)
	# beyond the board edge. Reeds, pads, trees and mist obscure it, not tint.
	var st := SurfaceTool.new()
	st.begin(Mesh.PRIMITIVE_TRIANGLES)
	var origin_z: float = -3.0
	var xs := grid_lines(7.0, 19.0, 0.5, [])
	var zs := grid_lines(RiverSpan.x - 0.6, RiverSpan.y + 0.6, 5.0 / 16.0, [RiverSpan.x, RiverSpan.y])
	for j in range(zs.size() - 1):
		for i in range(xs.size() - 1):
			var quad: Array = [Vector2(xs[i], zs[j]), Vector2(xs[i + 1], zs[j]), Vector2(xs[i + 1], zs[j + 1]), Vector2(xs[i], zs[j + 1])]
			if side < 0.0:
				quad.reverse()
			for k in [0, 1, 2, 0, 2, 3]:
				var p: Vector2 = quad[k]
				st.set_normal(Vector3.UP)
				st.add_vertex(Vector3(p.x * side, 0.0, p.y - origin_z))
	var water := MeshInstance3D.new()
	water.name = "SwampWater"
	water.mesh = st.commit()
	water.position = Vector3(0, WaterLevel, origin_z)
	water.material_override = water_material
	water.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	return water

## One quad per layer, all layers of a set in one mesh: horizontal mist sheets
## over the water, or vertical layers facing along the lanes (across X).
func veil_mesh(layers: Array, tint: Color, mist: bool) -> MeshInstance3D:
	var st := SurfaceTool.new()
	st.begin(Mesh.PRIMITIVE_TRIANGLES)
	for layer in layers:
		var center: Vector3 = layer.center
		var half: Vector2 = layer.size * 0.5
		var corners: Array = []
		for c in [Vector2(-1, -1), Vector2(1, -1), Vector2(1, 1), Vector2(-1, 1)]:
			var offset := Vector3(c.x * half.x, 0.0, c.y * half.y) if mist else Vector3(0.0, c.y * half.y, c.x * half.x)
			corners.append([center + offset, (c + Vector2.ONE) * 0.5])
		for k in [0, 1, 2, 0, 2, 3]:
			st.set_color(Color(1, 1, 1, layer.density))
			st.set_uv(corners[k][1])
			st.add_vertex(corners[k][0])
	var veil := MeshInstance3D.new()
	veil.mesh = st.commit()
	var material := ShaderMaterial.new()
	material.shader = veil_shader()
	material.set_shader_parameter("tint", tint)
	material.set_shader_parameter("mist", mist)
	veil.material_override = material
	veil.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	return veil

var _veil_shader: Shader = null
func veil_shader() -> Shader:
	if _veil_shader == null:
		_veil_shader = Shader.new()
		_veil_shader.code = VeilShader
	return _veil_shader

func add_veils(side: float) -> void:
	# Black layers inside each tunnel mouth deepen into darkness; the first sits
	# just behind the portal face, which hides every layer outside the arch.
	var tunnel: Array = []
	for k in range(6):
		tunnel.append({"center": Vector3(side * (7.03 + 0.17 * float(k)), 0.55, 3.0), "size": Vector2(5.4, 1.3), "density": 0.16 + 0.05 * float(k)})
	add_child(veil_mesh(tunnel, Color.BLACK, false))
	# A faint low mist lies over each river mouth, where logs and turtles fade
	# out, and drifts in thin patches; the water stays clearly blue under it.
	var river_mid: float = (RiverSpan.x + RiverSpan.y) * 0.5
	var mist: Array = []
	for k in range(3):
		mist.append({"center": Vector3(side * (8.2 + 0.2 * float(k)), 0.06 + 0.14 * float(k), river_mid), "size": Vector2(3.0, 5.6), "density": 0.12 - 0.03 * float(k)})
	for k in range(6):
		mist.append({"center": Vector3(side * rng.randf_range(9.0, 15.0), rng.randf_range(0.08, 0.3), rng.randf_range(RiverSpan.x + 1.0, RiverSpan.y - 1.0)),
			"size": Vector2(rng.randf_range(2.5, 4.5), rng.randf_range(2.0, 3.5)), "density": 0.07})
	add_child(veil_mesh(mist, Color("dbe9f7"), true))
## Deterministic jittered-grid scatter by zone, bucketed by asset and by whether
## its shadow can reach the board, so only harmless instances cast shadows.
func scatter() -> Dictionary:
	var out: Dictionary = {}
	var add := func(kind: String, x: float, z: float, scale: float, y_override: float) -> void:
		var variants: Array = Variants[kind]
		var asset: String = variants[rng.randi() % variants.size()]
		var height: float = 3.0 if kind in ["tree", "pine", "cypress"] else 1.2
		# The sun travels toward +x and +z (0.7 and 0.5 per unit of drop): only
		# instances whose shadow cannot reach the board cast one.
		var reach: float = height * scale
		var safe: bool = x > 7.0 or z > BoardFront or x + 0.75 * reach < -7.0 or z + 0.55 * reach < BoardBack
		var casts: bool = safe and kind in ["tree", "pine", "cypress", "shrub"]
		var key: String = "%s:%s" % [asset, "shadow" if casts else "none"]
		if not out.has(key):
			out[key] = []
		var angle: float = rng.randf() * TAU
		var y: float = terrain_height(x, z) if is_nan(y_override) else y_override
		# Small per-instance value and warmth variation, so repeats never match.
		var v: float = rng.randf_range(0.9, 1.1)
		var tint := Color(v * rng.randf_range(0.96, 1.04), v, v * rng.randf_range(0.94, 1.02))
		out[key].append({"transform": Transform3D(Basis(Vector3.UP, angle).scaled(Vector3.ONE * scale), Vector3(x, y - 0.03 * scale, z)),
			"tint": tint, "phase": rng.randf()})
	# Edge hedges: flush with the clip line on both grass strips, both sides.
	for side in [-1.0, 1.0]:
		var yaw: float = 0.0 if side > 0.0 else PI
		for span in [BottomSpan, MedianSpan]:
			var asset: String = "edge_hedge_long" if span == BottomSpan else "edge_hedge_short"
			var key: String = "%s:none" % asset
			if not out.has(key):
				out[key] = []
			out[key].append({"transform": Transform3D(Basis(Vector3.UP, yaw), Vector3(side * 7.0, GroundHeight, (span.x + span.y) * 0.5)),
				"tint": Color.WHITE, "phase": rng.randf()})
	# A solid hedge wall, built like the home-row hedges, closes the bottom of
	# the board behind the start bank. Its shadow falls away from the lanes.
	out["edge_hedge_front:shadow"] = []
	# Segments overlap a little so the wall reads as one continuous hedge.
	for k in range(6):
		out["edge_hedge_front:shadow"].append({"transform": Transform3D(Basis(Vector3.UP, -PI / 2.0), Vector3(-14.4 + 5.76 * float(k), GroundHeight, FrontHedgeFront)),
			"tint": Color.WHITE, "phase": rng.randf()})
	# An ordered wall of reeds closes each river mouth along the clip line,
	# the river's counterpart to the edge hedges.
	out["reed_wall:none"] = [{"transform": Transform3D(Basis.IDENTITY, Vector3(7.0, WaterLevel, (RiverSpan.x + RiverSpan.y) * 0.5)),
		"tint": Color.WHITE, "phase": rng.randf()},
		{"transform": Transform3D(Basis(Vector3.UP, PI), Vector3(-7.0, WaterLevel, (RiverSpan.x + RiverSpan.y) * 0.5)),
		"tint": Color.WHITE, "phase": rng.randf()}]
	# Out in the channel itself, reed clumps stand in the water: a thick bed
	# against the wall that thins as the river runs off to the sides, with lily
	# pads in the open water between them.
	for side in [-1.0, 1.0]:
		var step: float = 0.38
		var d: float = 0.95
		while d < 12.0:
			var cz: float = RiverSpan.x + 0.25
			while cz < RiverSpan.y - 0.25:
				var px: float = side * (7.0 + d + rng.randf_range(-0.12, 0.12))
				var pz: float = cz + rng.randf_range(-0.12, 0.12)
				# A bed against the wall that opens into clear blue water within a
				# few tiles; only the odd clump stands out in the open channel.
				var thick: float = pow(clampf(1.0 - (d - 0.95) / 3.5, 0.0, 1.0), 1.6)
				var roll: float = rng.randf()
				if roll < 0.03 + 0.8 * thick:
					add.call("reeds", px, pz, rng.randf_range(0.75, 1.05) * lerpf(0.8, 1.05, thick), WaterLevel)
				elif roll < 0.03 + 0.8 * thick + 0.05:
					add.call("lily", px, pz, rng.randf_range(0.8, 1.2), WaterLevel + 0.012)
				cz += step
			d += step
	# A loose grove fills the slope behind the home row's left half, which the
	# open-hill thinning below would otherwise leave nearly bare.
	var gz: float = BackGrove.position.y
	while gz < BackGrove.end.y:
		var gx: float = BackGrove.position.x
		while gx < BackGrove.end.x:
			var px: float = gx + rng.randf_range(-0.45, 0.45)
			var pz: float = gz + rng.randf_range(-0.4, 0.4)
			var roll: float = rng.randf()
			# Bushes along the hedge line, trees further up the slope.
			var tree_share: float = smoothstep(BackGrove.end.y, BackGrove.position.y, pz) * 0.75
			if roll < tree_share:
				add.call("tree" if rng.randf() < 0.7 else "pine", px, pz, rng.randf_range(0.9, 1.25), NAN)
			elif roll < 0.85:
				add.call("shrub", px, pz, rng.randf_range(1.0, 1.4), NAN)
			gx += 1.55
		gz += 1.45	# Scatter only what the cameras can see short of the fog.
	var cell: float = 1.5
	var z: float = -16.0
	while z < 15.0:
		var x: float = -18.0
		while x < 18.0:
			var px: float = x + rng.randf_range(0.1, cell - 0.1)
			var pz: float = z + rng.randf_range(0.1, cell - 0.1)
			x += cell
			var limit: float = height_limit(px, pz)
			if limit <= 0.0 or BackGrove.has_point(Vector2(px, pz)):
				continue
			var d: float = absf(px) - 7.0
			var h: float = terrain_height(px, pz)
			var roll: float = rng.randf()
			if swamp_weight(px, pz) > 0.2:
				continue  # the channel is dressed above
			if d > 0.9 and pz < RiverSpan.x - 0.9 and pz > RiverSpan.x - 4.0:
				# The swampy lower bank: moss-hung cypresses and shrubs.
				if roll < 0.38:
					add.call("cypress", px, pz, rng.randf_range(0.85, 1.15), NAN)
				elif roll < 0.6:
					add.call("shrub", px, pz, rng.randf_range(0.8, 1.1), NAN)
				continue
			if d > 0.0 and d < 1.6 and pz > RoadSpan.x - 0.2 and pz < RoadSpan.y + 0.2:
				limit = minf(limit, 1.2)  # only low growth on the crest over a tunnel
			# Woods thicken away from the board at the sides; above and below the
			# board the rolling hills stay open, with only a few trees and bushes.
			var edge: float = maxf(d, maxf(BoardBack - pz, pz - BoardFront))
			var wood: float = clampf(edge / 4.0, 0.3, 1.0)
			if d < 2.0 and (pz > BoardFront or pz < BoardBack):
				wood = 0.22
			if roll < 0.40 * wood and 2.8 <= limit:
				add.call("tree", px, pz, rng.randf_range(0.85, 1.15), NAN)
			elif roll < 0.55 * wood and 3.0 <= limit:
				add.call("pine", px, pz, rng.randf_range(0.85, 1.15), NAN)
			elif roll < 0.55 * wood + 0.15 and 1.0 <= limit:
				add.call("shrub", px, pz, rng.randf_range(0.8, 1.15), NAN)
		z += cell
	# The hills over the tunnels carry low shrubs and grass, so their slopes
	# never read as bare sheets beside the lanes.
	for i in range(220):
		var hx: float = rng.randf_range(7.6, 15.0) * (1.0 if rng.randf() < 0.5 else -1.0)
		var hz: float = rng.randf_range(-0.5, 7.0)
		if terrain_height(hx, hz) < 0.3 or height_limit(hx, hz) < 1.0:
			continue
		add.call("shrub" if rng.randf() < 0.35 else "grass", hx, hz, rng.randf_range(0.8, 1.2), NAN)
	# Grass and flower tufts dot the open ground nearest the board.
	for i in range(520):
		var fx: float = rng.randf_range(-16.0, 16.0)
		var fz: float = rng.randf_range(-13.0, 15.0)
		if height_limit(fx, fz) < 0.5 or terrain_height(fx, fz) < WaterLevel + 0.03:
			continue
		# The open meadow in front of the board carries most of the flowers.
		var bloom: float = 0.55 if fz > BoardFront and absf(fx) < 11.0 else 0.25
		add.call("flowers" if rng.randf() < bloom else "grass", fx, fz, rng.randf_range(0.8, 1.25), NAN)
	return out

func make_multimesh(mesh: Mesh, items: Array) -> MultiMeshInstance3D:
	var mm := MultiMesh.new()
	mm.transform_format = MultiMesh.TRANSFORM_3D
	mm.use_colors = true
	mm.use_custom_data = true
	mm.mesh = mesh
	mm.instance_count = items.size()
	for i in range(items.size()):
		mm.set_instance_transform(i, items[i].transform)
		mm.set_instance_color(i, items[i].tint)
		mm.set_instance_custom_data(i, Color(0.0, 0.0, 0.0, items[i].phase))
	var node := MultiMeshInstance3D.new()
	node.multimesh = mm
	return node

## Low-poly 3D clouds like the key art: chunky faceted white puffs with cool
## blue shaded facets. They ride a ring around the active camera, at the pitches
## its views see sky through (measured: about -0.45 to -0.68), and drift slowly. They are real geometry, so
## their depth is pushed to the far plane, so every other object hides them.
const CloudShader: String = """shader_type spatial;
render_mode unshaded, fog_disabled, cull_back;
// Set each frame to the camera's far distance.
uniform float far_plane = 30.0;
uniform vec3 lit : source_color = vec3(1.0, 1.0, 1.0);
uniform vec3 shade : source_color = vec3(0.66, 0.80, 0.98);
uniform vec3 sun = vec3(-0.45, 0.8, 0.35);
void vertex(){
    // Keep the cloud's place on screen but push its depth to just inside the far
    // plane, so every other 3D object draws in front of it: clouds only ever show
    // against the sky. Depth still grows with distance, so overlapping clouds
    // and facets keep their order. Reading the far depth through the projection
    // matrix works for either depth convention and either projection.
    vec4 view = MODELVIEW_MATRIX * vec4(VERTEX, 1.0);
    POSITION = PROJECTION_MATRIX * view;
    float k = 0.985 + 0.014 * clamp(length(view.xyz) / 40.0, 0.0, 1.0);
    vec4 far_clip = PROJECTION_MATRIX * vec4(0.0, 0.0, -far_plane * k, 1.0);
    POSITION.z = far_clip.z / far_clip.w * POSITION.w;
}
void fragment(){
    // Flat facet normals give the faceted low-poly look.
    vec3 n = normalize(cross(dFdx(VERTEX), dFdy(VERTEX)));
    vec3 l = normalize((VIEW_MATRIX * vec4(sun, 0.0)).xyz);
    float k = clamp(dot(n, l) * 0.6 + 0.55, 0.0, 1.0);
    ALBEDO = mix(shade, lit, smoothstep(0.35, 0.75, k));
}"""
## Inside the camera's far plane (about 30 units away): any ray that shows sky
## hits nothing nearer, so a cloud on it is always seen.
const CloudRadius: float = 22.0
const CloudCount: int = 30
const CloudDrift: float = 0.006

func cloud_mesh(seed_value: int) -> ArrayMesh:
	var cloud_rng := RandomNumberGenerator.new()
	cloud_rng.seed = seed_value
	var st := SurfaceTool.new()
	st.begin(Mesh.PRIMITIVE_TRIANGLES)
	# A big central puff flanked by smaller ones, all sharing a flat bottom.
	var puffs: Array = [[Vector3(0, 0.35, 0), 1.0]]
	for k in range(cloud_rng.randi_range(3, 5)):
		var side: float = -1.0 if k % 2 == 0 else 1.0
		puffs.append([Vector3(side * cloud_rng.randf_range(0.7, 1.4), cloud_rng.randf_range(0.0, 0.3), cloud_rng.randf_range(-0.35, 0.35)), cloud_rng.randf_range(0.5, 0.78)])
	for puff in puffs:
		var sphere := SphereMesh.new()
		sphere.radius = puff[1]
		sphere.height = puff[1] * 2.0
		sphere.radial_segments = 7
		sphere.rings = 4
		var arrays: Array = sphere.get_mesh_arrays()
		var verts: PackedVector3Array = arrays[Mesh.ARRAY_VERTEX]
		var indices: PackedInt32Array = arrays[Mesh.ARRAY_INDEX]
		for index in indices:
			var v: Vector3 = verts[index] + puff[0]
			v.y = maxf(v.y, -0.05)
			st.add_vertex(v)
	return st.commit()

func build_clouds() -> Node3D:
	var root := Node3D.new()
	root.name = "Clouds"
	var material := ShaderMaterial.new()
	material.shader = Shader.new()
	material.shader.code = CloudShader
	cloud_material = material
	var variants: Array = []
	for k in range(3):
		var mesh := cloud_mesh(Seed + 17 * k)
		mesh.surface_set_material(0, material)
		var mm := MultiMesh.new()
		mm.transform_format = MultiMesh.TRANSFORM_3D
		mm.mesh = mesh
		variants.append([mm, []])
	for i in range(CloudCount):
		# Spread evenly around the ring at the low pitches the game cameras look
		# through to see sky beyond the curved world.
		var yaw: float = TAU * (float(i) + rng.randf_range(-0.3, 0.3)) / float(CloudCount)
		var pitch: float = rng.randf_range(-0.66, -0.44)
		var dir := Vector3(sin(yaw) * sqrt(1.0 - pitch * pitch), pitch, -cos(yaw) * sqrt(1.0 - pitch * pitch))
		var scale: float = rng.randf_range(1.0, 1.6)
		var basis := Basis(Vector3.UP, yaw + rng.randf_range(-0.3, 0.3)).scaled(Vector3(scale, scale * 0.8, scale * 0.7))
		variants[i % variants.size()][1].append(Transform3D(basis, dir * CloudRadius))
	for variant in variants:
		var mm: MultiMesh = variant[0]
		mm.instance_count = variant[1].size()
		for i in range(mm.instance_count):
			mm.set_instance_transform(i, variant[1][i])
		var node := MultiMeshInstance3D.new()
		node.multimesh = mm
		node.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
		root.add_child(node)
	return root

func _process(delta: float) -> void:
	var camera := get_viewport().get_camera_3d()
	if clouds == null or camera == null:
		return
	clouds.global_position = camera.global_position
	cloud_material.set_shader_parameter("far_plane", camera.far)
	clouds.rotate_y(CloudDrift * delta)