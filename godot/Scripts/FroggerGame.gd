extends Node3D
class_name FroggerGame

const FRAME_SECONDS: float = 33.0 / 2000.0

var simulation: RefCounted = null
var state: Dictionary = {"frame": 0, "ram": [], "video": [], "objects": [], "sounds": []}
var actors: Dictionary = {}
var input_pulse: InputPulse = InputPulse.new()
var audio_player: AudioStreamPlayer = null
var audio_playback: AudioStreamGeneratorPlayback = null

var started: bool = false
var paused: bool = false
var modern: bool = true
var muted: bool = false
var fullscreen: bool = true
var shadows_enabled: bool = true
var antialiasing_enabled: bool = true
var accumulator: float = 0.0
var facing: int = 2
var coin_frames: int = 0
var render_frames: int = 0
var high_score: int = 0
var screenshot: String = ""
var water: ShaderMaterial = null
var camera: Camera3D = null
var sun: DirectionalLight3D = null

# Camera options
var perspective_view: bool = true
var follow_camera: bool = true
var camera_look_target: Vector3 = Vector3(0.0, 0.0, -0.12)

# Presentation helpers
var frog_visual: FrogVisualState = FrogVisualState.new()
var lady_visual: BoardVisuals.LadyFrogPresentation = BoardVisuals.LadyFrogPresentation.new()
var home_arrival: FrogVisualState.HomeArrivalVisual = FrogVisualState.HomeArrivalVisual.new()
var moving_visuals: PresentationMotion = PresentationMotion.new()
var frog_motion: PresentationMotion = PresentationMotion.new(4, 0.35, 72.0)
var presentation_delta: float = 0.0
var displayed_frog_x: float = 0.0
var displayed_frog_row: float = 0.0

class BonusPopup:
	var award: Dictionary
	var view: Control
	var anchor_uv: Vector2
	var display_text: String
	func _init(a: Dictionary, v: Control, uv: Vector2, text: String):
		award = a
		view = v
		anchor_uv = uv
		display_text = text

var popups: Array = []
var death_ripple: MeshInstance3D = null
var ripple_material: StandardMaterial3D = null
var known_diving_groups: Dictionary = {}
var turtle_supports: Array = []
var snake_facing: Dictionary = {}
var home_gator_visuals: Array = []
var anchored_death_frame: int = -1
var last_live_player_frame: int = -1
var death_initial_height: float = 0.0
var last_live_player_height: float = 0.0
var last_live_player_x: float = 0.0
var last_live_player_row: float = 0.0
var known_diving_level: int = -1
var lady_facing: int = 2
var lady_hop_start_frame: int = -1
var lady_last_motion_frame: int = -1

const LadyInRiverScale: float = 0.62
const PassengerScale: float = 0.76

# UI elements
var score_label: Label = null
var high_label: Label = null
var level_label: Label = null
var lives_label: Label = null
var message_label: Label = null
var player_header: Label = null
var timer_bar: ProgressBar = null
var menu: PanelContainer = null
var message_panel: PanelContainer = null
var menu_items: VBoxContainer = null
var bonus_overlay: Control = null
var display_font: Font = null
var body_font: Font = null

const Cream: Color = Color("eee6c9")
const RedColor: Color = Color("ff6655")
const LimeColor: Color = Color("b6df56")

# Review harness
var review: String = ""
var review_frame: int = 0
var review_capture_pending: bool = false
var review_close: bool = false
var review_lady_close: bool = false
var review_turtle_close: bool = false
var review_gator_close: bool = false
var review_snake_close: bool = false
var review_lady_clear_visible: bool = false
var review_lady_patrol_visible: bool = false
var review_gator_safe_observed: bool = false
var review_gator_snout_death_observed: bool = false
var review_measurements: Array = []

func _init():
	home_gator_visuals = [
		BoardVisuals.HomeGatorVisual.new(),
		BoardVisuals.HomeGatorVisual.new(),
		BoardVisuals.HomeGatorVisual.new(),
		BoardVisuals.HomeGatorVisual.new(),
		BoardVisuals.HomeGatorVisual.new()
	]

func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--screenshot="):
			screenshot = arg.substr(13)
	read_review_args()
	setup_world()
	setup_ui()
	load_preferences()
	reset_machine()
	for i in range(180):
		simulation.step()
	observe_frame()
	show_menu(false)
	var args = OS.get_cmdline_user_args()
	if screenshot != "" and not args.has("--menu-shot"):
		start_game(1)
		for i in range(50):
			simulation.step()
		observe_frame()
	if screenshot != "" and args.has("--later-board"):
		for round_idx in range(2):
			for bay in range(5):
				simulation.poke(0x8044, 24 + 48 * bay)
				simulation.poke(0x8047, 32)
				simulation.poke(0x8004, 0)
				simulation.poke(0x83cd, 0)
				simulation.poke(0x8122, 1)
				for f in range(180):
					simulation.step()
			for f in range(500):
				simulation.step()
		observe_frame()
		simulation.clear_sound_samples()

func reset_machine() -> void:
	var main_rom = FileAccess.get_file_as_bytes("res://rom/maincpu.bin")
	var sound_rom = FileAccess.get_file_as_bytes("res://rom/audiocpu.bin")
	simulation = ClassDB.instantiate("ArcadeSimulation")
	simulation.setup(main_rom, modern, sound_rom)
	accumulator = 0.0
	input_pulse.reset()
	clear_presentation()

func start_game(players: int) -> void:
	reset_machine()
	for i in range(180):
		simulation.step()
	for c in range(players):
		for i in range(6):
			simulation.step(16)
		for i in range(10):
			simulation.step()
	for i in range(6):
		simulation.step(32 if players == 1 else 64)
	for i in range(45):
		simulation.step()
	if high_score > 0:
		var n: int = int(high_score / 10)
		simulation.poke(0x83ef, (n % 10) | (((n / 10) % 10) << 4))
		simulation.poke(0x83f0, (((n / 100) % 10)) | (((n / 1000) % 10) << 4))
	observe_frame()
	simulation.clear_sound_samples()
	if audio_player != null:
		audio_player.stop()
	started = true
	paused = false
	menu.visible = false
	Input.mouse_mode = Input.MOUSE_MODE_HIDDEN
	message_label.text = ""
	message_panel.visible = false

func _process(delta: float) -> void:
	if simulation == null:
		return
	presentation_delta = clampf(delta, 0.0, 0.1)
	if review != "":
		if review_capture_pending:
			return
		step_review()
	elif not paused:
		accumulator = minf(accumulator + delta, 0.066)
		var steps: int = 0
		while accumulator >= FRAME_SECONDS and steps < 4:
			steps += 1
			var input_val: int = read_direction() if started else 0
			if coin_frames > 0:
				input_val |= 16
				coin_frames -= 1
			simulation.step(input_val)
			observe_frame()
			accumulator -= FRAME_SECONDS
	update_frog_motion()
	update_camera(delta)
	feed_audio()
	update_actors()
	update_hud()
	if water != null:
		water.set_shader_parameter("clock", float(state.get("frame", 0)) * FRAME_SECONDS)
	update_review_camera()
	if review != "":
		capture_review()
	elif screenshot != "":
		render_frames += 1
		if render_frames == 10:
			capture_screenshot()

func capture_screenshot() -> void:
	await RenderingServer.frame_post_draw
	var img = get_viewport().get_texture().get_image()
	var error = img.save_png(screenshot)
	print("SCREENSHOT %s %s" % [screenshot, error])
	get_tree().quit(0 if error == OK else 1)

func read_direction() -> int:
	var pads = Input.get_connected_joypads()
	var joy: int = pads[0] if pads.size() > 0 else -1
	var digital: int = 0
	if Input.is_physical_key_pressed(KEY_UP) or Input.is_physical_key_pressed(KEY_W) or (joy >= 0 and Input.is_joy_button_pressed(joy, JOY_BUTTON_DPAD_UP)):
		digital |= 1
	if Input.is_physical_key_pressed(KEY_DOWN) or Input.is_physical_key_pressed(KEY_S) or (joy >= 0 and Input.is_joy_button_pressed(joy, JOY_BUTTON_DPAD_DOWN)):
		digital |= 2
	if Input.is_physical_key_pressed(KEY_LEFT) or Input.is_physical_key_pressed(KEY_A) or (joy >= 0 and Input.is_joy_button_pressed(joy, JOY_BUTTON_DPAD_LEFT)):
		digital |= 4
	if Input.is_physical_key_pressed(KEY_RIGHT) or Input.is_physical_key_pressed(KEY_D) or (joy >= 0 and Input.is_joy_button_pressed(joy, JOY_BUTTON_DPAD_RIGHT)):
		digital |= 8
	var x: float = Input.get_joy_axis(joy, JOY_AXIS_LEFT_X) if joy >= 0 else 0.0
	var y: float = Input.get_joy_axis(joy, JOY_AXIS_LEFT_Y) if joy >= 0 else 0.0
	return handle_movement_press(input_pulse.from_inputs(digital, x, y))

func adapt_direction(held_mask: int) -> int:
	return handle_movement_press(input_pulse.from_held_mask(held_mask))

func handle_movement_press(press: int) -> int:
	if press != 0 and home_arrival.active(state.get("frame", 0), render_fraction()):
		home_arrival.cancel()
	return press

