class_name BatchedLane

## Draws every instance of one rigid lane model (logs, vehicle bodies and the
## two axles of each vehicle) through MultiMesh instead of one node tree per
## prop. The authored GLB is the only source of mesh data, surface tints and
## wheel pivots; wheels turn through instance transforms, so nothing here is
## skinned or animated. TintedMesh folds each model's palette surfaces into one
## surface, so a log lane is one draw call and a vehicle lane two (body, axles)
## per render pass.

const Capacity: int = 64

var body: MultiMeshInstance3D = null
var axles: MultiMeshInstance3D = null
var count: int = 0
var axle_count: int = 0
var spin: Transform3D = Transform3D.IDENTITY
var axle_offsets: Array[Transform3D] = []
var overflow_warned: bool = false

static var templates: Dictionary = {}
static var lane_shader: Shader = null

class Template:
	var body_mesh: Mesh = null
	var axle_mesh: Mesh = null
	var axle_offsets: Array[Transform3D] = []

static func _get_lane_shader() -> Shader:
	if lane_shader == null:
		lane_shader = Shader.new()
		lane_shader.code = """shader_type spatial;
#include "res://Shaders/curve.gdshaderinc"
#include "res://Shaders/tint.gdshaderinc"
varying vec3 world;
void vertex(){
    READ_TINT;
    world=(MODEL_MATRIX*vec4(VERTEX,1.0)).xyz;
    VERTEX+=curve_offset(MODEL_MATRIX,VERTEX);
}
void fragment(){
    // Lane props fade and are cut past the board edge exactly like the clipped
    // ModelActor materials: into the tunnel's darkness on the road, the swamp's
    // mist on the river. An instance on the board never reaches the fade.
    float edge=abs(world.x);
    if(edge>7.85)discard;
    float fade=smoothstep(7.0,7.8,edge);
    vec3 murk=world.z<0.0?vec3(0.38,0.5,0.42):vec3(0.0);
    ALBEDO=mix(tint.rgb,murk,fade);
    SPECULAR=0.5*(1.0-fade);
    ROUGHNESS=.75;
}"""
	return lane_shader

static func _collect_mesh_nodes(node: Node, out: Array) -> void:
	for child in node.get_children():
		if child is MeshInstance3D and (child as MeshInstance3D).mesh != null:
			out.append(child)
		_collect_mesh_nodes(child, out)

static func _same_vertices(a: Mesh, b: Mesh) -> bool:
	if a.get_surface_count() != b.get_surface_count():
		return false
	for surface in range(a.get_surface_count()):
		if a.surface_get_arrays(surface)[Mesh.ARRAY_VERTEX] != b.surface_get_arrays(surface)[Mesh.ARRAY_VERTEX]:
			return false
	return true

static func _build_template(asset: String) -> Template:
	var scene := load("res://Models/%s.glb" % asset) as PackedScene
	if scene == null:
		push_error("Missing batched model %s" % asset)
		return null
	var authored := scene.instantiate()
	var template := Template.new()
	var mesh_nodes: Array = []
	_collect_mesh_nodes(authored, mesh_nodes)
	var palette: Array = []
	var body_parts: Array = []
	var wheels: Array = []
	for mesh_node in mesh_nodes:
		var node := mesh_node as MeshInstance3D
		if node.name.begins_with("Wheel"):
			wheels.append(node)
		elif not body_parts.is_empty():
			push_error("%s exports more than one rigid body mesh" % asset)
			authored.free()
			return null
		else:
			body_parts = TintedMesh.color_parts(node.mesh, palette)
			if body_parts.is_empty():
				authored.free()
				return null
	if body_parts.is_empty():
		push_error("%s has no rigid body mesh to batch" % asset)
		authored.free()
		return null
	template.body_mesh = TintedMesh.merge(body_parts)
	if not wheels.is_empty():
		template.axle_mesh = _build_axles(asset, wheels, palette, template.axle_offsets)
		if template.axle_mesh == null:
			authored.free()
			return null
	authored.free()
	# Body and axles share one palette, so one material serves both.
	var material := ShaderMaterial.new()
	material.shader = _get_lane_shader()
	material.set_shader_parameter("tints", TintedMesh.tints(palette))
	template.body_mesh.surface_set_material(0, material)
	if template.axle_mesh != null:
		template.axle_mesh.surface_set_material(0, material)
	return template

