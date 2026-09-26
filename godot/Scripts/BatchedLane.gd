class_name BatchedLane

## Draws every instance of one rigid lane model (logs, vehicle bodies and the
## four wheel pivots of each vehicle) through MultiMesh instead of one node tree
## per prop. The authored GLB is the only source of mesh data, surface tints and
## wheel pivots; wheels turn through instance transforms, so nothing here is
## skinned or animated and one lane costs a handful of draw calls.

const Capacity: int = 64

var body: MultiMeshInstance3D = null
var wheels_left: MultiMeshInstance3D = null
var wheels_right: MultiMeshInstance3D = null
var count: int = 0
var left_count: int = 0
var right_count: int = 0
var turn: float = 0.0
var wheel_offsets: Array[Transform3D] = []
var wheel_left_count: int = 0
var overflow_warned: bool = false

static var templates: Dictionary = {}
static var lane_shader: Shader = null

class Template:
	var body_mesh: Mesh = null
	var wheel_mesh_left: Mesh = null
	var wheel_mesh_right: Mesh = null
	var wheel_offsets: Array[Transform3D] = []
	var wheel_left_count: int = 0

static func _get_lane_shader() -> Shader:
	if lane_shader == null:
		lane_shader = Shader.new()
		lane_shader.code = """shader_type spatial;
uniform vec4 tint : source_color = vec4(1.0);
varying vec3 world;
void vertex(){
    world=(MODEL_MATRIX*vec4(VERTEX,1.0)).xyz;
}
void fragment(){
    // Lane props are cut at the board edge exactly like the clipped ModelActor
    // materials; an instance that fits on the board never reaches the discard.
    if(abs(world.x)>7.0)discard;
    ALBEDO=tint.rgb;
    SPECULAR=0.5;
    ROUGHNESS=.75;
}"""
	return lane_shader

static func _tinted_mesh(source: Mesh) -> Mesh:
	# Copy the imported surfaces onto a lane shader material instead of mutating
	# the GLB resource, which the review tools may still instance per actor.
	var imported := source as ArrayMesh
	if imported == null:
		push_error("Batched model surfaces must import as ArrayMesh")
		return null
	var mesh := ArrayMesh.new()
	for surface in range(imported.get_surface_count()):
		mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, imported.surface_get_arrays(surface))
		var original := imported.surface_get_material(surface)
		if not (original is StandardMaterial3D):
			push_error("Batched surfaces need an authored tint")
			return null
		var material := ShaderMaterial.new()
		material.shader = _get_lane_shader()
		material.set_shader_parameter("tint", (original as StandardMaterial3D).albedo_color)
		mesh.surface_set_material(surface, material)
	return mesh

static func _collect_mesh_nodes(node: Node, out: Array) -> void:
	for child in node.get_children():
		if child is MeshInstance3D and (child as MeshInstance3D).mesh != null:
			out.append(child)
		_collect_mesh_nodes(child, out)

static func _build_template(asset: String) -> Template:
	var scene := load("res://Models/%s.glb" % asset) as PackedScene
	if scene == null:
		push_error("Missing batched model %s" % asset)
		return null
	var authored := scene.instantiate()
	var template := Template.new()
	var mesh_nodes: Array = []
	_collect_mesh_nodes(authored, mesh_nodes)
	var left_meshes: Array = []
	var right_meshes: Array = []
	for mesh_node in mesh_nodes:
		var node := mesh_node as MeshInstance3D
		if node.name.begins_with("Wheel"):
			# Blender bakes the hub detail off-center per side, so keep one
			# mesh per side instead of sharing a single wheel mesh across the
			# axle: sharing the left mesh under a right pivot offsets every
			# right hub cap ~9cm toward the chassis.
			var tinted := _tinted_mesh(node.mesh)
			if tinted == null:
				authored.free()
				return null
			if node.transform.origin.x < 0.0:
				left_meshes.append(tinted)
			else:
				right_meshes.append(tinted)
			template.wheel_offsets.append(node.transform)
		else:
			if template.body_mesh != null:
				push_error("%s exports more than one rigid body mesh" % asset)
				authored.free()
				return null
			template.body_mesh = _tinted_mesh(node.mesh)
	authored.free()
	if template.body_mesh == null:
		push_error("%s has no rigid body mesh to batch" % asset)
		return null
	if not template.wheel_offsets.is_empty() and template.wheel_offsets.size() != 4:
		push_error("%s must export four wheel pivots" % asset)
		return null
	# The GLB exports front axle before rear axle on each side, so the first
	# two pivots belong to the -X side. Sort them left-first so the left/right
	# counters below stay aligned with the per-side meshes.
	var paired: Array = []
	for offset in template.wheel_offsets:
		paired.append(offset)
	paired.sort_custom(func(a: Transform3D, b: Transform3D) -> bool: return a.origin.x < b.origin.x)
	template.wheel_offsets = []
	for offset in paired:
		template.wheel_offsets.append(offset)
	template.wheel_left_count = 0
	for offset in template.wheel_offsets:
		if offset.origin.x < 0.0:
			template.wheel_left_count += 1
	template.wheel_mesh_left = _combined_mesh(left_meshes)
	template.wheel_mesh_right = _combined_mesh(right_meshes)
	if left_meshes.size() + right_meshes.size() != template.wheel_offsets.size():
		push_error("%s lost a wheel mesh while batching" % asset)
		return null
	if template.wheel_left_count != left_meshes.size():
		push_error("%s has an asymmetric wheel layout" % asset)
		return null
	return template

