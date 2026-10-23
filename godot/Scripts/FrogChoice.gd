extends Button
class_name FrogChoice

var preview_model: Node3D

const SelectedFace: Color = Color("ffdd00")

func _process(delta: float) -> void:
	if is_visible_in_tree() and is_instance_valid(preview_model):
		preview_model.rotate_y(deg_to_rad(36.0) * delta)

# Each card renders a real rigged model in its own small world. Its viewport
# stops rendering when the menu is hidden, and never consumes gameplay input.
func configure(model: String, selected: bool, font: Font) -> void:
	size_flags_horizontal = Control.SIZE_EXPAND_FILL
	custom_minimum_size = Vector2(0, 150)
	tooltip_text = "Play as the pink frog" if model == "lady_frog" else "Play as the green frog"
	# The chosen frog sits on a sunny yellow candy card with a cream ring; the
	# other on a blue well.
	var face: Color = SelectedFace if selected else UiKit.PanelDeep
	UiKit.style_button(self, face, 16, Vector4(6, 6, 6, 6), 7.0)
	if selected:
		add_theme_stylebox_override("hover", get_theme_stylebox("normal"))
	UiKit.juice(self)
	if selected:
		# Drawn inside the card, so a scroll area never clips it.
		var ring := Panel.new()
		ring.mouse_filter = Control.MOUSE_FILTER_IGNORE
		ring.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
		ring.offset_bottom = -7
		var ring_style := StyleBoxFlat.new()
		ring_style.draw_center = false
		ring_style.border_color = UiKit.Cream
		ring_style.set_border_width_all(4)
		ring_style.set_corner_radius_all(UiKit.Radius)
		ring_style.anti_aliasing = true
		ring.add_theme_stylebox_override("panel", ring_style)
		add_child(ring)
	var container := SubViewportContainer.new()
	container.mouse_filter = Control.MOUSE_FILTER_IGNORE
	container.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	container.offset_left = 6
	container.offset_right = -6
	container.offset_top = 6
	container.offset_bottom = -40
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
	caption.text = "PINK" if model == "lady_frog" else "GREEN"
	caption.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	caption.add_theme_font_override("font", font)
	caption.add_theme_font_size_override("font_size", 18)
	caption.add_theme_color_override("font_color", UiKit.Cream)
	UiKit.outline_label(caption, 7)
	caption.set_anchors_and_offsets_preset(Control.PRESET_BOTTOM_WIDE)
	caption.offset_top = -40
	caption.offset_bottom = -12
	add_child(caption)