func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventKey and event.pressed and not event.echo:
		match event.physical_keycode:
			KEY_ENTER, KEY_SPACE:
				if not started or BoardVisuals.at(state, 0x83fe) == 0:
					start_game(1)
				elif paused:
					resume_game()
			KEY_1:
				start_game(1)
			KEY_2:
				start_game(2)
			KEY_ESCAPE, KEY_P:
				if started:
					paused = not paused
					if paused:
						show_menu(true)
					else:
						resume_game()
			KEY_C, KEY_5:
				coin_frames = 6
			KEY_R:
				if started:
					start_game(1)
			KEY_M:
				muted = not muted
				save_preferences()
			KEY_F11:
				fullscreen = not fullscreen
				apply_fullscreen()
				save_preferences()
	if event is InputEventJoypadButton and event.pressed:
		if event.button_index == JOY_BUTTON_A and (not started or BoardVisuals.at(state, 0x83fe) == 0):
			start_game(1)
		elif event.button_index == JOY_BUTTON_START:
			if not started:
				start_game(1)
			else:
				paused = not paused
				if paused:
					show_menu(true)
				else:
					resume_game()

func resume_game() -> void:
	paused = false
	menu.visible = false
	Input.mouse_mode = Input.MOUSE_MODE_HIDDEN
	accumulator = 0.0

func actor(key: String, model: String) -> ModelActor:
	if not actors.has(key):
		actors[key] = ModelActor.new(self, model)
	return actors[key]

static func pos3(x: float, row: float, height: float = 0.0) -> Vector3:
	return Vector3((x - 120.0) / 16.0, height, (row - 128.0) / 16.0)

func setup_world() -> void:
	var is_web: bool = OS.has_feature("web")
	var env := Environment.new()
	env.background_mode = Environment.BG_COLOR
	env.background_color = Color("171e29")
	env.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
	env.ambient_light_color = Color.WHITE
	env.ambient_light_energy = 0.30
	env.tonemap_mode = Environment.TONE_MAPPER_LINEAR
	if is_web:
		env.ssao_enabled = false
	else:
		env.ssao_enabled = true
		env.ssao_intensity = 1.45
		env.ssao_radius = 1.0
		env.ssao_sharpness = 0.55
	env.reflected_light_source = Environment.REFLECTION_SOURCE_DISABLED
	var world_env := WorldEnvironment.new()
	world_env.environment = env
	add_child(world_env)

	sun = DirectionalLight3D.new()
	sun.light_color = Color.WHITE
	sun.light_energy = 1.05
	sun.light_angular_distance = 1.5
	sun.shadow_enabled = shadows_enabled
	if is_web:
		sun.directional_shadow_mode = DirectionalLight3D.SHADOW_ORTHOGONAL
		sun.directional_shadow_blend_splits = false
		sun.shadow_blur = 0.6
		sun.directional_shadow_max_distance = 28.0
	else:
		sun.shadow_blur = 1.8
		sun.directional_shadow_blend_splits = true
	add_child(sun)
	sun.look_at(Vector3(1.4, -2.0, 1.0), Vector3.UP)

	var fill_light := DirectionalLight3D.new()
	fill_light.rotation_degrees = Vector3(-40, 140, 0)
	fill_light.light_color = Color("e8f0ff")
	fill_light.light_energy = 0.08
	add_child(fill_light)

	var board_scene = load("res://Models/board.glb")
	add_child(board_scene.instantiate())

	camera = Camera3D.new()
	camera.projection = Camera3D.PROJECTION_ORTHOGONAL
	camera.size = 16.8
	camera.position = Vector3(0, 19, 9.8)
	camera.keep_aspect = Camera3D.KEEP_HEIGHT
	add_child(camera)
	camera.look_at(Vector3(0, 0, -0.12))
	camera.current = true

	var surface := MeshInstance3D.new()
	var plane := PlaneMesh.new()
	plane.size = Vector2(14, 5)
	plane.subdivide_width = 64
	plane.subdivide_depth = 32
	surface.mesh = plane
	surface.position = Vector3(0, -0.025, -3)
	water = ShaderMaterial.new()
	water.shader = load("res://Shaders/river.gdshader")
	surface.material_override = water
	add_child(surface)

func feed_audio() -> void:
	if audio_player == null:
		audio_player = AudioStreamPlayer.new()
		var gen := AudioStreamGenerator.new()
		gen.mix_rate = 48000
		gen.buffer_length = 0.08
		audio_player.stream = gen
		audio_player.volume_db = -9.0
		add_child(audio_player)
	if simulation == null:
		return
	var sample_count: int = simulation.get_sound_sample_count()
	if muted or paused or not started:
		simulation.clear_sound_samples()
		if audio_player.playing:
			audio_player.stop()
		return
	if not audio_player.playing:
		audio_player.play()
		audio_playback = audio_player.get_stream_playback() as AudioStreamGeneratorPlayback
	if audio_playback == null:
		return
	var available: int = audio_playback.get_frames_available()
	var n: int = mini(sample_count, available)
	if n > 0:
		if simulation.has_method("get_stereo_sound_samples"):
			var buf = simulation.get_stereo_sound_samples(n)
			if buf.size() > 0:
				audio_playback.push_buffer(buf)
		else:
			var raw = simulation.get_sound_samples(n)
			var buf := PackedVector2Array()
			buf.resize(raw.size())
			for i in range(raw.size()):
				var v: float = raw[i]
				buf[i] = Vector2(v, v)
			audio_playback.push_buffer(buf)

func apply_fullscreen() -> void:
	DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_FULLSCREEN if fullscreen else DisplayServer.WINDOW_MODE_WINDOWED)

func apply_antialiasing() -> void:
	get_viewport().msaa_3d = Viewport.MSAA_2X if antialiasing_enabled else Viewport.MSAA_DISABLED

func load_preferences() -> void:
	if screenshot != "":
		muted = true
		return
	var c := ConfigFile.new()
	if c.load("user://settings.cfg") == OK:
		high_score = int(c.get_value("play", "high_score", 0))
		muted = bool(c.get_value("play", "muted", false))
		shadows_enabled = bool(c.get_value("play", "shadows_enabled", true))
		antialiasing_enabled = bool(c.get_value("play", "antialiasing_enabled", true))
		var version: int = int(c.get_value("play", "defaults_version", 0))
		var defaults = GameDefaults.from_stored(version,
			bool(c.get_value("play", "modern_collision", true)),
			bool(c.get_value("play", "perspective_view", true)),
			bool(c.get_value("play", "follow_camera", true)),
			bool(c.get_value("play", "fullscreen", true)))
		modern = defaults["modern"]
		perspective_view = defaults["perspective"]
		follow_camera = defaults["follow"]
		fullscreen = defaults["fullscreen"]
		if version < GameDefaults.PreferencesVersion:
			save_preferences()
	else:
		save_preferences()
	apply_fullscreen()
	apply_antialiasing()
	if sun != null:
		sun.shadow_enabled = shadows_enabled

func save_preferences() -> void:
	if screenshot != "":
		return
	var c := ConfigFile.new()
	c.set_value("play", "defaults_version", GameDefaults.PreferencesVersion)
	c.set_value("play", "high_score", high_score)
	c.set_value("play", "modern_collision", modern)
	c.set_value("play", "muted", muted)
	c.set_value("play", "shadows_enabled", shadows_enabled)
	c.set_value("play", "antialiasing_enabled", antialiasing_enabled)
	c.set_value("play", "perspective_view", perspective_view)
	c.set_value("play", "follow_camera", follow_camera)
	c.set_value("play", "fullscreen", fullscreen)
	c.save("user://settings.cfg")

func style_box(color: Color, radius: int = 12, border: int = 0) -> StyleBoxFlat:
	var s := StyleBoxFlat.new()
	s.bg_color = color
	s.corner_radius_top_left = radius
	s.corner_radius_top_right = radius
	s.corner_radius_bottom_left = radius
	s.corner_radius_bottom_right = radius
	s.content_margin_left = 22
	s.content_margin_right = 22
	s.content_margin_top = 14
	s.content_margin_bottom = 14
	s.set_border_width_all(border)
	s.border_color = Color("70614e")
	return s

func edge_panel_style(color: Color, at_top: bool) -> StyleBoxFlat:
	var s = style_box(color, 0, 2)
	if at_top:
		s.corner_radius_bottom_left = 16
		s.corner_radius_bottom_right = 16
	else:
		s.corner_radius_top_left = 16
		s.corner_radius_top_right = 16
	return s

func make_text(text_val: String, size_val: int, color_val: Color) -> Label:
	var l := Label.new()
	l.text = text_val
	l.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	l.add_theme_font_size_override("font_size", size_val)
	l.add_theme_font_override("font", display_font if size_val >= 23 else body_font)
	l.add_theme_color_override("font_color", color_val)
	if size_val >= 23:
		l.add_theme_color_override("font_shadow_color", Color(0, 0, 0, 0.45))
		l.add_theme_constant_override("shadow_offset_y", 2)
	return l

