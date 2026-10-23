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
var frame_requested: bool = false
var model_name: String
var animation_names: Dictionary = {}
var is_clipped: bool = false
var supports_squash_color: bool = false
var squash_materials: Dictionary = {}

static var unclipped_materials: Dictionary = {}
static var clipped_materials: Dictionary = {}
static var unclipped_shaders: Dictionary = {}
static var clipped_shaders: Dictionary = {}
## Per model: the merged mesh of each part group and the shared palette.
static var merged_models: Dictionary = {}
## Per model: the imported scene. The resource cache only holds scenes that
## are still referenced, and actors keep none of the imported meshes, so
## without this every new actor would re-read its GLB from disk.
static var scenes: Dictionary = {}

static func _get_unclipped_shader(animated_squash: bool) -> Shader:
	if not unclipped_shaders.has(animated_squash):
		var unclipped_shader := Shader.new()
		unclipped_shader.code = """shader_type spatial;
render_mode depth_draw_opaque;
#include "res://Shaders/curve.gdshaderinc"
#include "res://Shaders/tint.gdshaderinc"
uniform float squash_color = 0.0;
varying float intact_shade;
void vertex(){
    READ_TINT;
    intact_shade=0.35+0.65*max(dot(normalize(NORMAL),normalize(vec3(-0.45,0.8,0.35))),0.0);
    VERTEX+=curve_offset(MODEL_MATRIX,VERTEX);
}
void fragment(){
    ALBEDO=tint.rgb*(1.0-squash_color);
    EMISSION=tint.rgb*intact_shade*0.9*squash_color;
    SPECULAR=0.5*(1.0-squash_color);
    ROUGHNESS=.75;
}"""
		if not animated_squash:
			unclipped_shader.code = unclipped_shader.code.replace("uniform float squash_color = 0.0;", "const float squash_color = 0.0;")
		unclipped_shaders[animated_squash] = unclipped_shader
	return unclipped_shaders[animated_squash]

static func _get_clipped_shader(animated_squash: bool) -> Shader:
	if not clipped_shaders.has(animated_squash):
		var clipped_shader := Shader.new()
		clipped_shader.code = """shader_type spatial;
#include "res://Shaders/curve.gdshaderinc"
#include "res://Shaders/tint.gdshaderinc"
uniform float squash_color = 0.0;
varying vec3 world;
varying float intact_shade;
void vertex(){
    READ_TINT;
    world=(MODEL_MATRIX*vec4(VERTEX,1.0)).xyz;
    intact_shade=0.35+0.65*max(dot(normalize(NORMAL),normalize(vec3(-0.45,0.8,0.35))),0.0);
    VERTEX+=curve_offset(MODEL_MATRIX,VERTEX);
}
void fragment(){
    // Past the board edge a prop fades into the tunnel's darkness (road) or the
    // swamp's mist (river) before it is cut, instead of vanishing at |x| = 7.
    float edge=abs(world.x);
    if(edge>7.85)discard;
    float fade=smoothstep(7.0,7.8,edge);
    vec3 murk=world.z<0.0?vec3(0.38,0.5,0.42):vec3(0.0);
    ALBEDO=mix(tint.rgb*(1.0-squash_color),murk,fade);
    EMISSION=tint.rgb*intact_shade*0.9*squash_color*(1.0-fade);
    SPECULAR=0.5*(1.0-squash_color)*(1.0-fade);
    ROUGHNESS=.75;
}"""
		if not animated_squash:
			clipped_shader.code = clipped_shader.code.replace("uniform float squash_color = 0.0;", "const float squash_color = 0.0;")
		clipped_shaders[animated_squash] = clipped_shader
	return clipped_shaders[animated_squash]

func _init(parent: Node, asset: String):
	model_name = asset
	# Frog-owned material uniforms avoid GLES3/WebGL's limited per-instance
	# uniform pool. Other actors continue sharing immutable materials.
	supports_squash_color = asset in ["frog", "lady_frog"]
	if not scenes.has(asset):
		scenes[asset] = load("res://Models/%s.glb" % asset)
	root = (scenes[asset] as PackedScene).instantiate() as Node3D
	var tints := _merge_parts(asset)
	parent.add_child(root)
	_setup_materials(tints)
	animator = _find_node_of_type(root, "AnimationPlayer") as AnimationPlayer
	passenger_socket = root.find_child("PassengerSocket", true, false) as Node3D
	skeleton = _find_node_of_type(root, "Skeleton3D") as Skeleton3D
	play("Idle")
	set_active(false)

