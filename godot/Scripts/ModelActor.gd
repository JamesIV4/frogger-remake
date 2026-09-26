class_name ModelActor

var root: Node3D
var animator: AnimationPlayer
var passenger_socket: Node3D
var skeleton: Skeleton3D
var mesh_instances: Array[MeshInstance3D] = []
var current_anim: String = ""
var squash_color: float = -1.0

static var clipped_materials: Dictionary = {}
static var clip_shader: Shader = null

static func _get_clip_shader() -> Shader:
	if clip_shader == null:
		clip_shader = Shader.new()
		clip_shader.code = """shader_type spatial;
instance uniform float squash_color = 0.0;
uniform vec4 tint : source_color = vec4(1.0);
varying vec3 world;
varying float intact_shade;
void vertex(){
    world=(MODEL_MATRIX*vec4(VERTEX,1.0)).xyz;
    intact_shade=0.35+0.65*max(dot(normalize(NORMAL),normalize(vec3(-0.45,0.8,0.35))),0.0);
}
void fragment(){
    if(abs(world.x)>7.0)discard;
    ALBEDO=tint.rgb*(1.0-squash_color);
    EMISSION=tint.rgb*intact_shade*0.9*squash_color;
    SPECULAR=0.5*(1.0-squash_color);
    ROUGHNESS=.75;
}"""
	return clip_shader

func _init(parent: Node, asset: String):
	var scene = load("res://Models/%s.glb" % asset)
	root = scene.instantiate() as Node3D
	parent.add_child(root)
	_clip_at_board_edge(root)
	animator = _find_node_of_type(root, "AnimationPlayer") as AnimationPlayer
	passenger_socket = root.find_child("PassengerSocket", true, false) as Node3D
	skeleton = _find_node_of_type(root, "Skeleton3D") as Skeleton3D
	play("Idle")

func _clip_at_board_edge(node: Node) -> void:
	if node is MeshInstance3D and node.mesh != null:
		var mesh: MeshInstance3D = node as MeshInstance3D
		for i in range(mesh.mesh.get_surface_count()):
			var original = mesh.mesh.surface_get_material(i)
			if original is StandardMaterial3D:
				var id: int = original.get_instance_id()
				var shader_mat: ShaderMaterial
				if clipped_materials.has(id):
					shader_mat = clipped_materials[id]
				else:
					shader_mat = ShaderMaterial.new()
					shader_mat.shader = _get_clip_shader()
					shader_mat.set_shader_parameter("tint", original.albedo_color)
					clipped_materials[id] = shader_mat
				mesh.set_surface_override_material(i, shader_mat)
				if not mesh_instances.has(mesh):
					mesh_instances.append(mesh)
	for child in node.get_children():
		_clip_at_board_edge(child)

func set_squash_color(amount: float) -> void:
	amount = clampf(amount, 0.0, 1.0)
	if absf(amount - squash_color) < 0.001:
		return
	squash_color = amount
	for mesh in mesh_instances:
		mesh.set_instance_shader_parameter("squash_color", amount)

func play(clip: String, loop: bool = true, speed: float = 1.0) -> void:
	if animator == null:
		return
	for anim_name in animator.get_animation_list():
		if anim_name.to_lower().ends_with(clip.to_lower()):
			if current_anim == anim_name and animator.is_playing():
				return
			var anim = animator.get_animation(anim_name)
			anim.loop_mode = Animation.LOOP_LINEAR if loop else Animation.LOOP_NONE
			animator.speed_scale = speed
			animator.play(anim_name, 0.035)
			current_anim = anim_name
			return

func pose(clip: String, seconds: float) -> void:
	if animator == null:
		return
	for anim_name in animator.get_animation_list():
		if anim_name.to_lower().ends_with(clip.to_lower()):
			var anim = animator.get_animation(anim_name)
			if current_anim != anim_name:
				animator.play(anim_name, 0.0)
				current_anim = anim_name
			animator.seek(clampf(seconds, 0.0, anim.length), true, true)
			animator.pause()
			return
	push_error("Missing required clip %s on %s" % [clip, root.name])

func _find_node_of_type(n: Node, type_name: String) -> Node:
	if n.is_class(type_name):
		return n
	for child in n.get_children():
		var found = _find_node_of_type(child, type_name)
		if found != null:
			return found
	return null