func setup_ui() -> void:
	display_font = load("res://Fonts/PressStart2P-Regular.ttf")
	body_font = load("res://Fonts/Silkscreen-Regular.ttf")
	var ui := CanvasLayer.new()
	add_child(ui)
	var root := Control.new()
	ui.add_child(root)
	root.mouse_filter = Control.MOUSE_FILTER_IGNORE

	var layout_root = func():
		var size: Vector2 = get_viewport().get_visible_rect().size
		root.size = Vector2(1100, size.y)
		root.position = Vector2((size.x - 1100) / 2.0, 0)
	layout_root.call()
	get_viewport().size_changed.connect(layout_root)

	var th := Theme.new()
	th.default_font = body_font
	th.default_font_size = 17
	root.theme = th

	var frame_color := Color("202733")
	var header := PanelContainer.new()
	header.position = Vector2(115, 0)
	header.size = Vector2(870, 94)
	header.add_theme_stylebox_override("panel", edge_panel_style(frame_color, true))
	root.add_child(header)

	var row := HBoxContainer.new()
	row.alignment = BoxContainer.ALIGNMENT_CENTER
	row.add_theme_constant_override("separation", 110)
	header.add_child(row)

	for title in ["1-UP", "FROGGER", "HI-SCORE"]:
		var col := VBoxContainer.new()
		col.alignment = BoxContainer.ALIGNMENT_CENTER
		row.add_child(col)
		var heading = make_text(title, 23 if title.contains("F") else 16, Cream)
		col.add_child(heading)
		if title == "1-UP":
			player_header = heading
			score_label = make_text("00000", 26, RedColor)
			col.add_child(score_label)
		elif title == "HI-SCORE":
			high_label = make_text("00000", 26, RedColor)
			col.add_child(high_label)
		else:
			level_label = make_text("THE GREAT CROSSING", 12, LimeColor)
			col.add_child(level_label)

	var bottom := PanelContainer.new()
	bottom.size = Vector2(870, 64)
	bottom.add_theme_stylebox_override("panel", edge_panel_style(frame_color, false))
	root.add_child(bottom)

	var foot := HBoxContainer.new()
	foot.alignment = BoxContainer.ALIGNMENT_CENTER
	foot.add_theme_constant_override("separation", 26)
	bottom.add_child(foot)

	lives_label = make_text("FROGS   ●●●", 19, LimeColor)
	lives_label.custom_minimum_size = Vector2(225, 0)
	foot.add_child(lives_label)

	timer_bar = ProgressBar.new()
	timer_bar.min_value = 0
	timer_bar.max_value = 100
	timer_bar.value = 100
	timer_bar.show_percentage = false
	timer_bar.custom_minimum_size = Vector2(330, 18)
	timer_bar.size_flags_vertical = Control.SIZE_SHRINK_CENTER
	timer_bar.add_theme_stylebox_override("background", style_box(Color("121a29"), 4))
	timer_bar.add_theme_stylebox_override("fill", style_box(Color("9ac75d"), 4))
	foot.add_child(timer_bar)
	foot.add_child(make_text("TIME", 19, Cream))

	message_panel = PanelContainer.new()
	message_panel.visible = false
	message_panel.mouse_filter = Control.MOUSE_FILTER_IGNORE
	var msg_style = style_box(Color(0.06, 0.10, 0.17, 0.93), 12, 2)
	msg_style.border_color = Color("ffce57")
	message_panel.add_theme_stylebox_override("panel", msg_style)
	root.add_child(message_panel)

	message_label = make_text("", 30, Cream)
	message_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	message_label.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	message_label.mouse_filter = Control.MOUSE_FILTER_IGNORE
	message_label.add_theme_color_override("font_shadow_color", Color.BLACK)
	message_label.add_theme_constant_override("shadow_offset_y", 3)
	message_panel.add_child(message_label)

	menu = PanelContainer.new()
	menu.size = Vector2(440, 484)
	menu.add_theme_stylebox_override("panel", style_box(Color(0.075, 0.10, 0.15, 0.97), 18, 2))
	root.add_child(menu)

	menu_items = VBoxContainer.new()
	menu_items.add_theme_constant_override("separation", 14)
	menu.add_child(menu_items)

	var layout_overlay = func():
		var viewport_size = get_viewport().get_visible_rect().size
		var h: float = root.size.y
		bottom.position = Vector2(115, h - 64)
		var message_width: float = clampf(viewport_size.x - 48.0, 120.0, 840.0)
		message_panel.size = Vector2(message_width, 180)
		message_panel.position = Vector2((1100.0 - message_width) / 2.0, (h - 180.0) / 2.0)
		message_label.custom_minimum_size = Vector2(message_width - 44.0, 150.0)
		message_label.add_theme_font_size_override("font_size", 18 if viewport_size.x < 620 else (24 if viewport_size.x < 900 else 30))
		menu.position = Vector2(330, (h - 484.0) / 2.0)
	layout_overlay.call()
	get_viewport().size_changed.connect(layout_overlay)

	bonus_overlay = Control.new()
	bonus_overlay.size = get_viewport().get_visible_rect().size
	bonus_overlay.mouse_filter = Control.MOUSE_FILTER_IGNORE
	ui.add_child(bonus_overlay)
	get_viewport().size_changed.connect(func(): bonus_overlay.size = get_viewport().get_visible_rect().size)

func show_menu(resume: bool) -> void:
	menu.visible = true
	Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
	for child in menu_items.get_children():
		menu_items.remove_child(child)
		child.queue_free()
	if resume:
		menu_items.add_child(make_text("PAUSED", 23, Cream))
		menu_items.add_child(make_text("The crossing can wait.", 15, LimeColor))
		add_button("RESUME", resume_game)
	add_button("NEW GAME" if resume else "ONE PLAYER", func(): start_game(1))
	if not resume:
		add_button("TWO PLAYERS · TAKE TURNS", func(): start_game(2))

	var collision := CheckButton.new()
	collision.text = "Classic collision (original ROM)"
	collision.button_pressed = not modern
	collision.toggled.connect(func(val):
		modern = not val
		if simulation != null:
			simulation.modern(modern)
		save_preferences())
	menu_items.add_child(collision)

	var perspective := CheckButton.new()
	perspective.text = "Perspective view"
	perspective.button_pressed = perspective_view
	perspective.toggled.connect(func(val):
		perspective_view = val
		save_preferences())
	menu_items.add_child(perspective)

	var follow := CheckButton.new()
	follow.text = "Follow frog (closer view)"
	follow.button_pressed = follow_camera
	follow.toggled.connect(func(val):
		follow_camera = val
		save_preferences())
	menu_items.add_child(follow)

	var shadows_btn := CheckButton.new()
	shadows_btn.text = "Shadows"
	shadows_btn.button_pressed = shadows_enabled
	shadows_btn.toggled.connect(func(val):
		shadows_enabled = val
		if sun != null:
			sun.shadow_enabled = shadows_enabled
		save_preferences())
	menu_items.add_child(shadows_btn)

	var aa_btn := CheckButton.new()
	aa_btn.text = "Antialiasing"
	aa_btn.button_pressed = antialiasing_enabled
	aa_btn.toggled.connect(func(val):
		antialiasing_enabled = val
		apply_antialiasing()
		save_preferences())
	menu_items.add_child(aa_btn)

	var sound_btn := CheckButton.new()
	sound_btn.text = "Sound"
	sound_btn.button_pressed = not muted
	sound_btn.toggled.connect(func(val):
		muted = not val
		save_preferences())
	menu_items.add_child(sound_btn)

	menu_items.add_child(make_text("Arrow keys / WASD / D-pad to hop\nReach all five homes. Avoid cars and open water.", 13, Color("adb8cc")))

func add_button(text_val: String, action: Callable) -> void:
	var b := Button.new()
	b.text = text_val
	b.custom_minimum_size = Vector2(350, 43)
	b.add_theme_stylebox_override("normal", style_box(Color("526e3e"), 7))
	b.add_theme_stylebox_override("hover", style_box(Color("6f914d"), 7))
	b.pressed.connect(action)
	menu_items.add_child(b)
	var btn_count: int = 0
	for child in menu_items.get_children():
		if child is Button:
			btn_count += 1
	if btn_count == 1:
		b.grab_focus()

func update_hud() -> void:
	var player: int = 2 if BoardVisuals.at(state, 0x83fd) == 2 else 1
	player_header.text = "%d-UP" % player
	var current_score: int = bcd_score(0x83ed if player == 1 else 0x83eb)
	score_label.text = "%05d" % current_score
	var best: int = maxi(bcd_score(0x83ef), maxi(bcd_score(0x83ed), bcd_score(0x83eb)))
	if best > high_score:
		high_score = best
		save_preferences()
	high_label.text = "%05d" % high_score
	level_label.text = "PLAYER %d     •     LEVEL %02d" % [player, maxi(1, BoardVisuals.at(state, 0x83b7))]
	var count: int = BoardVisuals.at(state, 0x83e5 if player == 1 else 0x83e6)
	var circles: String = ""
	for i in range(clampi(count, 0, 12)):
		circles += "●"
	lives_label.text = "FROGS   %s" % circles
	timer_bar.value = clampf(float(BoardVisuals.at(state, 0x83dd)) / 60.0, 0.0, 1.0) * 100.0
	if started and not paused:
		if BoardVisuals.at(state, 0x83fe) == 0:
			message_label.text = "GAME OVER\nPress Enter or controller A to restart"
		elif BoardVisuals.at(state, 0x8297) > 0 and BoardVisuals.at(state, 0x842f) >= 5:
			message_label.text = "ALL FROGS HOME!"
		else:
			message_label.text = ""
	message_panel.visible = message_label.text.length() > 0

func bcd_score(addr: int) -> int:
	var low: int = BoardVisuals.at(state, addr)
	var high: int = BoardVisuals.at(state, addr + 1)
	return 10 * ((low & 15) + 10 * (low >> 4) + 100 * (high & 15) + 1000 * (high >> 4))

