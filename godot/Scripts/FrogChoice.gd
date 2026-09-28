extends Button
class_name FrogChoice

var preview_model: Node3D

func _process(delta: float) -> void:
	if is_visible_in_tree() and is_instance_valid(preview_model):
		preview_model.rotate_y(deg_to_rad(36.0) * delta)

# Each card renders a real rigged model in its own small world. Its viewport
# stops rendering when the menu is hidden, and never consumes gameplay input.
func configure(model: String, selected: bool, font: Font) -> void:
	size_flags_horizontal = Control.SIZE_EXPAND_FILL
	custom_minimum_size = Vector2(0, 142)
	tooltip_text = "Play as the pink frog" if model == "lady_frog" else "Play as the green frog"
	var normal := StyleBoxFlat.new()
	normal.bg_color = Color("334937") if selected else Color("253442")
	normal.border_color = Color("b6df56") if selected else Color("617080")
	normal.set_border_width_all(3 if selected else 1)
	normal.set_corner_radius_all(8)
	add_theme_stylebox_override("normal", normal)
	var hover := normal.duplicate() as StyleBoxFlat
	hover.bg_color = Color("466044")
	add_theme_stylebox_override("hover", hover)
	add_theme_stylebox_override("pressed", hover)
	var focus := StyleBoxFlat.new()
	focus.bg_color = Color.TRANSPARENT
	focus.border_color = Color("eee6c9")
	focus.set_border_width_all(3)
	focus.set_corner_radius_all(8)
	add_theme_stylebox_override("focus", focus)
	var container := SubViewportContainer.new()
	container.mouse_filter = Control.MOUSE_FILTER_IGNORE
	container.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	container.offset_left = 4
	container.offset_right = -4
	container.offset_top = 4
	container.offset_bottom = -32
	container.stretch = true
	add_child(container)
	var viewport := SubViewport.new()
	viewport.size = Vector2i(180, 106)
	viewport.own_world_3d = true
	viewport.transparent_bg = true
	viewport.render_target_update_mode = SubViewport.UPDATE_WHEN_VISIBLE
	container.add_child(viewport)
	var world := Node3D.new()
	viewport.add_child(world)
	var environment := WorldEnvironment.new()
	environment.environment = Environment.new()
	environment.environment.background_mode = Environment.BG_COLOR
	environment.environment.background_color = Color.TRANSPARENT
	environment.environment.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
	environment.environment.ambient_light_color = Color.WHITE
	environment.environment.ambient_light_energy = 0.65
	world.add_child(environment)
	var light := DirectionalLight3D.new()
	light.rotation_degrees = Vector3(-45, -35, 0)
	light.light_energy = 1.15
	world.add_child(light)
	var preview_camera := Camera3D.new()
	preview_camera.projection = Camera3D.PROJECTION_ORTHOGONAL
	preview_camera.size = 1.4
	preview_camera.position = Vector3(0.9, 1.05, 1.6)
	world.add_child(preview_camera)
	preview_camera.look_at_from_position(preview_camera.position, Vector3(0, 0.23, 0))
	preview_camera.current = true
	# Previews use the asset's own materials: gameplay clipping/squash shader
	# uniforms are unnecessary here and consume the Web renderer's shared budget.
	var preview: Node3D = load("res://Models/%s.glb" % model).instantiate()
	preview_model = preview
	preview.rotation.y = atan2(preview_camera.position.x, preview_camera.position.z)
	world.add_child(preview)
	for node in preview.find_children("*", "AnimationPlayer", true, false):
		for clip in node.get_animation_list():
			if clip.to_lower().ends_with("idle"):
				node.get_animation(clip).loop_mode = Animation.LOOP_LINEAR
				node.play(clip)
	var caption := Label.new()
	caption.mouse_filter = Control.MOUSE_FILTER_IGNORE
	caption.text = "Pink" if model == "lady_frog" else "Green"
	caption.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	caption.add_theme_font_override("font", font)
	caption.add_theme_font_size_override("font_size", 16)
	caption.add_theme_color_override("font_color", Color("eee6c9"))
	caption.set_anchors_and_offsets_preset(Control.PRESET_BOTTOM_WIDE)
	caption.offset_top = -30
	caption.offset_bottom = -6
	add_child(caption)
