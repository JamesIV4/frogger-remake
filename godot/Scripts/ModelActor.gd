class_name ModelActor

var root: Node3D
var animator: AnimationPlayer
var passenger_socket: Node3D
var skeleton: Skeleton3D
var mesh_instances: Array[MeshInstance3D] = []
var surface_bindings: Array = []
var current_anim: String = ""
var squash_color: float = -1.0
var is_active: bool = true
var is_clipped: bool = false
var supports_squash_color: bool = false

static var unclipped_materials: Dictionary = {}
static var clipped_materials: Dictionary = {}
static var unclipped_shaders: Dictionary = {}
static var clipped_shaders: Dictionary = {}

static func _get_unclipped_shader(animated_squash: bool) -> Shader:
	if not unclipped_shaders.has(animated_squash):
		var unclipped_shader := Shader.new()
		unclipped_shader.code = """shader_type spatial;
render_mode depth_draw_opaque;
instance uniform float squash_color = 0.0;
uniform vec4 tint : source_color = vec4(1.0);
varying float intact_shade;
void vertex(){
    intact_shade=0.35+0.65*max(dot(normalize(NORMAL),normalize(vec3(-0.45,0.8,0.35))),0.0);
}
void fragment(){
    ALBEDO=tint.rgb*(1.0-squash_color);
    EMISSION=tint.rgb*intact_shade*0.9*squash_color;
    SPECULAR=0.5*(1.0-squash_color);
    ROUGHNESS=.75;
}"""
		if not animated_squash:
			unclipped_shader.code = unclipped_shader.code.replace("instance uniform float squash_color = 0.0;", "const float squash_color = 0.0;")
		unclipped_shaders[animated_squash] = unclipped_shader
	return unclipped_shaders[animated_squash]

static func _get_clipped_shader(animated_squash: bool) -> Shader:
	if not clipped_shaders.has(animated_squash):
		var clipped_shader := Shader.new()
		clipped_shader.code = """shader_type spatial;
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
		if not animated_squash:
			clipped_shader.code = clipped_shader.code.replace("instance uniform float squash_color = 0.0;", "const float squash_color = 0.0;")
		clipped_shaders[animated_squash] = clipped_shader
	return clipped_shaders[animated_squash]

func _init(parent: Node, asset: String):
	# Each mesh with an instance uniform consumes one of GLES3's limited slots,
	# even on hidden actors. Only playable frog models need the squash effect.
	supports_squash_color = asset in ["frog", "lady_frog"]
	var scene = load("res://Models/%s.glb" % asset)
	root = scene.instantiate() as Node3D
	parent.add_child(root)
	_setup_materials(root)
	animator = _find_node_of_type(root, "AnimationPlayer") as AnimationPlayer
	passenger_socket = root.find_child("PassengerSocket", true, false) as Node3D
	skeleton = _find_node_of_type(root, "Skeleton3D") as Skeleton3D
	play("Idle")
	set_active(false)

func _setup_materials(node: Node) -> void:
	if node is MeshInstance3D and node.mesh != null:
		var mesh: MeshInstance3D = node as MeshInstance3D
		for i in range(mesh.mesh.get_surface_count()):
			var original = mesh.mesh.surface_get_material(i)
			if original is StandardMaterial3D:
				var id: String = "%d:%s" % [original.get_instance_id(), supports_squash_color]
				var unclipped_mat: ShaderMaterial
				if unclipped_materials.has(id):
					unclipped_mat = unclipped_materials[id]
				else:
					unclipped_mat = ShaderMaterial.new()
					unclipped_mat.shader = _get_unclipped_shader(supports_squash_color)
					unclipped_mat.set_shader_parameter("tint", original.albedo_color)
					unclipped_materials[id] = unclipped_mat

				var clipped_mat: ShaderMaterial
				if clipped_materials.has(id):
					clipped_mat = clipped_materials[id]
				else:
					clipped_mat = ShaderMaterial.new()
					clipped_mat.shader = _get_clipped_shader(supports_squash_color)
					clipped_mat.set_shader_parameter("tint", original.albedo_color)
					clipped_materials[id] = clipped_mat

				mesh.set_surface_override_material(i, unclipped_mat)
				surface_bindings.append({
					"mesh": mesh,
					"surface": i,
					"unclipped": unclipped_mat,
					"clipped": clipped_mat
				})
				if not mesh_instances.has(mesh):
					mesh_instances.append(mesh)
	for child in node.get_children():
		_setup_materials(child)

func set_clipped(clipped: bool) -> void:
	if is_clipped == clipped:
		return
	is_clipped = clipped
	for binding in surface_bindings:
		var mat = binding["clipped"] if clipped else binding["unclipped"]
		binding["mesh"].set_surface_override_material(binding["surface"], mat)

func set_active(active: bool) -> void:
	if is_active == active:
		return
	is_active = active
	root.visible = active
	var mode: int = Node.PROCESS_MODE_INHERIT if active else Node.PROCESS_MODE_DISABLED
	if animator != null:
		animator.process_mode = mode
	if skeleton != null:
		skeleton.process_mode = mode

func set_squash_color(amount: float) -> void:
	if not supports_squash_color:
		return
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