func update_camera(delta: float) -> void:
	var view_size: Vector2 = get_viewport().get_visible_rect().size
	var aspect: float = maxf(0.55, view_size.x / maxf(1.0, view_size.y))
	camera.projection = Camera3D.PROJECTION_PERSPECTIVE if perspective_view else Camera3D.PROJECTION_ORTHOGONAL
	camera.size = 9.8 if (follow_camera and started) else maxf(16.8, 16.2 / aspect)
	camera.fov = 54.0 if (follow_camera and started) else 52.0

	var tracking: bool = follow_camera and started and FrogVisualState.player_on_board(state)
	var tx: float = 0.0
	var tz: float = 0.0
	if tracking:
		var x: float = float(frog_visual.death_x) if frog_visual.dying else displayed_frog_x
		var row: float = float(frog_visual.death_row) if frog_visual.dying else displayed_frog_row
		var p: Vector3 = pos3(x, row)
		tx = clampf(p.x, -2.5, 2.5)
		tz = clampf(p.z, -4.0, 4.0)
	elif follow_camera and started:
		tx = clampf(camera.position.x, -2.5, 2.5)
		tz = clampf(camera.position.z - (5.8 if perspective_view else 6.5), -4.0, 4.0)
	var desired: Vector3
	if tracking or (follow_camera and started):
		desired = Vector3(tx, 9.0 if perspective_view else 11.2, tz + (5.8 if perspective_view else 6.5))
	else:
		desired = Vector3(0, 15.5, 9.5) if perspective_view else Vector3(0, 19, 9.8)
	var responsiveness: float = 1.0 - exp(-6.0 * minf(delta, 0.1))
	camera.position = camera.position.lerp(desired, responsiveness)
	var desired_look: Vector3 = Vector3(tx, 0, tz - 0.80) if (tracking or (follow_camera and started)) else Vector3(0, 0, -0.12)
	camera_look_target = camera_look_target.lerp(desired_look, responsiveness)
	camera.look_at(camera_look_target)

func observe_frame() -> void:
	state = simulation.snapshot()
	frog_visual.observe(state)
	lady_visual.observe(state, func(addr): return simulation.peek(addr))
	track_lady_hop()

func clear_presentation() -> void:
	frog_visual.reset()
	lady_visual.reset()
	home_arrival.reset()
	moving_visuals.reset()
	frog_motion.reset()
	snake_facing.clear()
	for gator in home_gator_visuals:
		gator.reset()
	facing = 2
	lady_facing = 2
	lady_hop_start_frame = -1
	lady_last_motion_frame = -1
	anchored_death_frame = -1
	last_live_player_frame = -1
	known_diving_groups.clear()
	known_diving_level = -1
	for p in popups:
		p.view.queue_free()
	popups.clear()

func render_fraction() -> float:
	return 0.0 if paused else clampf(accumulator / FRAME_SECONDS, 0.0, 1.0)

func update_frog_motion() -> void:
	displayed_frog_x = frog_motion.step(0, float(BoardVisuals.at(state, 0x8044)), state.get("frame", 0), presentation_delta, paused)
	displayed_frog_row = frog_motion.step(1, float(BoardVisuals.at(state, 0x8047)), state.get("frame", 0), presentation_delta, paused)

func track_lady_hop() -> void:
	if BoardVisuals.at(state, 0x8135) == 0 or frog_visual.carrying:
		return
	var code: int = BoardVisuals.at(state, 0x8041)
	var direction: int = 3 if code == 0x21 else (1 if code == 0xa1 else 0)
	if lady_facing == 2:
		lady_facing = 1 if (BoardVisuals.at(state, 0x833d) & 0x80) != 0 else 3
	if direction == 0:
		return
	var cur_frame: int = state.get("frame", 0)
	if lady_hop_start_frame == 0 or direction != lady_facing or cur_frame - lady_hop_start_frame >= 12:
		lady_hop_start_frame = cur_frame
	lady_facing = direction
	lady_last_motion_frame = cur_frame

func update_actors() -> void:
	var fraction: float = render_fraction()
	for a in actors.values():
		a.set_active(false)
	update_bonuses(fraction)
	update_lanes(fraction)
	update_player(fraction)
	update_homes_and_hazards()

func update_player(fraction: float) -> void:
	var player = actor("player", "frog")
	player.root.scale = Vector3.ONE
	player.set_squash_color(0.0)
	var cur_frame: int = state.get("frame", 0)
	var holding_home: bool = home_arrival.active(cur_frame, fraction)
	player.set_active(holding_home or FrogVisualState.player_on_board(state))
	var hop: int = frog_visual.hop_direction
	if frog_visual.hop_active(cur_frame):
		match hop:
			1: facing = 0
			2: facing = 2
			3: facing = 1
			_: facing = 3
	player.root.rotation = Vector3(0, facing * PI / 2.0, 0)
	if holding_home:
		var finishing: bool = home_arrival.finishing_hop(cur_frame, fraction)
		var home_row: float = home_arrival.visual_row(cur_frame, fraction)
		player.root.position = pos3(home_arrival.visual_x(cur_frame, fraction), home_row, BoardVisuals.surface_height(home_row) if finishing else 0.08)
		player.root.rotation = Vector3(0, PI, 0)
		if finishing:
			player.pose("Hop", home_arrival.hop_pose_seconds(cur_frame, fraction))
		else:
			player.play("Celebrate")
	elif frog_visual.dying:
		var age: float = frog_visual.death_seconds(cur_frame, fraction)
		if anchored_death_frame != frog_visual.death_frame:
			death_initial_height = BoardVisuals.surface_height(float(frog_visual.death_row))
			if frog_visual.drowning:
				death_initial_height -= turtle_ride_depth(float(frog_visual.death_x), float(frog_visual.death_row))
				if last_live_player_frame >= frog_visual.death_frame - 2 and absf(last_live_player_x - float(frog_visual.death_x)) <= 12.0 and absf(last_live_player_row - float(frog_visual.death_row)) <= 8.0:
					death_initial_height = minf(death_initial_height, last_live_player_height)
			anchored_death_frame = frog_visual.death_frame
		var h: float = death_initial_height
		if frog_visual.drowning:
			h = FrogVisualState.drown_height(BoardVisuals.surface_height(float(frog_visual.death_row)), death_initial_height, age)
		else:
			var squash: float = FrogVisualState.squash_progress(age)
			if frog_visual.death_row <= 144 or frog_visual.death_row >= 208:
				h = lerpf(h, maxf(h, 0.09), squash)
			player.root.scale = Vector3(1.0 + 0.50 * squash, 1.0 - 0.92 * squash, 1.0 + 0.40 * squash)
			player.set_squash_color(squash)
		player.root.position = pos3(float(frog_visual.death_x), float(frog_visual.death_row), h)
		player.pose("Drown" if frog_visual.drowning else "Squash", age)
	else:
		var x: float = displayed_frog_x
		var row: float = displayed_frog_row
		player.root.position = pos3(x, row, BoardVisuals.surface_height(row) - turtle_ride_depth(x, row))
		last_live_player_height = player.root.position.y
		last_live_player_x = x
		last_live_player_row = row
		last_live_player_frame = cur_frame
		if frog_visual.hop_active(cur_frame):
			player.pose("Hop", frog_visual.hop_seconds(cur_frame, fraction))
		else:
			player.play("Idle")

	if frog_visual.carrying or (holding_home and home_arrival.passenger):
		if player.passenger_socket == null:
			push_error("The Blender frog rig has no PassengerSocket")
		var passenger: ModelActor
		if not actors.has("passenger"):
			passenger = ModelActor.new(player.passenger_socket, "lady_frog")
			actors["passenger"] = passenger
		else:
			passenger = actors["passenger"]
		passenger.set_active(true)
		passenger.root.position = Vector3.ZERO
		passenger.root.scale = Vector3.ONE * PassengerScale
		passenger.root.global_rotation = Vector3(0, player.root.global_rotation.y, 0)
		if holding_home:
			if home_arrival.finishing_hop(cur_frame, fraction):
				passenger.pose("Hop", home_arrival.hop_pose_seconds(cur_frame, fraction))
			else:
				passenger.play("Celebrate")
		elif frog_visual.hop_active(cur_frame):
			passenger.pose("Hop", frog_visual.hop_seconds(cur_frame, fraction))
		else:
			passenger.play("Idle")
	update_death_ripple(fraction)