static func _build_axles(asset: String, wheels: Array, palette: Array, offsets: Array[Transform3D]) -> Mesh:
	# The left and right wheel pivots of an axle sit on one line across the
	# chassis (same height and depth), and every wheel turns about that line.
	# Draw both wheels of an axle as one instance spun about its center: the
	# pivots stay put and each wheel turns exactly as it would alone. Blender
	# bakes the hub detail off-center per side, so each side keeps its own mesh.
	if wheels.size() != 4:
		push_error("%s must export four wheel pivots" % asset)
		return null
	var grouped: Dictionary = {}
	for wheel in wheels:
		var pivot: Transform3D = (wheel as MeshInstance3D).transform
		if not pivot.basis.is_equal_approx(Basis.IDENTITY):
			push_error("%s wheel pivots must not rotate" % asset)
			return null
		var key := Vector2(snappedf(pivot.origin.y, 0.0001), snappedf(pivot.origin.z, 0.0001))
		if not grouped.has(key):
			grouped[key] = []
		grouped[key].append(wheel)
	var axle_mesh: Mesh = null
	var first: Array = []
	for key in grouped:
		var pair: Array = grouped[key]
		if pair.size() != 2:
			push_error("%s has an asymmetric wheel layout" % asset)
			return null
		pair.sort_custom(func(a: MeshInstance3D, b: MeshInstance3D) -> bool: return a.transform.origin.x < b.transform.origin.x)
		var left: MeshInstance3D = pair[0]
		var right: MeshInstance3D = pair[1]
		if left.transform.origin.x >= 0.0 or right.transform.origin.x <= 0.0:
			push_error("%s axle lacks a wheel on each side" % asset)
			return null
		var center: Vector3 = (left.transform.origin + right.transform.origin) * 0.5
		if axle_mesh == null:
			var parts: Array = []
			for wheel in pair:
				parts.append_array(TintedMesh.color_parts(wheel.mesh, palette, Transform3D(Basis.IDENTITY, wheel.transform.origin - center)))
			axle_mesh = TintedMesh.merge(parts)
			first = [left, right, center]
		elif not (_same_vertices(left.mesh, first[0].mesh) and _same_vertices(right.mesh, first[1].mesh)
				and (left.transform.origin - center).is_equal_approx(first[0].transform.origin - first[2])
				and (right.transform.origin - center).is_equal_approx(first[1].transform.origin - first[2])):
			push_error("%s axles differ; they cannot share one mesh" % asset)
			return null
		offsets.append(Transform3D(Basis.IDENTITY, center))
	return axle_mesh

static func get_template(asset: String) -> Template:
	if not templates.has(asset):
		templates[asset] = _build_template(asset)
	return templates[asset] as Template

static func _make_view(parent: Node3D, name: String, mesh: Mesh, capacity: int) -> MultiMeshInstance3D:
	var multimesh := MultiMesh.new()
	multimesh.transform_format = MultiMesh.TRANSFORM_3D
	multimesh.mesh = mesh
	multimesh.instance_count = capacity
	multimesh.visible_instance_count = 0
	var view := MultiMeshInstance3D.new()
	view.name = name
	view.multimesh = multimesh
	view.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_ON
	parent.add_child(view)
	return view

func _init(parent: Node3D, asset: String) -> void:
	var template := get_template(asset)
	if template == null or template.body_mesh == null:
		push_error("Missing batched template %s" % asset)
		return
	axle_offsets = template.axle_offsets
	body = _make_view(parent, "Batched %s" % asset, template.body_mesh, Capacity)
	if template.axle_mesh != null:
		axles = _make_view(parent, "Batched %s axles" % asset, template.axle_mesh, Capacity * axle_offsets.size())

func begin(turn_radians: float) -> void:
	count = 0
	axle_count = 0
	# Spin in the axle's own space so its pivots stay put. One shared turn
	# clock drives every wheel in the lane; the per-lane phase from
	# FroggerGame keeps parallel traffic from spinning in lockstep.
	spin = Transform3D(Basis(Vector3.RIGHT, turn_radians), Vector3.ZERO)

func place(origin: Transform3D) -> void:
	if body == null:
		return
	if count >= Capacity:
		if not overflow_warned:
			push_warning("Batched lane overflowed its MultiMesh capacity")
			overflow_warned = true
		return
	body.multimesh.set_instance_transform(count, origin)
	if axles != null:
		for offset in axle_offsets:
			axles.multimesh.set_instance_transform(axle_count, origin * offset * spin)
			axle_count += 1
	count += 1

func finish() -> void:
	if body == null:
		return
	body.multimesh.visible_instance_count = count
	if axles != null:
		axles.multimesh.visible_instance_count = axle_count

func reset() -> void:
	# Lanes rebuild their instances every frame; this only clears props a
	# previous match left behind until the next placement pass.
	count = 0
	axle_count = 0
	finish()

func visible_instances() -> int:
	return count