static func get_template(asset: String) -> Template:
	if not templates.has(asset):
		templates[asset] = _build_template(asset)
	return templates[asset] as Template

static func _combined_mesh(meshes: Array) -> Mesh:
	# One MultiMesh draws one mesh resource, so merge the per-side wheel meshes
	# of that side into a single resource. The offsets stay per-pivot; the mesh
	# index below picks which merged surface set each pivot draws.
	var combined := ArrayMesh.new()
	for mesh in meshes:
		var imported := mesh as ArrayMesh
		if imported == null:
			push_error("Batched wheel mesh must import as ArrayMesh")
			return null
		for surface in range(imported.get_surface_count()):
			combined.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, imported.surface_get_arrays(surface))
			combined.surface_set_material(combined.get_surface_count() - 1, imported.surface_get_material(surface))
	if combined.get_surface_count() == 0:
		return null
	return combined

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
	wheel_offsets = template.wheel_offsets
	wheel_left_count = template.wheel_left_count
	body = _make_view(parent, "Batched %s" % asset, template.body_mesh, Capacity)
	if template.wheel_mesh_left != null:
		wheels_left = _make_view(parent, "Batched %s wheels L" % asset, template.wheel_mesh_left, Capacity * wheel_left_count)
	if template.wheel_mesh_right != null:
		wheels_right = _make_view(parent, "Batched %s wheels R" % asset, template.wheel_mesh_right, Capacity * (wheel_offsets.size() - wheel_left_count))

func begin(turn_radians: float) -> void:
	count = 0
	left_count = 0
	right_count = 0
	turn = turn_radians

func place(origin: Transform3D) -> void:
	if body == null:
		return
	if count >= Capacity:
		if not overflow_warned:
			push_warning("Batched lane overflowed its MultiMesh capacity")
			overflow_warned = true
		return
	body.multimesh.set_instance_transform(count, origin)
	# Spin in the wheel's own space so its pivot stays put. One shared turn
	# clock drives every wheel in the lane; the per-lane phase from
	# FroggerGame keeps parallel traffic from spinning in lockstep.
	var spin := Basis(Vector3.RIGHT, turn)
	for index in range(wheel_offsets.size()):
		var left_side := index < wheel_left_count
		var view := wheels_left if left_side else wheels_right
		if view == null:
			continue
		var slot: int = (left_count if left_side else right_count)
		# Left pivots sit on the -X side of the chassis, so slot the two left
		# pivots first and the two right pivots into the right MultiMesh.
		view.multimesh.set_instance_transform(slot,
			origin * wheel_offsets[index] * Transform3D(spin, Vector3.ZERO))
		if left_side:
			left_count += 1
		else:
			right_count += 1
	count += 1

func finish() -> void:
	if body == null:
		return
	body.multimesh.visible_instance_count = count
	if wheels_left != null:
		wheels_left.multimesh.visible_instance_count = left_count
	if wheels_right != null:
		wheels_right.multimesh.visible_instance_count = right_count

func reset() -> void:
	# Lanes rebuild their instances every frame; this only clears props a
	# previous match left behind until the next placement pass.
	count = 0
	left_count = 0
	right_count = 0
	finish()

func visible_instances() -> int:
	return count