func update_lanes(fraction: float) -> void:
	turtle_supports.clear()
	var widths: Array = [60, 31, 92, 44, 47, 0, 34, 18, 18, 18, 18]
	var models: Array = ["log", "turtle", "log", "log", "turtle", "", "truck", "sport", "car", "dozer", "racecar"]
	var level_val: int = BoardVisuals.at(state, 0x83b7)
	if known_diving_level != level_val:
		known_diving_groups.clear()
		known_diving_level = level_val
	var cur_frame: int = state.get("frame", 0)
	for lane in range(11):
		if lane == 5:
			continue
		var table: int = 0x8100 + lane * 9
		var count: int = mini(8, BoardVisuals.at(state, table))
		var row: int = (lane + 3) * 16
		var width: int = widths[lane]
		var is_turtle: bool = models[lane] == "turtle"
		for index in range(count):
			var raw_center: float = float(BoardVisuals.at(state, table + index + 1) - (12 if lane < 5 else 3)) - float(width) / 2.0
			var center: float = moving_visuals.step(lane * 16 + index, raw_center, cur_frame, presentation_delta, paused)
			var members: int = 2 if lane == 1 else (3 if lane == 4 else 1)
			var group_key: String = "%d_%d" % [lane, index]
			if is_turtle and BoardVisuals.turtle_phase(state, raw_center, row) > 0:
				known_diving_groups[group_key] = true
			var depth: float = BoardVisuals.turtle_depth(state, raw_center, row, fraction, known_diving_groups.has(group_key)) if is_turtle else 0.0
			var crocodile: bool = (lane == 0 and index == 0 and BoardVisuals.river_gator_active(state))
			for member in range(members):
				for wrap in [-1, 0, 1]:
					var x: float = center + float(wrap) * 256.0 + (float(member) - float(members - 1) / 2.0) * 16.0
					if is_turtle:
						turtle_supports.append({"x": x, "row": row, "depth": depth})
					var half_width: float = (float(width) - 3.0) * 0.52 if (lane < 5 and not is_turtle) else (9.0 if is_turtle else (15.0 if lane == 6 else 10.0))
					var gator_fit = BoardVisuals.fit_river_gator(width) if crocodile else {}
					var render_x: float = x + (gator_fit.get("center_offset_pixels", 0.0) if crocodile else 0.0)
					if not BoardVisuals.intersects_playfield(render_x, half_width):
						continue
					var actor_key: String = "lane%d.%d.%d.%d.%s" % [lane, index, member, wrap, str(crocodile)]
					var obj = actor(actor_key, "river_gator" if crocodile else models[lane])
					obj.set_active(true)
					var straddling_edge: bool = absf(render_x - 120.0) + half_width > 110.0
					obj.set_clipped(straddling_edge)
					obj.root.position = pos3(render_x, float(row), ((-0.22 if is_turtle else -0.18) if lane < 5 else 0.02))
					if crocodile:
						obj.root.rotation = Vector3(0, PI / 2.0, 0)
						obj.root.scale = Vector3(gator_fit["width_scale"], 0.9, gator_fit["length_scale"])
						obj.play("Bite")
					elif models[lane] == "log":
						obj.root.scale = Vector3((float(width) - 3.0) / 16.0, 1.0, 1.0)
					elif is_turtle:
						obj.root.position += Vector3(0, -depth, 0)
						obj.root.rotation = Vector3(0, -PI / 2.0, 0)
						obj.play("Dive" if depth > 0.01 else "Swim")
					else:
						obj.root.rotation = Vector3(0, (-1.0 if lane % 2 == 0 else 1.0) * PI / 2.0, 0)
						obj.root.scale = Vector3.ONE * 0.84
						obj.play("Move")

func turtle_ride_depth(x: float, row: float) -> float:
	var nearest: float = 10.0
	var depth: float = 0.0
	for support in turtle_supports:
		var distance: float = absf(x - support["x"])
		if absf(row - float(support["row"])) <= 7.5 and distance < nearest:
			nearest = distance
			depth = support["depth"]
	return depth if nearest <= 10.0 else 0.0

func update_homes_and_hazards() -> void:
	var home_base: int = 0x8263 if BoardVisuals.at(state, 0x83fd) == 2 else 0x825e
	var cur_frame: int = state.get("frame", 0)
	var frac: float = render_fraction()
	for i in range(5):
		var held_here: bool = home_arrival.active(cur_frame, frac) and absf(float(home_arrival.x) - (24.0 + 48.0 * float(i))) <= 8.0
		if BoardVisuals.at(state, home_base + i) != 0 and not held_here:
			var a = actor("home%d" % i, "frog")
			a.set_active(true)
			a.root.position = Vector3(-6.0 + 3.0 * float(i), 0.08, -6.0)
			a.root.rotation = Vector3(0, PI, 0)
			a.play("Celebrate")
		var tile: int = BoardVisuals.at(state, 0xab64 - i * 0xc0)
		if tile >= 44 and tile <= 47:
			var a = actor("homefly%d" % i, "fly")
			a.set_active(true)
			a.root.position = Vector3(-6.0 + 3.0 * float(i), 0.12, -6.0)
		var native_gator: bool = (tile == 208 or BoardVisuals.at(state, 0xab64 - i * 0xc0 + 32) == 208)
		var native_reveal: float = BoardVisuals.home_gator_reveal(state, tile == 208, frac) if native_gator else 0.0
		var reveal_val = home_gator_visuals[i].reveal(float(cur_frame) + frac, native_gator, native_reveal)
		if reveal_val != null:
			var a = actor("homegator%d" % i, "gator")
			a.set_active(true)
			a.root.position = Vector3(-6.0 + 3.0 * float(i), 0.10, -7.03 + 0.75 * float(reveal_val))
			a.root.scale = Vector3.ONE * 0.6
			a.play("Bite" if native_gator else "Idle")

	for addr in [0x8048, 0x8050, 0x8058]:
		var x: int = BoardVisuals.at(state, addr)
		var y: int = BoardVisuals.at(state, addr + 3)
		if x < 8 or x > 235 or y < 32 or y > 136 or BoardVisuals.at(state, addr + 1) == 0:
			continue
		var is_snake: bool = (addr != 0x8058)
		var a = actor("hazard%d" % addr, "snake" if is_snake else "otter")
		a.set_active(true)
		var surface: float = -0.18 + ModelFootprints.LogTopTiles if (is_snake and y < 128) else BoardVisuals.surface_height(float(y))
		var height: float = surface - 0.75 * ModelFootprints.SnakeBottomTiles + 0.008 if is_snake else surface + 0.01
		var motion_id: int = 1000 + addr
		a.root.position = pos3(moving_visuals.step(motion_id, float(x), cur_frame, presentation_delta, paused), float(y), height)
		var heading: int = 1
		if is_snake:
			if snake_facing.has(addr):
				heading = snake_facing[addr]
			else:
				heading = 1 if (BoardVisuals.at(state, addr + 1) & 0x80) != 0 else -1
			var speed: float = moving_visuals.velocity_x(motion_id)
			if absf(speed) > 0.06:
				heading = 1 if speed > 0.0 else -1
			snake_facing[addr] = heading
		a.root.rotation = Vector3(0, float(heading) * PI / 2.0, 0)
		a.root.scale = Vector3.ONE * 0.75
		a.play("Move")

	if lady_visual.visible(ModelFootprints.FrogAlongX * LadyInRiverScale):
		var a = actor("lady", "lady_frog")
		a.set_active(true)
		a.root.position = pos3(moving_visuals.step(50000, lady_visual.x, cur_frame, presentation_delta, paused),
			float(BoardVisuals.LadyFrogRow), BoardVisuals.lady_frog_height(LadyInRiverScale))
		a.root.scale = Vector3.ONE * LadyInRiverScale
		a.root.rotation = Vector3(0, float(lady_facing) * PI / 2.0, 0)
		if lady_hop_start_frame > 0 and lady_last_motion_frame >= lady_hop_start_frame and cur_frame - lady_hop_start_frame < 12:
			a.pose("Hop", clampf((float(cur_frame - lady_hop_start_frame) + frac) / 12.0, 0.0, 1.0) * (10.0 / 60.0))
		else:
			a.play("Idle")

func update_bonuses(fraction: float) -> void:
	var incoming = simulation.get_bonus_awards()
	var rescued: bool = false
	for award in incoming:
		if award.get("kind") == 1:
			rescued = true
	var viewport_size: Vector2 = get_viewport().get_visible_rect().size
	for award in incoming:
		var view: Control
		var display_text: String
		var screen: Vector2
		var kind: int = award.get("kind", 0)
		var amount: int = award.get("amount", 0)
		if kind == 2:
			home_arrival.begin(award, rescued, frog_visual.hop_seconds(award.get("frame", 0), 0.0))
			display_text = "TIME BONUS +%d" % amount
			var panel := PanelContainer.new()
			panel.size = Vector2(minf(370.0, viewport_size.x - 24.0), 106)
			panel.mouse_filter = Control.MOUSE_FILTER_IGNORE
			var st = style_box(Color(0.06, 0.10, 0.20, 0.94), 12, 2)
			st.border_color = Color("ffce57")
			st.content_margin_top = 8
			st.content_margin_bottom = 8
			panel.add_theme_stylebox_override("panel", st)
			var content := VBoxContainer.new()
			content.alignment = BoxContainer.ALIGNMENT_CENTER
			content.add_theme_constant_override("separation", 4)
			panel.add_child(content)
			var heading = make_text("TIME BONUS", 20, Cream)
			heading.add_theme_font_override("font", display_font)
			content.add_child(heading)
			content.add_child(make_text("+%d" % amount, 29, Color("7beaff")))
			view = panel
			screen = Vector2(viewport_size.x * 0.5, viewport_size.y / 3.0)
		else:
			display_text = "+%d" % amount
			var label = make_text(display_text, 28, Color("ff75da") if kind == 1 else Color("fff32f"))
			label.size = Vector2(180, 50)
			label.mouse_filter = Control.MOUSE_FILTER_IGNORE
			label.add_theme_color_override("font_shadow_color", Color.BLACK)
			label.add_theme_constant_override("shadow_offset_y", 3)
			view = label
			screen = camera.unproject_position(pos3(float(award.get("x", 0)), maxf(32.0, float(award.get("row", 0))), 0.83))
			screen.x += -65.0 if kind == 0 else 65.0
			screen.x = clampf(screen.x, 90.0, maxf(90.0, viewport_size.x - 90.0))
			screen.y = clampf(screen.y, minf(140.0, viewport_size.y * 0.2), maxf(140.0, viewport_size.y - 120.0))
		bonus_overlay.add_child(view)
		popups.append(BonusPopup.new(award, view, Vector2(screen.x / viewport_size.x, screen.y / viewport_size.y), display_text))

	var cur_frame: int = state.get("frame", 0)
	for i in range(popups.size() - 1, -1, -1):
		var popup: BonusPopup = popups[i]
		var age: float = maxf(0.0, float(cur_frame - popup.award.get("frame", 0)) + fraction) * FRAME_SECONDS
		var time_bonus: bool = (popup.award.get("kind", 0) == 2)
		var lifetime: float = 1.55 if time_bonus else 1.7
		if age >= lifetime:
			popup.view.queue_free()
			popups.remove_at(i)
			continue
		if time_bonus:
			popup.view.size = Vector2(minf(370.0, viewport_size.x - 24.0), 106)
		var origin: Vector2 = popup.anchor_uv * viewport_size
		popup.view.position = origin - popup.view.size * 0.5 + (Vector2.ZERO if time_bonus else Vector2(0, -age * 36.0))
		var opacity: float = minf(1.0, age / 0.08) if time_bonus else 1.0
		opacity *= clampf((lifetime - age) / (0.22 if time_bonus else 0.35), 0.0, 1.0)
		popup.view.modulate = Color(1, 1, 1, opacity)