## Blender exports one skinned mesh node per palette color, all bound to the
## rig's one skeleton and skin. Each group of such parts (same parent, skin,
## skeleton and placement) becomes one merged node with a single surface, so an
## actor costs one draw call per render pass instead of one per color. The
## merged meshes are built once per model and shared by every actor.
func _merge_parts(asset: String) -> PackedColorArray:
	var groups: Array = []
	for node in root.find_children("*", "MeshInstance3D", true, false):
		var part := node as MeshInstance3D
		if part.mesh == null:
			continue
		var group: Dictionary = {}
		for candidate in groups:
			var first: MeshInstance3D = candidate.parts[0]
			if first.get_parent() == part.get_parent() and first.skin == part.skin and first.skeleton == part.skeleton and first.transform.is_equal_approx(part.transform):
				group = candidate
				break
		if group.is_empty():
			group = {"parts": []}
			groups.append(group)
		group.parts.append(part)
	if not merged_models.has(asset):
		var palette: Array = []
		var meshes: Array = []
		for group in groups:
			var parts: Array = []
			for part in group.parts:
				parts.append_array(TintedMesh.color_parts((part as MeshInstance3D).mesh, palette))
			meshes.append(TintedMesh.merge(parts))
		merged_models[asset] = {"meshes": meshes, "tints": TintedMesh.tints(palette)}
	var merged_model: Dictionary = merged_models[asset]
	for index in range(groups.size()):
		var parts: Array = groups[index].parts
		var first: MeshInstance3D = parts[0]
		var merged := MeshInstance3D.new()
		merged.name = "Merged %s" % first.name if groups.size() > 1 else "Merged"
		merged.mesh = merged_model.meshes[index]
		merged.transform = first.transform
		merged.skin = first.skin
		merged.cast_shadow = first.cast_shadow
		var holder := first.get_parent()
		holder.add_child(merged)
		merged.skeleton = first.skeleton
		for part in parts:
			holder.remove_child(part)
			part.free()
		mesh_instances.append(merged)
	return merged_model.tints

func _setup_materials(tints: PackedColorArray) -> void:
	var id: String = "%s:%s" % [model_name, supports_squash_color]
	if not unclipped_materials.has(id):
		var unclipped := ShaderMaterial.new()
		unclipped.shader = _get_unclipped_shader(supports_squash_color)
		unclipped.set_shader_parameter("tints", tints)
		unclipped_materials[id] = unclipped
		var clipped := ShaderMaterial.new()
		clipped.shader = _get_clipped_shader(supports_squash_color)
		clipped.set_shader_parameter("tints", tints)
		clipped_materials[id] = clipped
	var unclipped_mat: ShaderMaterial = unclipped_materials[id]
	var clipped_mat: ShaderMaterial = clipped_materials[id]
	if supports_squash_color:
		squash_materials[id] = {
			"unclipped": unclipped_mat.duplicate(),
			"clipped": clipped_mat.duplicate()
		}
		unclipped_mat = squash_materials[id]["unclipped"]
		clipped_mat = squash_materials[id]["clipped"]
	for mesh in mesh_instances:
		mesh.set_surface_override_material(0, unclipped_mat)
		surface_bindings.append({
			"mesh": mesh,
			"surface": 0,
			"unclipped": unclipped_mat,
			"clipped": clipped_mat
		})

func set_clipped(clipped: bool) -> void:
	if is_clipped == clipped:
		return
	is_clipped = clipped
	for binding in surface_bindings:
		var mat = binding["clipped"] if clipped else binding["unclipped"]
		binding["mesh"].set_surface_override_material(binding["surface"], mat)

func set_active(active: bool) -> void:
	frame_requested = active
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
	for materials in squash_materials.values():
		# Keep both variants current so crossing a board edge cannot reset shading.
		materials["unclipped"].set_shader_parameter("squash_color", amount)
		materials["clipped"].set_shader_parameter("squash_color", amount)

func animation_name(clip: String) -> StringName:
	if not animation_names.has(clip):
		animation_names[clip] = &""
		for name in animator.get_animation_list():
			if name.to_lower().ends_with(clip.to_lower()):
				animation_names[clip] = StringName(name)
				break
	return animation_names[clip]

func play(clip: String, loop: bool = true, speed: float = 1.0) -> void:
	if animator == null:
		return
	var anim_name := animation_name(clip)
	if anim_name == &"":
		return
	if current_anim == anim_name and animator.is_playing():
		return
	var anim = animator.get_animation(anim_name)
	anim.loop_mode = Animation.LOOP_LINEAR if loop else Animation.LOOP_NONE
	animator.speed_scale = speed
	animator.play(anim_name, 0.035)
	current_anim = anim_name

func pose(clip: String, seconds: float) -> void:
	if animator == null:
		return
	var anim_name := animation_name(clip)
	if anim_name == &"":
		push_error("Missing required clip %s on %s" % [clip, root.name])
		return
	var anim = animator.get_animation(anim_name)
	if current_anim != anim_name:
		animator.play(anim_name, 0.0)
		current_anim = anim_name
	animator.seek(clampf(seconds, 0.0, anim.length), true, true)
	animator.pause()

func _find_node_of_type(n: Node, type_name: String) -> Node:
	if n.is_class(type_name):
		return n
	for child in n.get_children():
		var found = _find_node_of_type(child, type_name)
		if found != null:
			return found
	return null