func update_death_ripple(fraction: float) -> void:
	var is_active: bool = frog_visual.dying and frog_visual.drowning
	if not is_active:
		if death_ripple != null:
			death_ripple.visible = false
		return
	if death_ripple == null:
		ripple_material = StandardMaterial3D.new()
		ripple_material.albedo_color = Color(0.65, 0.88, 1.0, 0.6)
		ripple_material.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
		ripple_material.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
		var torus := TorusMesh.new()
		torus.inner_radius = 0.42
		torus.outer_radius = 0.45
		torus.rings = 32
		torus.ring_segments = 6
		death_ripple = MeshInstance3D.new()
		death_ripple.mesh = torus
		death_ripple.material_override = ripple_material
		add_child(death_ripple)
	var age: float = frog_visual.death_seconds(state.get("frame", 0), fraction)
	death_ripple.visible = true
	death_ripple.position = pos3(float(frog_visual.death_x), float(frog_visual.death_row), 0.025)
	death_ripple.scale = Vector3(1.0 + age * 0.6, 0.18, 1.0 + age * 0.6)
	var tint = ripple_material.albedo_color
	tint.a = maxf(0.0, 0.65 - age * 0.4)
	ripple_material.albedo_color = tint

# Review tests implementation
func read_review_args() -> void:
	if screenshot == "":
		return
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--review="):
			review = arg.substr(9)
	if review != "" and review != "default-view":
		perspective_view = (review == "perspective" or review == "perspective-follow")
		follow_camera = (review == "perspective-follow" or review == "ortho-follow" or review == "bonus-follow")
	var args = OS.get_cmdline_user_args()
	review_close = args.has("--review-close")
	review_lady_close = args.has("--review-lady-close")
	review_turtle_close = args.has("--review-turtle-close")
	review_gator_close = args.has("--review-gator-close")
	review_snake_close = args.has("--review-snake-close")

func review_frames_list() -> Array:
	match review:
		"carry", "carry-left": return [1, 5, 9, 13, 25]
		"lady-move": return [1, 8, 17, 25, 30, 40, 45]
		"lady-hidden": return [1, 8]
		"lady-hidden-pickup": return [1, 8, 9, 12]
		"lady-goal-overwrite": return [1, 8]
		"lady-rom-clear": return [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
		"snake", "snake-left": return [1, 8]
		"bottom-grass": return [1, 8]
		"hop-right": return [1, 6, 12, 24, 48]
		"log-left", "log-right": return [1, 4, 8, 12, 16, 20, 24, 32, 40, 48]
		"turtle-rider": return [1, 25, 50, 75, 100, 125, 150, 175, 200, 225, 250]
		"turtle-drown": return [1, 100, 124, 125, 126, 130, 140, 150, 175]
		"squash": return [1, 4, 8, 16, 32, 48, 72, 92]
		"edge-squash", "upper-edge-squash": return [1, 4, 8, 16, 32]
		"drown": return [1, 8, 16, 32, 48, 64, 76]
		"bonus", "bonus-follow": return [1, 13, 25, 50, 95, 105, 115]
		"home-input": return [1, 13, 16, 18, 25]
		"home-normal": return [1, 13, 25, 30, 50]
		"final-home": return [1, 3, 4, 5, 7, 9, 11, 16, 25, 28]
		"game-over": return [1, 10]
		"game-over-a": return [1, 2, 4, 8]
		"default-view": return [1, 40]
		"home-gator": return [1, 20, 40, 60, 80, 100]
		"home-gator-retreat": return [1, 20, 40, 60, 80, 100, 175, 176, 178, 182, 186, 188, 190]
		"river-gator": return [1, 12, 24]
		"river-back", "river-snout": return [1, 2, 4, 8, 16]
		"snake-motion": return [1, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80]
		"wrap": return [1, 2, 8, 24, 48, 72, 96, 120]
		"motion": return [60, 61, 62, 63, 64, 65, 66, 67, 68]
		"frog-motion": return [66, 67, 68, 69, 70, 71, 72, 73, 74]
		"turtle": return [1, 25, 50, 75, 100, 125, 150, 175, 200, 225, 250]
		"perspective", "perspective-follow", "ortho-follow": return [1, 40]
		_: return [10]

func set_review_frog(x: int, row: int) -> void:
	simulation.poke(0x8044, x)
	simulation.poke(0x8047, row)
	simulation.poke(0x8004, 0)
	simulation.poke(0x829c, 0)
	simulation.poke(0x83cd, 0)
	simulation.poke(0x81b2, 0)
	simulation.poke(0x8247, 0)
	for a in range(0x8248, 0x8254):
		simulation.poke(a, 0)

func put_passenger_on_log() -> void:
	var x: int = (simulation.peek(0x811c) - 34) & 255
	if x < 50 or x > 195:
		x = (simulation.peek(0x811d) - 34) & 255
	set_review_frog(x, 96)
	simulation.poke(0x8134, 1)
	simulation.poke(0x8135, 1)
	simulation.poke(0x8040, x)
	simulation.poke(0x8041, 0x1e)
	simulation.poke(0x8042, 4)
	simulation.poke(0x8043, 98)

func step_review() -> void:
	review_frame += 1
	if review_frame == 1:
		if review in ["carry", "carry-left", "bonus", "bonus-follow", "home-input"]:
			put_passenger_on_log()
		if review in ["squash", "drown", "edge-squash", "upper-edge-squash"]:
			var x: int = 120
			var row: int = 208
			if review == "edge-squash":
				row = 216
				simulation.poke(0x815a, 1)
				simulation.poke(0x815b, x + 12)
			if review == "upper-edge-squash":
				row = 136
				simulation.poke(0x8136, 1)
				simulation.poke(0x8137, x + 20)
			if review == "drown":
				row = 96
				var best: float = -1.0
				for candidate in range(40, 201):
					var gap: float = 256.0
					var count: int = simulation.peek(0x811b)
					for i in range(count):
						var center: float = float(simulation.peek(0x811c + i) - 34)
						var dist: float = float(abs(((candidate - int(center) + 128) & 255) - 128))
						gap = minf(gap, dist - 22.0)
					if gap > best:
						best = gap
						x = candidate
			set_review_frog(x, row)
			simulation.poke(0x8004, 1)
			simulation.poke(0x829c, 1 if review == "drown" else 0)
		if review == "wrap":
			simulation.poke(0x8113, 254)
		if review == "bottom-grass":
			set_review_frog(120, 240)
		if review in ["log-left", "log-right"]:
			var chosen: int = 120
			var count: int = simulation.peek(0x811b)
			for index in range(mini(8, count)):
				var center: int = (simulation.peek(0x811c + index) - 34) & 255
				if center >= 56 and center <= 184:
					chosen = center
					break
			set_review_frog(chosen, 96)
		if review == "final-home":
			set_review_frog(216, 42)
			for bay in range(4):
				simulation.poke(0x825e + bay, 1)
			simulation.poke(0x825c, 4)
			simulation.poke(0x8268, 0)
			simulation.poke(0x8249, 1)
			simulation.poke(0x8251, 5)
			simulation.poke(0x8254, 2)
			simulation.poke(0x8134, 1)
			simulation.poke(0x8135, 1)
			simulation.poke(0x8040, 216)
			simulation.poke(0x8041, 0x1e)
			simulation.poke(0x8043, 44)
	if review in ["bonus", "bonus-follow", "home-input"] and review_frame == 12:
		set_review_frog(120, 32)
		simulation.poke(0x8134, 1)
		simulation.poke(0x8135, 1)
		simulation.poke(0x8120, 0)
		simulation.poke(0x8121, 3)
		simulation.poke(0x8122, 1)
	if review == "home-normal" and review_frame == 12:
		set_review_frog(120, 32)
		simulation.poke(0x8120, 0)
		simulation.poke(0x8121, 0)
		simulation.poke(0x8122, 1)
		simulation.poke(0x8134, 0)
		simulation.poke(0x8135, 0)
	var input_val: int = 0
	if review in ["carry", "carry-left"] and review_frame >= 4 and review_frame < 6:
		input_val = 4 if review == "carry-left" else 8
	if review in ["motion", "frog-motion"] and review_frame % 2 == 0:
		accumulator = 0.0
		return
	if review == "hop-right":
		input_val = adapt_direction(8)
	if review == "frog-motion":
		input_val = adapt_direction(8)
	if review == "home-input" and review_frame >= 16 and review_frame < 18:
		input_val = adapt_direction(8)
	if review in ["log-left", "log-right"]:
		input_val = adapt_direction(4 if review == "log-left" else 8)
	simulation.step(input_val)
	if review == "lady-rom-clear" and review_frame == 1:
		set_review_frog(120, 224)
		simulation.poke(0x8134, 0)
		simulation.poke(0x8135, 1)
		simulation.poke(0x813d, 0)
		simulation.poke(0x8040, 80)
		simulation.poke(0x8041, 0x21)
		simulation.poke(0x8042, 4)
		simulation.poke(0x8043, 96)
		simulation.poke(0x811c, 100)
		simulation.poke(0x833d, 1)
		simulation.poke(0x833e, 50)
		simulation.poke(0x8340, 2)
	if review == "game-over-a":
		if review_frame == 1:
			simulation.poke(0x83fe, 0)
		if review_frame == 2:
			var btn := InputEventJoypadButton.new()
			btn.button_index = JOY_BUTTON_A
			btn.pressed = true
			_unhandled_input(btn)
	if review in ["river-gator", "river-back", "river-snout"]:
		var x: int = 120
		simulation.poke(0x83b7, 2)
		simulation.poke(0x8100, 1)
		simulation.poke(0x8101, x + 12 + 30)
		simulation.poke(0x8150, 1)
		if review_frame == 1 and review != "river-gator":
			set_review_frog(130 if review == "river-back" else 152, 48)
	if review in ["home-gator", "home-gator-retreat"]:
		var phase: int = mini(review_frame, 120 if review == "home-gator" else 200)
		var bay: int = 2
		var base_address: int = 0xab64 - bay * 0xc0
		simulation.poke(0x8122, phase)
		simulation.poke(base_address, 16 if (phase < 80 or phase >= 176) else 208)
		simulation.poke(base_address + 1, 16 if (phase < 80 or phase >= 176) else 209)
		simulation.poke(base_address + 32, 16 if phase >= 176 else (208 if phase < 80 else 210))
		simulation.poke(base_address + 33, 16 if phase >= 176 else (209 if phase < 80 else 211))
	if review == "game-over":
		simulation.poke(0x83fe, 0)
	if review == "turtle-rider" or (review == "turtle-drown" and review_frame <= 125):
		var probe: Dictionary = simulation.snapshot()
		var best: float = -1.0
		var ride_x: int = 120
		var ride_row: int = 64
		for lane in [1, 4]:
			var table: int = 0x8100 + lane * 9
			var row: int = (lane + 3) * 16
			var group_size: int = 2 if lane == 1 else 3
			var width: int = 31 if lane == 1 else 47
			for index in range(mini(8, BoardVisuals.at(probe, table))):
				var center: float = float(BoardVisuals.at(probe, table + index + 1) - 12 - width / 2)
				var diver: bool = known_diving_groups.has("%d_%d" % [lane, index]) or BoardVisuals.turtle_phase(probe, center, row) > 0
				var depth: float = BoardVisuals.turtle_depth(probe, center, row, 0.0, diver)
				for member in range(group_size):
					var x: int = int(roundf(center + (float(member) - float(group_size - 1) / 2.0) * 16.0))
					if x >= 24 and x <= 216 and depth > best:
						best = depth
						ride_x = x
						ride_row = row
		set_review_frog(ride_x, ride_row)
		if review == "turtle-drown" and review_frame == 125:
			simulation.poke(0x8004, 1)
			simulation.poke(0x829c, 1)
	if review == "lady-move":
		var bx: int = 96 + review_frame if review_frame <= 16 else (128 - review_frame if review_frame <= 30 else 98 + review_frame - 30)
		simulation.poke(0x8134, 0)
		simulation.poke(0x8135, 1)
		simulation.poke(0x8040, bx)
		simulation.poke(0x8041, 0xa1 if review_frame <= 16 else (0x21 if review_frame <= 30 else 0x1e))
		simulation.poke(0x8042, 4)
		simulation.poke(0x8043, 96)
	if review in ["lady-hidden", "lady-hidden-pickup", "lady-goal-overwrite"]:
		var patrol_x: int = 80
		if review_frame == 1:
			set_review_frog(120, 224)
		if review == "lady-hidden-pickup" and review_frame >= 9:
			set_review_frog(patrol_x, 96)
		simulation.poke(0x8135, 1)
		simulation.poke(0x8134, 1 if (review == "lady-hidden-pickup" and review_frame >= 9) else 0)
		simulation.poke(0x811c, 100)
		simulation.poke(0x833d, 1)
		simulation.poke(0x833e, 49)
		simulation.poke(0x8040, 216 if review == "lady-goal-overwrite" else 0)
		simulation.poke(0x8041, 0x19 if review == "lady-goal-overwrite" else 0)
		simulation.poke(0x8042, 3 if review == "lady-goal-overwrite" else 0)
		simulation.poke(0x8043, 16 if review == "lady-goal-overwrite" else 0)
	if review in ["snake", "snake-left"]:
		var x: int = 180 - review_frame if review == "snake-left" else ((simulation.peek(0x811c) - 34) & 255)
		if review == "snake" and (x < 50 or x > 195):
			x = (simulation.peek(0x811d) - 34) & 255
		simulation.poke(0x8048, x)
		simulation.poke(0x8049, 0x81 if review == "snake-left" else 1)
		simulation.poke(0x804b, 96)
	observe_frame()
	accumulator = 0.0

func capture_review() -> void:
	var r_frames = review_frames_list()
	if review_capture_pending or not r_frames.has(review_frame):
		return
	review_capture_pending = true
	var frame: int = review_frame
	await RenderingServer.frame_post_draw
	var dir_name: String = screenshot.get_base_dir()
	var base_stem: String = screenshot.get_file().get_basename()
	var stem: String = dir_name.path_join(base_stem)
	var path: String = "%s-%03d.png" % [stem, frame]
	var img = get_viewport().get_texture().get_image()
	var error = img.save_png(path)

	var player = actor("player", "frog")
	var free_lady: ModelActor = actors.get("lady")
	var snake: ModelActor = actors.get("hazard32840")
	var home_gator: ModelActor = actors.get("homegator2")
	var river_gator: ModelActor = actors.get("lane0.0.0.0.True")
	var rider: ModelActor = actors.get("passenger")

	var rider_visible: bool = (rider != null and rider.root.is_visible_in_tree())
	if review == "river-back" and BoardVisuals.at(state, 0x8004) != 0 and not frog_visual.dying:
		review_gator_safe_observed = true
	if review == "river-snout" and BoardVisuals.at(state, 0x829c) != 0 and frog_visual.dying:
		review_gator_snout_death_observed = true
	if review == "lady-rom-clear" and BoardVisuals.at(state, 0x8135) != 0 and BoardVisuals.at(state, 0x8041) == 0:
		if BoardVisuals.at(state, 0x8040) == 0:
			review_lady_clear_visible = review_lady_clear_visible or (free_lady != null and free_lady.root.is_visible_in_tree())
		else:
			review_lady_patrol_visible = review_lady_patrol_visible or (free_lady != null and free_lady.root.is_visible_in_tree())
	var socket_error: float = (rider.root.global_position - player.passenger_socket.global_position).length() if (rider_visible and player.passenger_socket != null) else 0.0

	var hop_flags: Array = []
	var hop_counters: Array = []
	for i in range(4):
		hop_flags.append(BoardVisuals.at(state, 0x8248 + i))
		hop_counters.append(BoardVisuals.at(state, 0x8250 + i))

	var bonus_labels_arr: Array = []
	for p in popups:
		bonus_labels_arr.append({
			"amount": p.award.get("amount", 0),
			"kind": "Rescue" if p.award.get("kind", 0) == 1 else ("Time" if p.award.get("kind", 0) == 2 else "Bug"),
			"text": p.display_text,
			"screenX": p.view.position.x,
			"screenY": p.view.position.y
		})

	var visible_logs: int = 0
	for k in actors.keys():
		if k.begins_with("lane") and actors[k].root.is_visible_in_tree():
			visible_logs += 1

	var cur_state_frame: int = state.get("frame", 0)
	var measurement: Dictionary = {
		"frame": frame,
		"romFrame": cur_state_frame,
		"dying": frog_visual.dying,
		"drowning": frog_visual.drowning,
		"gameState": BoardVisuals.at(state, 0x83fe),
		"started": started,
		"menuVisible": menu.visible,
		"mouseMode": "Visible" if Input.mouse_mode == Input.MOUSE_MODE_VISIBLE else "Hidden",
		"messageText": message_label.text,
		"messagePosition": [message_label.global_position.x, message_label.global_position.y],
		"messageSize": [message_label.size.x, message_label.size.y],
		"nativeX": BoardVisuals.at(state, 0x8044),
		"nativeRow": BoardVisuals.at(state, 0x8047),
		"displayedFrogX": displayed_frog_x,
		"displayedFrogRow": displayed_frog_row,
		"hopFlags": hop_flags,
		"hopCounters": hop_counters,
		"deathSeconds": frog_visual.death_seconds(cur_state_frame, 0.0),
		"deathInitialHeight": death_initial_height,
		"drownFloor": BoardVisuals.surface_height(float(frog_visual.death_row)) - FrogVisualState.MaximumDrownDepth,
		"carrying": frog_visual.carrying,
		"riderVisible": rider_visible,
		"socketError": socket_error,
		"playerY": player.root.global_position.y,
		"riderY": rider.root.global_position.y if rider_visible else 0.0,
		"playerZ": player.root.global_position.z,
		"riderZ": rider.root.global_position.z if rider_visible else 0.0,
		"playerYaw": player.root.global_rotation.y,
		"riderYaw": rider.root.global_rotation.y if rider_visible else 0.0,
		"riderScale": rider.root.scale.x if rider_visible else 0.0,
		"ladyVisible": free_lady.root.is_visible_in_tree() if free_lady != null else false,
		"ladyYaw": free_lady.root.global_rotation.y if free_lady != null else 0.0,
		"ladyScale": free_lady.root.scale.x if free_lady != null else 0.0,
		"ladyCode": BoardVisuals.at(state, 0x8041),
		"ladyX": BoardVisuals.at(state, 0x8040),
		"ladyRow": BoardVisuals.at(state, 0x8043),
		"ladyArmed": BoardVisuals.at(state, 0x8135) != 0,
		"ladyAttached": BoardVisuals.at(state, 0x8134) != 0,
		"ladyY": free_lady.root.global_position.y if free_lady != null else 0.0,
		"ladyVisualX": lady_visual.x,
		"snakeVisible": snake.root.is_visible_in_tree() if snake != null else false,
		"snakeY": snake.root.global_position.y if snake != null else 0.0,
		"snakeX": BoardVisuals.at(state, 0x8048),
		"snakeRow": BoardVisuals.at(state, 0x804b),
		"snakeCode": BoardVisuals.at(state, 0x8049),
		"snakeYaw": snake.root.global_rotation.y if snake != null else 0.0,
		"homeGatorVisible": home_gator.root.is_visible_in_tree() if home_gator != null else false,
		"homeGatorZ": home_gator.root.global_position.z if home_gator != null else 0.0,
		"homeGatorY": home_gator.root.global_position.y if home_gator != null else 0.0,
		"homeGatorPhase": BoardVisuals.at(state, 0x8122),
		"homeGatorRetreating": home_gator_visuals[2].retreating(),
		"riverGatorVisible": river_gator.root.is_visible_in_tree() if river_gator != null else false,
		"riverGatorScale": [river_gator.root.scale.x, river_gator.root.scale.y, river_gator.root.scale.z] if river_gator != null else [],
		"riverGatorX": river_gator.root.global_position.x if river_gator != null else 0.0,
		"riverGatorTipX": 120.0 + 16.0 * (river_gator.root.global_position.x + ModelFootprints.RiverGatorFrontTiles * river_gator.root.scale.z) if river_gator != null else 0.0,
		"riverGatorSnoutLength": 16.0 * ModelFootprints.RiverGatorSnoutTiles * river_gator.root.scale.z if river_gator != null else 0.0,
		"nativeGatorTipX": BoardVisuals.at(state, 0x8101),
		"riverGatorArmed": BoardVisuals.river_gator_active(state),
		"riverGatorZone": "Back" if BoardVisuals.river_gator_contact(BoardVisuals.at(state, 0x8044), BoardVisuals.at(state, 0x8101)) == BoardVisuals.RiverGatorZone.Back else ("Snout" if BoardVisuals.river_gator_contact(BoardVisuals.at(state, 0x8044), BoardVisuals.at(state, 0x8101)) == BoardVisuals.RiverGatorZone.Snout else "Outside"),
		"riverGatorRideTile": BoardVisuals.at(state, 0xa846),
		"holdFlag": BoardVisuals.at(state, 0x8004),
		"secondBank": BoardVisuals.at(state, 0x829c),
		"hopActive": frog_visual.hop_active(cur_state_frame),
		"hopDirection": frog_visual.hop_direction,
		"ladyHopping": lady_hop_start_frame > 0 and cur_state_frame - lady_hop_start_frame < 12,
		"turtleRideDepth": turtle_ride_depth(float(BoardVisuals.at(state, 0x8044)), float(BoardVisuals.at(state, 0x8047))),
		"modelScale": [player.root.scale.x, player.root.scale.y, player.root.scale.z],
		"holdingHome": home_arrival.active(cur_state_frame, 0.0),
		"finishingHomeHop": home_arrival.finishing_hop(cur_state_frame, 0.0),
		"homeX": home_arrival.x,
		"homeRow": home_arrival.row,
		"homeVisualRow": home_arrival.visual_row(cur_state_frame, 0.0),
		"bonusLabels": bonus_labels_arr,
		"cameraMode": "Perspective" if camera.projection == Camera3D.PROJECTION_PERSPECTIVE else "Orthogonal",
		"following": follow_camera,
		"modernCollision": modern,
		"fullscreenPreference": fullscreen,
		"windowMode": "Fullscreen" if DisplayServer.window_get_mode() == DisplayServer.WINDOW_MODE_FULLSCREEN else "Windowed",
		"cameraSize": camera.size,
		"cameraFov": camera.fov,
		"cameraPosition": [camera.position.x, camera.position.y, camera.position.z],
		"motionLogX": moving_visuals.display_x(3 * 16),
		"motionCarX": moving_visuals.display_x(8 * 16),
		"nativeLogX": BoardVisuals.at(state, 0x811c) - 34,
		"nativeCarX": BoardVisuals.at(state, 0x8149) - 12,
		"visibleLogs": visible_logs,
		"error": "OK" if error == OK else "Error"
	}
	review_measurements.append(measurement)
	print("REVIEW %s %d %s" % [review, frame, error])
	if error != OK:
		get_tree().quit(1)
		return
	if frame == r_frames[-1]:
		if review == "lady-move" and lady_facing != 3:
			push_error("Pink frog lost its left-facing pose during neutral log drift")
			get_tree().quit(1)
			return
		if review == "lady-hidden" and not (free_lady != null and free_lady.root.is_visible_in_tree()):
			push_error("Armed pink frog is invisible with a blank ROM sprite")
			get_tree().quit(1)
			return
		if review == "lady-goal-overwrite" and (not (free_lady != null and free_lady.root.is_visible_in_tree()) or absf(lady_visual.x - 80.0) > 1.0):
			push_error("Goal sprite overwrote the pink frog's position")
			get_tree().quit(1)
			return
		if review == "lady-rom-clear" and (not review_lady_clear_visible or not review_lady_patrol_visible):
			push_error("The original ROM clear made the pink frog invisible in Godot")
			get_tree().quit(1)
			return
		if review == "river-back" and not review_gator_safe_observed:
			push_error("Modern river gator back did not remain safe")
			get_tree().quit(1)
			return
		if review == "river-snout" and not review_gator_snout_death_observed:
			push_error("Modern river gator snout was not fatal")
			get_tree().quit(1)
			return
		if review == "lady-hidden-pickup" and (not rider_visible or (free_lady != null and free_lady.root.is_visible_in_tree())):
			push_error("Pink frog did not move visibly from log to passenger")
			get_tree().quit(1)
			return
		if review == "game-over-a" and BoardVisuals.at(state, 0x83fe) == 0:
			push_error("Controller A did not restart after game over")
			get_tree().quit(1)
			return
		if review == "default-view" and (not modern or not perspective_view or not follow_camera or not fullscreen):
			push_error("Fresh-game defaults changed unexpectedly")
			get_tree().quit(1)
			return
		if review == "snake-left" and (snake != null and snake.root.rotation.y > -1.3):
			push_error("Snake failed to face its leftward travel")
			get_tree().quit(1)
			return
		if review == "snake-motion" and (snake != null and snake.root.rotation.y < 1.3):
			push_error("Snake followed a ROM flip instead of its rightward lane travel")
			get_tree().quit(1)
			return
		var json_str = JSON.stringify(review_measurements, "\t")
		var f_out = FileAccess.open(stem + ".json", FileAccess.WRITE)
		if f_out != null:
			f_out.store_string(json_str)
			f_out.close()
		get_tree().quit(0)
		return
	review_capture_pending = false

func update_review_camera() -> void:
	if not review_close and not review_lady_close and not review_turtle_close and not review_gator_close and not review_snake_close:
		return
	var player = actor("player", "frog")
	var target: Vector3
	if review_gator_close:
		target = pos3(134.0, 48.0, 0.0) if review in ["river-gator", "river-back", "river-snout"] else Vector3(0, 0.30, -6.3)
	elif review_snake_close and actors.has("hazard32840"):
		target = actors["hazard32840"].root.position + Vector3.UP * 0.1
	elif review_turtle_close and turtle_supports.any(func(t): return t["x"] >= 24.0 and t["x"] <= 216.0):
		var valid_turtles = turtle_supports.filter(func(t): return t["x"] >= 24.0 and t["x"] <= 216.0)
		valid_turtles.sort_custom(func(a, b): return a["depth"] > b["depth"])
		var best_t = valid_turtles[0]
		target = pos3(best_t["x"], float(best_t["row"]), -0.22 - best_t["depth"]) + Vector3.UP * 0.2
	elif review_lady_close and actors.has("lady"):
		target = actors["lady"].root.position + Vector3.UP * 0.2
	else:
		target = player.root.position + Vector3.UP * 0.3

	camera.size = 3.3 if review_gator_close else 2.5
	camera.position = target + (Vector3(2.0, 2.2, 2.9) if review_gator_close else Vector3(2.5, 2.5, 3.5))
	camera.look_at(target)
