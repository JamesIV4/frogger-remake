extends Node3D
class_name FroggerGame

const FRAME_SECONDS: float = 33.0 / 2000.0

var simulation: RefCounted = null
var state: Dictionary = {"frame": 0, "ram": [], "video": [], "objects": [], "sounds": []}
var actors: Dictionary = {}
var player_frog: String = "frog"
var batched_lanes: Dictionary = {}
var warmup_node: Node3D = null
var warmup_frames: int = 0
var input_pulse: InputPulse = InputPulse.new()
var audio_player: AudioStreamPlayer = null
var audio_playback: AudioStreamGeneratorPlayback = null
var audio_capacity: int = 0
var web_audio: JavaScriptObject = null
var web_audio_checked: bool = false

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
var top_down_camera: bool = false
var follow_camera: bool = true
var desktop_follow_zoom_percent: float = 0.0
var mobile_follow_zoom_percent: float = 0.0
var camera_look_target: Vector3 = Vector3(0.0, 0.0, -0.12)
var camera_view_size: Vector2 = Vector2.ZERO

# Presentation helpers
var frog_visual: FrogVisualState = FrogVisualState.new()
var lady_visual: BoardVisuals.LadyFrogPresentation = BoardVisuals.LadyFrogPresentation.new()
var home_arrival: FrogVisualState.HomeArrivalVisual = FrogVisualState.HomeArrivalVisual.new()
var moving_visuals: PresentationMotion = PresentationMotion.new()
var snake_visuals: Dictionary = {}
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
var turtle_supports: Array = []
enum BeaverVisualPhase { Hidden, Approach, Grab, Look, Bite, Sink, Done }
var beaver_phase: BeaverVisualPhase = BeaverVisualPhase.Hidden
var beaver_phase_seconds: float = 0.0
var beaver_player_hit: bool = false
var beaver_last_x: float = 120.0
var beaver_last_row: float = 80.0
var beaver_heading: int = 1
var home_gator_visuals: Array = []
var river_gator_visual = BoardVisuals.RiverGatorPresentation.new()
var anchored_death_frame: int = -1
var last_live_player_frame: int = -1
var death_initial_height: float = 0.0
var last_live_player_height: float = 0.0
var last_live_player_x: float = 0.0
var last_live_player_row: float = 0.0
var level_intro_home_counts: Dictionary = {}
var level_intro_number: int = 0
var level_intro_waiting: bool = false
var level_intro_fade: float = 0.0
var level_intro_player: int = 0
const LevelIntroFadeIn: float = 0.25
const LevelIntroFadeOut: float = 0.20
const ModalVerticalPadding: float = 24.0
const ModalGap: float = 12.0
const PortraitFooterGap: float = 8.0
var lady_facing: int = 2
var lady_hop_start_frame: int = -1
var lady_last_motion_frame: int = -1

const LadyInRiverScale: float = 0.62
const PassengerScale: float = 0.76
const BeaverScale: float = 0.75
const BeaverAttackSeconds: float = 0.60
const BeaverGrabSeconds: float = 0.42
const BeaverLookSeconds: float = 0.24
const BeaverBiteSeconds: float = 0.22
const BeaverSinkSeconds: float = 0.28
const BeaverUnderHoldSeconds: float = 0.08

# ROM lane tables. Rigid lanes render through one MultiMesh per lane; lanes with
# rigged swimmers (turtles) and the river gator keep ModelActor nodes because
# those meshes are skinned and animated.
const LaneWidths: Array = [60, 31, 92, 44, 47, 0, 34, 18, 18, 18, 18]
const LaneModels: Array = ["log", "turtle", "log", "log", "turtle", "", "truck", "sport", "car", "dozer", "racecar"]
const BatchedLaneModels: Dictionary = {0: "log", 2: "log", 3: "log", 6: "truck", 7: "sport", 8: "car", 9: "dozer", 10: "racecar"}

# UI elements
var score_label: Label = null
var high_label: Label = null
var level_label: Label = null
var lives_label: Label = null
var lives_icons: HBoxContainer = null
var frog_icon_texture: Texture2D = null

# Mobile swipe & touch control state
var touch_start_pos: Vector2 = Vector2.ZERO
var touch_start_time: int = 0
var touch_active: bool = false
var swipe_triggered: bool = false
var swipe_direction: int = 0
var swipe_frames: int = 0
const SWIPE_THRESHOLD: float = 30.0
var message_label: Label = null
var message_hint: Label = null
var hud_header: PanelContainer = null
var hud_footer: PanelContainer = null
var last_input_method: String = "keyboard"
var player_header: Label = null
var timer_bar: ProgressBar = null
var menu: PanelContainer = null
var menu_scroll: ScrollContainer = null
var message_panel: PanelContainer = null
var menu_items: VBoxContainer = null
var bonus_overlay: Control = null
var fps_container: PanelContainer = null
var fps_label: Label = null
var show_fps: bool = false
var options_open: bool = false
var options_return_to_pause: bool = false
var gameplay_options_button: Button = null
var mouse_idle_seconds: float = 0.0
var touch_device: bool = false
var layout_ui: Callable
var prof_sim_us: float = 0.0
var prof_cam_us: float = 0.0
var prof_audio_us: float = 0.0
var prof_actors_us: float = 0.0
var prof_hud_us: float = 0.0
var prof_total_script_us: float = 0.0
var prof_last_steps: int = 0
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
var review_beaver_close: bool = false
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
	if args.has("--pause-shot"):
		show_menu(true)
	elif args.has("--options-shot"):
		show_options(started)
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
	options_open = false
	menu.visible = false
	reset_mouse_idle()
	message_label.text = ""
	message_panel.visible = false

func _process(delta: float) -> void:
	update_mouse_visibility(delta)
	if simulation == null:
		return
	if warmup_node != null:
		# Let the hidden precompiled actors draw behind the menu so GPU uploads
		# happen now. The node hides after two presented frames; the compiled
		# shaders stay cached for gameplay without per-type upload stutters.
		warmup_frames += 1
		if warmup_frames >= 2:
			warmup_node.visible = false
	var t_start: int = Time.get_ticks_usec()
	presentation_delta = clampf(delta, 0.0, 0.1)
	var steps: int = 0
	if review != "":
		if review_capture_pending:
			return
		step_review()
	elif not paused:
		accumulator = minf(accumulator + delta, 0.066)
		while accumulator >= FRAME_SECONDS and steps < 4:
			steps += 1
			var input_val: int = read_direction() if started else 0
			if coin_frames > 0:
				input_val |= 16
				coin_frames -= 1
			simulation.step(input_val)
			observe_frame()
			accumulator -= FRAME_SECONDS
	var t_sim: int = Time.get_ticks_usec()
	update_frog_motion()
	feed_audio()
	var t_audio: int = Time.get_ticks_usec()
	update_actors()
	var t_actors: int = Time.get_ticks_usec()
	# Home-arrival presentation is resolved by update_actors before following
	# the visible frog, rather than the simulation's already-reset position.
	update_camera(delta)
	var t_cam: int = Time.get_ticks_usec()
	update_hud()
	var t_hud: int = Time.get_ticks_usec()
	if water != null:
		water.set_shader_parameter("clock", float(state.get("frame", 0)) * FRAME_SECONDS)
	update_review_camera()

	if show_fps:
		var sim_us: float = float(t_sim - t_start)
		var cam_us: float = float(t_cam - t_actors)
		var audio_us: float = float(t_audio - t_sim)
		var actors_us: float = float(t_actors - t_audio)
		var hud_us: float = float(t_hud - t_cam)
		var total_us: float = float(t_hud - t_start)
		prof_sim_us = lerpf(prof_sim_us, sim_us, 0.1)
		prof_cam_us = lerpf(prof_cam_us, cam_us, 0.1)
		prof_audio_us = lerpf(prof_audio_us, audio_us, 0.1)
		prof_actors_us = lerpf(prof_actors_us, actors_us, 0.1)
		prof_hud_us = lerpf(prof_hud_us, hud_us, 0.1)
		prof_total_script_us = lerpf(prof_total_script_us, total_us, 0.1)
		prof_last_steps = steps

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

func trigger_swipe(dir: int) -> void:
	swipe_direction = dir
	swipe_frames = 6

func read_direction() -> int:
	var pads = Input.get_connected_joypads()
	var joy: int = pads[0] if pads.size() > 0 else -1
	var digital: int = 0
	if swipe_frames > 0:
		digital |= swipe_direction
		swipe_frames -= 1
		if swipe_frames == 0:
			swipe_direction = 0
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

func reset_mouse_idle() -> void:
	mouse_idle_seconds = 0.0
	Input.mouse_mode = Input.MOUSE_MODE_VISIBLE

func update_mouse_visibility(delta: float) -> void:
	if touch_device or not started or paused or menu == null or menu.visible or BoardVisuals.at(state, 0x83fe) == 0:
		reset_mouse_idle()
		return
	mouse_idle_seconds += delta
	if mouse_idle_seconds >= 2.0:
		Input.mouse_mode = Input.MOUSE_MODE_HIDDEN

func _input(event: InputEvent) -> void:
	# Mouse-to-touch and touch-to-mouse emulation must not change the real device.
	if event.device != InputEvent.DEVICE_ID_EMULATION:
		if event is InputEventKey and event.pressed and not event.echo:
			last_input_method = "keyboard"
		elif event is InputEventMouseButton and event.pressed:
			last_input_method = "keyboard"
		elif (event is InputEventJoypadButton and event.pressed) or (event is InputEventJoypadMotion and absf(event.axis_value) > 0.35):
			last_input_method = "controller"
		elif (event is InputEventScreenTouch and event.pressed) or event is InputEventScreenDrag:
			last_input_method = "touch"
	# Observe motion before Controls consume it, including motion over the HUD.
	if event is InputEventMouseMotion and event.relative != Vector2.ZERO:
		reset_mouse_idle()

func restart_prompt() -> String:
	match last_input_method:
		"touch": return "Tap to restart"
		"controller": return "Press controller A to restart"
	return "Press Enter to restart"

func navigate_back() -> void:
	if options_open:
		show_menu(options_return_to_pause)
	elif started:
		if paused:
			resume_game()
		else:
			show_menu(true)

func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventKey and event.pressed and not event.echo:
		match event.physical_keycode:
			KEY_ENTER, KEY_SPACE:
				if options_open:
					return
				if not started or BoardVisuals.at(state, 0x83fe) == 0:
					start_game(1)
				elif paused:
					resume_game()
			KEY_1:
				if not options_open:
					start_game(1)
			KEY_2:
				if not options_open:
					start_game(2)
			KEY_ESCAPE, KEY_P:
				navigate_back()
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
		if event.button_index == JOY_BUTTON_A and not options_open and (not started or BoardVisuals.at(state, 0x83fe) == 0):
			start_game(1)
		elif event.button_index == JOY_BUTTON_START:
			if not started and not options_open:
				start_game(1)
			else:
				navigate_back()
		elif event.button_index == JOY_BUTTON_B and menu.visible:
			navigate_back()

	if menu.visible:
		return
	# Mobile swipe and touch controls
	if event is InputEventScreenTouch:
		if event.pressed:
			touch_start_pos = event.position
			touch_start_time = Time.get_ticks_msec()
			touch_active = true
			swipe_triggered = false
		else:
			if touch_active and not swipe_triggered:
				var elapsed: int = Time.get_ticks_msec() - touch_start_time
				var dist: float = (event.position - touch_start_pos).length()
				if elapsed < 350 and dist < 25.0:
					if not started or BoardVisuals.at(state, 0x83fe) == 0:
						start_game(1)
					elif not paused and (menu == null or not menu.visible):
						trigger_swipe(1)
			touch_active = false
			swipe_triggered = false

	elif event is InputEventScreenDrag:
		if touch_active and (menu == null or not menu.visible):
			var offset: Vector2 = event.position - touch_start_pos
			if offset.length() >= SWIPE_THRESHOLD:
				if not started or BoardVisuals.at(state, 0x83fe) == 0:
					start_game(1)
				elif not paused:
					var dir: int = 0
					if absf(offset.x) > absf(offset.y):
						dir = 8 if offset.x > 0 else 4
					else:
						dir = 2 if offset.y > 0 else 1
					trigger_swipe(dir)
				touch_start_pos = event.position
				swipe_triggered = true

	elif event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_LEFT:
			if event.pressed:
				touch_start_pos = event.position
				touch_start_time = Time.get_ticks_msec()
				touch_active = true
				swipe_triggered = false
			else:
				if touch_active and not swipe_triggered:
					var elapsed: int = Time.get_ticks_msec() - touch_start_time
					var dist: float = (event.position - touch_start_pos).length()
					if elapsed < 350 and dist < 25.0:
						if not started or BoardVisuals.at(state, 0x83fe) == 0:
							start_game(1)
						elif not paused and (menu == null or not menu.visible):
							trigger_swipe(1)
				touch_active = false
				swipe_triggered = false

	elif event is InputEventMouseMotion:
		if touch_active and (event.button_mask & MOUSE_BUTTON_MASK_LEFT) != 0 and (menu == null or not menu.visible):
			var offset: Vector2 = event.position - touch_start_pos
			if offset.length() >= SWIPE_THRESHOLD:
				if not started or BoardVisuals.at(state, 0x83fe) == 0:
					start_game(1)
				elif not paused:
					var dir: int = 0
					if absf(offset.x) > absf(offset.y):
						dir = 8 if offset.x > 0 else 4
					else:
						dir = 2 if offset.y > 0 else 1
					trigger_swipe(dir)
				touch_start_pos = event.position
				swipe_triggered = true

func resume_game() -> void:
	paused = false
	options_open = false
	menu.visible = false
	reset_mouse_idle()
	accumulator = 0.0

func toggle_pause_from_gear() -> void:
	if not started:
		return
	if paused:
		resume_game()
	else:
		show_menu(true)

func actor(key: String, model: String) -> ModelActor:
	if actors.has(key) and not actors[key].root.scene_file_path.ends_with("/%s.glb" % model):
		actors[key].root.queue_free()
		actors.erase(key)
		if key == "player":
			actors.erase("passenger")
	if not actors.has(key):
		actors[key] = ModelActor.new(self, model)
	return actors[key]

func active_frog_model() -> String:
	if BoardVisuals.at(state, 0x83fd) == 2:
		return "frog" if player_frog == "lady_frog" else "lady_frog"
	return player_frog

func rescue_frog_model() -> String:
	return "frog" if active_frog_model() == "lady_frog" else "lady_frog"

func select_player_frog(model: String) -> void:
	if model == player_frog or model not in ["frog", "lady_frog"]:
		return
	player_frog = model
	# The passenger is owned by its player's socket; freeing that root also
	# frees the passenger. Recreate cached actors with their newly selected roles.
	for view in actors.values():
		if view.root.get_parent() == self:
			view.root.queue_free()
	actors.clear()
	save_preferences()
	show_menu(false)

func setup_frog_selector(parent: VBoxContainer) -> void:
	parent.add_child(make_text("CHOOSE YOUR FROG", 15, Cream))
	var choices := HBoxContainer.new()
	choices.add_theme_constant_override("separation", 10)
	parent.add_child(choices)
	for model in ["frog", "lady_frog"]:
		var choice := FrogChoice.new()
		choice.configure(model, model == player_frog, body_font)
		choice.pressed.connect(select_player_frog.bind(model))
		choices.add_child(choice)

static func pos3(x: float, row: float, height: float = 0.0) -> Vector3:
	return Vector3((x - 120.0) / 16.0, height, (row - 128.0) / 16.0)

static func _hide_warmup_shadows(node: Node) -> void:
	# Warmup instances sit under the opaque board so the board occludes their
	# color output; keep them out of the shadow map too so they never darken it.
	if node is GeometryInstance3D:
		(node as GeometryInstance3D).cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	for child in node.get_children():
		_hide_warmup_shadows(child)

# Model coverage for the one-time GPU warmup: every ModelActor asset plus the
# batched lane bodies (including the board) and both wheel sides.
const WarmupModels: Array = ["frog", "lady_frog", "turtle", "gator", "river_gator", "fly", "snake", "otter"]
const WarmupClips: Dictionary = {
	"frog": ["Idle", "Hop", "Squash", "Drown", "Celebrate"],
	"lady_frog": ["Idle", "Hop", "Drown", "Celebrate"],
	"turtle": ["Idle", "Swim", "Dive"],
	"gator": ["Idle", "Bite"],
	"river_gator": ["Idle", "Bite"],
	"fly": ["Idle", "Move"], "snake": ["Idle", "Move"], "otter": ["Idle", "Move", "Attack"],
}
const WarmupBatched: Array = ["log", "car", "truck", "sport", "dozer", "racecar"]

func setup_world() -> void:
	# Pre-compile every gameplay asset before the first real frame so the web
	# build pays import, shader and skin upload costs once, up front, instead of
	# stuttering the first time each actor type comes on screen. Instances stay
	# alive behind the menu for a couple of frames so the renderer actually
	# uploads them, then _process hides the node once drawn. Parked well under the
	# board base (board vertices bottom out near y=-1.42) at board center: the
	# opaque slab occludes every pixel so nothing ever flashes behind the menu,
	# while the instances stay inside the frustum and still submit real draw
	# work (so pipelines, skins and buffers upload).
	warmup_node = Node3D.new()
	warmup_node.name = "WarmupPrecompile"
	warmup_node.position = Vector3(0, -3.0, 0.0)
	add_child(warmup_node)
	for model in WarmupModels:
		var showcase := ModelActor.new(warmup_node, model)
		_hide_warmup_shadows(showcase.root)
		showcase.set_active(true)
		showcase.set_clipped(true)
		# Touch squash's instance uniform path once so the driver has the variant.
		showcase.set_squash_color(0.6)
		showcase.set_squash_color(0.0)
		for clip in WarmupClips[model]:
			showcase.play(clip)
		var double := ModelActor.new(warmup_node, model)
		double.set_active(true)
	# Pre-build the rigid batched equivalents too: touching every log/vehicle
	# body and wheel mesh compiles the lane shader against those vertex layouts.
	for model in WarmupBatched:
		var template := BatchedLane.get_template(model)
		if template == null:
			continue
		var probe := MeshInstance3D.new()
		probe.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
		probe.mesh = template.body_mesh
		warmup_node.add_child(probe)
		if template.wheel_mesh_left != null:
			var left := MeshInstance3D.new()
			left.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
			left.mesh = template.wheel_mesh_left
			warmup_node.add_child(left)
		if template.wheel_mesh_right != null:
			var right := MeshInstance3D.new()
			right.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
			right.mesh = template.wheel_mesh_right
			warmup_node.add_child(right)
	# The drown ripple builds its torus lazily; compiling it now avoids a
	# hitch on the first water death.
	var warm_torus := TorusMesh.new()
	warm_torus.inner_radius = 0.42
	warm_torus.outer_radius = 0.45
	warm_torus.rings = 32
	warm_torus.ring_segments = 6
	var warm_ripple := MeshInstance3D.new()
	warm_ripple.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	warm_ripple.mesh = warm_torus
	var warm_ripple_mat := StandardMaterial3D.new()
	warm_ripple_mat.albedo_color = Color(0.65, 0.88, 1.0, 0.6)
	warm_ripple_mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	warm_ripple_mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	warm_ripple.material_override = warm_ripple_mat
	warmup_node.add_child(warm_ripple)
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

	# Logs and vehicle bodies are rigid: one MultiMesh per lane draws every log
	# and every car, truck, dozer or racecar with four spinning wheel pivots,
	# instead of dozens of MeshInstance3D trees per lane.
	for lane in BatchedLaneModels:
		batched_lanes[lane] = BatchedLane.new(self, BatchedLaneModels[lane])

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
	surface.position = Vector3(0, BoardVisuals.WaterSurfaceHeight, -3)
	water = ShaderMaterial.new()
	water.shader = load("res://Shaders/river.gdshader")
	water.set_shader_parameter("compatibility_color", RenderingServer.get_current_rendering_method() == "gl_compatibility")
	surface.material_override = water
	add_child(surface)

func feed_audio() -> void:
	if simulation == null:
		return
	if not web_audio_checked:
		web_audio_checked = true
		if OS.has_feature("web"):
			web_audio = JavaScriptBridge.get_interface("FroggerAudio")
			if web_audio != null:
				web_audio.report_driver(AudioServer.get_driver_name())
	if web_audio != null and bool(web_audio.enabled):
		var audible: bool = started and not muted and not paused
		web_audio.set_active(audible)
		var count: int = simulation.get_sound_sample_count()
		if audible and count > 0:
			# The bridge accepts strings/JS objects, not PackedByteArray. One
			# bounded mono PCM copy per frame avoids per-sample bridge calls.
			var pcm: PackedFloat32Array = simulation.get_sound_samples(count)
			web_audio.push_pcm(Marshalls.raw_to_base64(pcm.to_byte_array()))
		else:
			simulation.clear_sound_samples()
		return
	if audio_player == null:
		audio_player = AudioStreamPlayer.new()
		var gen := AudioStreamGenerator.new()
		# The ROM synthesizer always produces 48 kHz, even when Safari's device
		# uses a different output rate. Godot performs the output resampling.
		gen.mix_rate_mode = AudioStreamGenerator.MIX_RATE_CUSTOM
		gen.mix_rate = 48000
		gen.buffer_length = 0.08
		audio_player.stream = gen
		audio_player.playback_type = AudioServer.PLAYBACK_TYPE_STREAM
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
		audio_capacity = audio_playback.get_frames_available() if audio_playback != null else 0
	if audio_playback == null:
		return
	var available: int = audio_playback.get_frames_available()
	# A suspended/blocked browser output or a simulation catch-up can leave
	# both queues full. Flush stale effects under the mixer lock; clear_buffer()
	# requires a stopped playback. Reuse it so suspended output cannot accumulate
	# retired playback objects waiting for the audio thread to consume them.
	if audio_capacity - available + sample_count > 2400: # 50 ms at 48 kHz.
		AudioServer.lock()
		audio_playback.stop()
		audio_playback.clear_buffer()
		audio_playback.start()
		AudioServer.unlock()
		available = audio_playback.get_frames_available()
		audio_capacity = available
	# Drain the producer every frame, retaining the newest samples on overflow.
	# Leaving the oldest samples in the native queue compounds output latency.
	if sample_count > 0:
		var buf: PackedVector2Array = simulation.get_stereo_sound_samples(sample_count)
		if buf.size() > available:
			buf = buf.slice(buf.size() - available)
		if not buf.is_empty():
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
		player_frog = str(c.get_value("play", "player_frog", "frog"))
		if player_frog not in ["frog", "lady_frog"]:
			player_frog = "frog"
		show_fps = bool(c.get_value("play", "show_fps", false))
		shadows_enabled = bool(c.get_value("play", "shadows_enabled", true))
		antialiasing_enabled = bool(c.get_value("play", "antialiasing_enabled", true))
		top_down_camera = bool(c.get_value("play", "top_down_camera", false))
		desktop_follow_zoom_percent = clampf(float(c.get_value("play", "desktop_follow_zoom_percent", 0.0)), -50.0, 100.0)
		mobile_follow_zoom_percent = clampf(float(c.get_value("play", "mobile_follow_zoom_percent", 0.0)), -50.0, 100.0)
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
	if fps_container != null:
		fps_container.visible = show_fps
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
	c.set_value("play", "player_frog", player_frog)
	c.set_value("play", "show_fps", show_fps)
	c.set_value("play", "shadows_enabled", shadows_enabled)
	c.set_value("play", "antialiasing_enabled", antialiasing_enabled)
	c.set_value("play", "perspective_view", perspective_view)
	c.set_value("play", "top_down_camera", top_down_camera)
	c.set_value("play", "follow_camera", follow_camera)
	c.set_value("play", "desktop_follow_zoom_percent", desktop_follow_zoom_percent)
	c.set_value("play", "mobile_follow_zoom_percent", mobile_follow_zoom_percent)
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
		s.border_width_top = 0
		s.corner_radius_bottom_left = 16
		s.corner_radius_bottom_right = 16
	else:
		s.border_width_bottom = 0
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
	# Touch emulation is enabled for swipes, so touchscreen_available also reports
	# true on a mouse-only desktop. Use the platform / browser pointer instead.
	touch_device = OS.has_feature("android") or OS.has_feature("ios") or OS.get_cmdline_user_args().has("--mobile-ui")
	if OS.has_feature("web"):
		touch_device = touch_device or bool(JavaScriptBridge.eval("window.matchMedia('(pointer: coarse)').matches"))
	last_input_method = "touch" if touch_device else "keyboard"
	if touch_device:
		# Match phone-sized logical UI units instead of shrinking a desktop canvas.
		get_tree().root.content_scale_size = Vector2i(480, 320)
		get_tree().root.content_scale_mode = Window.CONTENT_SCALE_MODE_CANVAS_ITEMS
	display_font = load("res://Fonts/PressStart2P-Regular.ttf")
	body_font = load("res://Fonts/Silkscreen-Regular.ttf")
	frog_icon_texture = load("res://frogger.svg")
	var ui := CanvasLayer.new()
	add_child(ui)

	fps_container = PanelContainer.new()
	var fps_style := StyleBoxFlat.new()
	fps_style.bg_color = Color(0.06, 0.09, 0.14, 0.85)
	fps_style.set_corner_radius_all(6)
	fps_style.content_margin_left = 10
	fps_style.content_margin_right = 10
	fps_style.content_margin_top = 6
	fps_style.content_margin_bottom = 6
	fps_container.add_theme_stylebox_override("panel", fps_style)
	fps_container.position = Vector2(16, 16)
	fps_container.visible = show_fps
	ui.add_child(fps_container)

	fps_label = Label.new()
	fps_label.add_theme_font_override("font", body_font)
	fps_label.add_theme_font_size_override("font_size", 13)
	fps_label.add_theme_color_override("font_color", Color("55ff77"))
	fps_container.add_child(fps_label)

	var root := Control.new()
	ui.add_child(root)
	root.mouse_filter = Control.MOUSE_FILTER_IGNORE

	var th := Theme.new()
	th.default_font = body_font
	th.default_font_size = 20 if touch_device else 17
	root.theme = th

	var frame_color := Color("202733")
	var header := PanelContainer.new()
	hud_header = header
	header.size = Vector2(870, 94)
	header.add_theme_stylebox_override("panel", edge_panel_style(frame_color, true))
	root.add_child(header)

	var row := HBoxContainer.new()
	row.alignment = BoxContainer.ALIGNMENT_CENTER
	row.add_theme_constant_override("separation", 12)
	header.add_child(row)

	for title in ["1-UP", "FROGGER", "HI-SCORE"]:
		var col := VBoxContainer.new()
		col.size_flags_horizontal = Control.SIZE_EXPAND_FILL
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
	hud_footer = bottom
	bottom.size = Vector2(870, 64)
	var footer_style := edge_panel_style(frame_color, false)
	footer_style.content_margin_left = 12
	footer_style.content_margin_right = 12
	bottom.add_theme_stylebox_override("panel", footer_style)
	root.add_child(bottom)

	var foot := HBoxContainer.new()
	foot.alignment = BoxContainer.ALIGNMENT_CENTER
	foot.add_theme_constant_override("separation", 12)
	bottom.add_child(foot)
	gameplay_options_button = Button.new()
	gameplay_options_button.icon = load("res://Icons/settings.svg")
	gameplay_options_button.tooltip_text = "Pause"
	gameplay_options_button.expand_icon = true
	gameplay_options_button.add_theme_constant_override("icon_max_width", 26)
	gameplay_options_button.custom_minimum_size = Vector2(52, 48)
	for button_state in ["normal", "hover", "pressed", "disabled"]:
		var gear_style := style_box(Color("6f914d") if button_state == "hover" else Color("526e3e"), 7)
		gear_style.content_margin_left = 12
		gear_style.content_margin_right = 12
		gear_style.content_margin_top = 10
		gear_style.content_margin_bottom = 10
		gameplay_options_button.add_theme_stylebox_override(button_state, gear_style)
	gameplay_options_button.pressed.connect(toggle_pause_from_gear)
	foot.add_child(gameplay_options_button)

	var before_extra := Control.new()
	before_extra.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	foot.add_child(before_extra)
	var extra_life_label := make_text("EXTRA LIFE\n20,000", 12, LimeColor)
	extra_life_label.name = "ExtraLifeLabel"
	extra_life_label.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	foot.add_child(extra_life_label)
	var before_lives := Control.new()
	before_lives.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	foot.add_child(before_lives)

	var lives_box := HBoxContainer.new()
	lives_box.name = "LivesBox"
	lives_box.alignment = BoxContainer.ALIGNMENT_CENTER
	lives_box.add_theme_constant_override("separation", 10)
	foot.add_child(lives_box)

	lives_label = make_text("FROGS", 19, LimeColor)
	lives_box.add_child(lives_label)

	lives_icons = HBoxContainer.new()
	lives_icons.add_theme_constant_override("separation", 5)
	lives_icons.alignment = BoxContainer.ALIGNMENT_CENTER
	lives_box.add_child(lives_icons)

	var before_timer := Control.new()
	before_timer.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	foot.add_child(before_timer)
	var timer_box := HBoxContainer.new()
	timer_box.name = "TimerBox"
	timer_box.add_theme_constant_override("separation", 12)
	foot.add_child(timer_box)
	timer_bar = ProgressBar.new()
	timer_bar.min_value = 0
	timer_bar.max_value = 100
	timer_bar.value = 100
	timer_bar.show_percentage = false
	timer_bar.custom_minimum_size = Vector2(70, 18)
	timer_bar.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	timer_bar.size_flags_vertical = Control.SIZE_SHRINK_CENTER
	timer_bar.add_theme_stylebox_override("background", style_box(Color("121a29"), 4))
	timer_bar.add_theme_stylebox_override("fill", style_box(Color("9ac75d"), 4))
	timer_box.add_child(timer_bar)
	var time_label := make_text("TIME", 19, Cream)
	timer_box.add_child(time_label)

	message_panel = PanelContainer.new()
	message_panel.visible = false
	message_panel.mouse_filter = Control.MOUSE_FILTER_IGNORE
	var msg_style = style_box(Color(0.06, 0.10, 0.17, 0.93), 12, 2)
	msg_style.border_color = Color("ffce57")
	msg_style.content_margin_top = ModalVerticalPadding
	msg_style.content_margin_bottom = ModalVerticalPadding
	message_panel.add_theme_stylebox_override("panel", msg_style)
	root.add_child(message_panel)
	var message_content := VBoxContainer.new()
	message_content.alignment = BoxContainer.ALIGNMENT_CENTER
	message_content.mouse_filter = Control.MOUSE_FILTER_IGNORE
	message_content.add_theme_constant_override("separation", 18)
	message_panel.add_child(message_content)

	message_label = make_text("", 30, Cream)
	message_label.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	message_label.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	message_label.mouse_filter = Control.MOUSE_FILTER_IGNORE
	message_label.add_theme_color_override("font_shadow_color", Color.BLACK)
	message_label.add_theme_constant_override("shadow_offset_y", 3)
	message_content.add_child(message_label)
	message_hint = make_text("", 18, Cream)
	message_hint.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	message_hint.mouse_filter = Control.MOUSE_FILTER_IGNORE
	message_hint.visible = false
	message_content.add_child(message_hint)

	menu = PanelContainer.new()
	menu.size = Vector2(440, 484)
	menu.add_theme_stylebox_override("panel", style_box(Color(0.075, 0.10, 0.15, 0.97), 18, 2))
	root.add_child(menu)
	menu_scroll = ScrollContainer.new()
	menu_scroll.horizontal_scroll_mode = ScrollContainer.SCROLL_MODE_DISABLED
	menu_scroll.follow_focus = true
	menu_scroll.scroll_deadzone = 12
	menu.add_child(menu_scroll)

	menu_items = VBoxContainer.new()
	menu_items.mouse_filter = Control.MOUSE_FILTER_PASS
	menu_items.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	menu_items.add_theme_constant_override("separation", 14)
	menu_scroll.add_child(menu_items)

	layout_ui = func():
		var viewport_size: Vector2 = get_viewport().get_visible_rect().size
		root.size = viewport_size
		var panel_width: float = minf(870.0, viewport_size.x)
		var narrow: bool = viewport_size.x < 650.0
		var portrait_footer: bool = touch_device and viewport_size.x < viewport_size.y
		var h: float = root.size.y
		header.size = Vector2(panel_width, 94)
		header.position = Vector2((viewport_size.x - panel_width) / 2.0, 0)
		for label in [score_label, high_label]:
			label.add_theme_font_size_override("font_size", 19 if narrow else 26)
		var title_label: Label = row.get_child(1).get_child(0)
		title_label.add_theme_font_size_override("font_size", 18 if narrow else 23)
		level_label.visible = not narrow
		lives_label.add_theme_font_size_override("font_size", 10 if narrow else 19)
		time_label.visible = not narrow
		extra_life_label.add_theme_font_size_override("font_size", 10 if narrow else 12)
		foot.add_theme_constant_override("separation", 0 if portrait_footer else (4 if narrow else 12))
		lives_box.add_theme_constant_override("separation", 4 if narrow else 10)
		lives_box.custom_minimum_size.x = 116.0 if narrow else 0.0
		for footer_spacer in [before_extra, before_lives, before_timer]:
			footer_spacer.size_flags_horizontal = Control.SIZE_SHRINK_BEGIN if portrait_footer else Control.SIZE_EXPAND_FILL
			footer_spacer.custom_minimum_size.x = PortraitFooterGap if portrait_footer else 0.0
		timer_box.size_flags_horizontal = Control.SIZE_EXPAND_FILL if portrait_footer else Control.SIZE_SHRINK_BEGIN
		timer_box.custom_minimum_size.x = 70.0 if narrow else panel_width * 0.32
		bottom.size = Vector2(panel_width, 80)
		bottom.position = Vector2((viewport_size.x - panel_width) / 2.0, h - bottom.size.y)
		message_label.add_theme_font_size_override("font_size", 18 if viewport_size.x < 620 else (24 if viewport_size.x < 900 else 30))
		update_message_layout()
		var menu_height: float = clampf(menu_items.get_combined_minimum_size().y + 28.0, 220.0, 620.0)
		menu.size = Vector2(minf(480.0 if touch_device else 520.0, viewport_size.x - 24.0), minf(menu_height, h - 40.0))
		menu.position = (viewport_size - menu.size) / 2.0
	layout_ui.call()
	get_viewport().size_changed.connect(layout_ui)

	bonus_overlay = Control.new()
	bonus_overlay.size = get_viewport().get_visible_rect().size
	bonus_overlay.mouse_filter = Control.MOUSE_FILTER_IGNORE
	ui.add_child(bonus_overlay)
	get_viewport().size_changed.connect(func(): bonus_overlay.size = get_viewport().get_visible_rect().size)

func clear_menu() -> void:
	menu.visible = true
	message_label.text = ""
	message_panel.visible = false
	reset_mouse_idle()
	touch_active = false
	swipe_direction = 0
	swipe_frames = 0
	for child in menu_items.get_children():
		menu_items.remove_child(child)
		child.queue_free()
	menu_scroll.scroll_vertical = 0
	layout_ui.call_deferred()

func enable_menu_swipe_scrolling(node: Node) -> void:
	# A child with STOP captures the emulated mouse drag before ScrollContainer
	# receives it. PASS keeps buttons and sliders tappable while letting the
	# parent handle a vertical swipe. Godot cancels button presses once scrolling.
	for child in node.get_children():
		if child is Control and child.mouse_filter == Control.MOUSE_FILTER_STOP:
			child.mouse_filter = Control.MOUSE_FILTER_PASS
		enable_menu_swipe_scrolling(child)

func show_menu(resume: bool) -> void:
	clear_menu()
	options_open = false
	paused = resume
	if resume:
		menu_items.add_child(make_text("PAUSED", 23, Cream))
		add_button("RESUME", resume_game)
		add_button("OPTIONS", func(): show_options(true))
		add_button("RETURN TO MAIN MENU", return_to_main_menu)
	else:
		menu_items.add_child(make_text("FROGGER", 30, Cream))
		setup_frog_selector(menu_items)
		add_button("ONE PLAYER", func(): start_game(1))
		add_button("TWO PLAYERS - TAKE TURNS", func(): start_game(2))
		add_button("OPTIONS", func(): show_options(false))
		var instructions := make_text("Swipe to hop" if touch_device else "Arrow keys / WASD / D-pad to hop", 14, Color("adb8cc"))
		menu_items.add_child(instructions)
	enable_menu_swipe_scrolling(menu_items)

func follow_zoom_percent() -> float:
	return mobile_follow_zoom_percent if touch_device else desktop_follow_zoom_percent

func set_follow_zoom_percent(value: float) -> void:
	if touch_device:
		mobile_follow_zoom_percent = clampf(value, -50.0, 100.0)
	else:
		desktop_follow_zoom_percent = clampf(value, -50.0, 100.0)
	save_preferences()

func add_follow_zoom_control(follow: CheckButton) -> void:
	var zoom_group := VBoxContainer.new()
	zoom_group.name = "FollowZoomControl"
	menu_items.add_child(zoom_group)
	var heading := HBoxContainer.new()
	zoom_group.add_child(heading)
	var label := make_text("", 18, Cream)
	label.horizontal_alignment = HORIZONTAL_ALIGNMENT_LEFT
	label.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	heading.add_child(label)
	var reset := Button.new()
	reset.name = "FollowZoomDefault"
	reset.text = "DEFAULT"
	reset.custom_minimum_size.y = 40
	heading.add_child(reset)
	var slider := HSlider.new()
	slider.name = "FollowZoomSlider"
	slider.min_value = -50.0
	slider.max_value = 100.0
	slider.step = 5.0
	slider.value = follow_zoom_percent()
	slider.custom_minimum_size.y = 40
	zoom_group.add_child(slider)
	var update_control := func():
		var value: float = follow_zoom_percent()
		label.text = "Follow zoom: %s%d%%" % ["+" if value > 0.0 else "", int(value)]
		slider.editable = follow_camera
		reset.disabled = not follow_camera or is_zero_approx(value)
		zoom_group.modulate.a = 1.0 if follow_camera else 0.45
	slider.value_changed.connect(func(value: float):
		set_follow_zoom_percent(value)
		update_control.call())
	reset.pressed.connect(func(): slider.value = 0.0)
	follow.toggled.connect(func(_enabled: bool): update_control.call())
	update_control.call()

func return_to_main_menu() -> void:
	started = false
	paused = false
	coin_frames = 0
	reset_machine()
	for i in range(180):
		simulation.step()
	observe_frame()
	simulation.clear_sound_samples()
	message_label.text = ""
	message_panel.visible = false
	show_menu(false)

func show_options(return_to_pause: bool) -> void:
	clear_menu()
	options_open = true
	options_return_to_pause = return_to_pause
	paused = started
	menu_items.add_child(make_text("OPTIONS", 23, Cream))
	add_button("BACK", func(): show_menu(options_return_to_pause))

	var collision := CheckButton.new()
	collision.text = "Classic collision"
	collision.button_pressed = not modern
	collision.toggled.connect(func(val):
		modern = not val
		if simulation != null:
			simulation.modern(modern)
		save_preferences())
	menu_items.add_child(collision)

	var perspective := CheckButton.new()
	perspective.text = "Perspective view"
	perspective.tooltip_text = "Off uses orthographic projection"
	perspective.button_pressed = perspective_view
	perspective.toggled.connect(func(val):
		perspective_view = val
		save_preferences())
	menu_items.add_child(perspective)
	var top_down := CheckButton.new()
	top_down.text = "Top-down camera"
	top_down.button_pressed = top_down_camera
	top_down.toggled.connect(func(val):
		top_down_camera = val
		save_preferences())
	menu_items.add_child(top_down)

	var follow := CheckButton.new()
	follow.text = "Follow frog (closer view)"
	follow.button_pressed = follow_camera
	follow.toggled.connect(func(val):
		follow_camera = val
		save_preferences())
	menu_items.add_child(follow)
	add_follow_zoom_control(follow)

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

	var fps_btn := CheckButton.new()
	fps_btn.text = "FPS Counter"
	fps_btn.button_pressed = show_fps
	fps_btn.toggled.connect(func(val):
		show_fps = val
		if fps_container != null:
			fps_container.visible = show_fps
		save_preferences())
	menu_items.add_child(fps_btn)

	var sound_btn := CheckButton.new()
	sound_btn.text = "Sound"
	sound_btn.button_pressed = not muted
	sound_btn.toggled.connect(func(val):
		muted = not val
		save_preferences())
	menu_items.add_child(sound_btn)

	for child in menu_items.get_children():
		if child is CheckButton:
			child.custom_minimum_size.y = 48.0 if touch_device else 34.0
	enable_menu_swipe_scrolling(menu_items)

func add_button(text_val: String, action: Callable) -> void:
	var b := Button.new()
	b.text = text_val
	b.custom_minimum_size = Vector2(0, 52 if touch_device else 43)
	b.add_theme_font_size_override("font_size", 17)
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

func update_message_layout() -> void:
	var viewport_size: Vector2 = get_viewport().get_visible_rect().size
	var text_width: float = 0.0
	var labels: Array = [message_label]
	if not message_hint.text.is_empty():
		labels.append(message_hint)
	for label in labels:
		var font: Font = label.get_theme_font("font")
		text_width = maxf(text_width, font.get_string_size(label.text, HORIZONTAL_ALIGNMENT_LEFT, -1, label.get_theme_font_size("font_size")).x)
	var width: float = minf(text_width + 44.0, viewport_size.x - 48.0)
	var height: float = ModalVerticalPadding * 2.0 + 18.0 * float(labels.size() - 1)
	for label in labels:
		var font: Font = label.get_theme_font("font")
		height += font.get_multiline_string_size(label.text, HORIZONTAL_ALIGNMENT_LEFT, width - 44.0, label.get_theme_font_size("font_size")).y
	message_panel.size = Vector2(width, height)
	var center: Vector2 = viewport_size * 0.5
	if follow_camera and (level_intro_waiting or level_intro_fade > 0.0):
		center.y = viewport_size.y / 3.0
	message_panel.position = center - message_panel.size * 0.5
	# Reserve a separate row for every time-bonus panel, even on the first
	# frame and after a camera or viewport change. Follow placement is shared
	# by desktop, web and mobile, independent of projection and touch input.
	var intro_visible: bool = level_intro_waiting or level_intro_fade > 0.0
	var upper_edge: float = message_panel.get_global_rect().position.y
	var lower_edge: float = message_panel.get_global_rect().end.y
	var safe_top: float = hud_header.get_global_rect().end.y + 16.0
	for popup in popups:
		if popup.award.get("kind", 0) != 2:
			continue
		popup.view.size = popup.view.get_combined_minimum_size()
		var top: float = safe_top if follow_camera else (viewport_size.y - popup.view.size.y) * 0.5
		if intro_visible:
			if upper_edge - ModalGap - popup.view.size.y >= safe_top:
				top = upper_edge - ModalGap - popup.view.size.y
				upper_edge = top
			else:
				top = lower_edge + ModalGap
				lower_edge = top + popup.view.size.y
		popup.view.global_position = Vector2((viewport_size.x - popup.view.size.x) * 0.5, top)

func observe_level_intro() -> void:
	if not started or BoardVisuals.at(state, 0x83fe) == 0:
		return
	var player: int = BoardVisuals.at(state, 0x83fd)
	var level: int = BoardVisuals.at(state, 0x83b7)
	# The per-player home counter is incremented by the actual home award.
	# Announce the upcoming board here, while the completed board is visible.
	var homes: int = BoardVisuals.at(state, 0x825d if player == 2 else 0x825c)
	if homes == 5 and level_intro_home_counts.get(player, 0) != 5:
		level_intro_player = player
		level_intro_number = level + 1
		level_intro_waiting = true
		level_intro_fade = 0.0
	level_intro_home_counts[player] = homes
	if level_intro_waiting and player == level_intro_player and level == level_intro_number:
		# This is the former show trigger. Do not wait for respawn timers.
		level_intro_waiting = false
	if player != level_intro_player:
		level_intro_waiting = false
		level_intro_fade = 0.0

func update_level_intro() -> void:
	if not started or paused:
		return
	if level_intro_waiting:
		level_intro_fade = minf(1.0, level_intro_fade + presentation_delta / LevelIntroFadeIn)
	else:
		level_intro_fade = maxf(0.0, level_intro_fade - presentation_delta / LevelIntroFadeOut)

func update_hud() -> void:
	# Keep the gear interactive and in place during pause so it can resume play.
	gameplay_options_button.visible = started
	gameplay_options_button.tooltip_text = "Resume" if paused else "Pause"
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
	lives_label.text = "FROGS"
	var count: int = clampi(BoardVisuals.at(state, 0x83e5 if player == 1 else 0x83e6), 0, 12)
	var narrow_footer: bool = get_viewport().get_visible_rect().size.x < 650.0
	# Keep the life meter identifiable on a player's last frog. With three or
	# more reserve icons, reclaim the label width so the portrait row still fits.
	lives_label.visible = not narrow_footer or count <= 2
	if lives_icons != null:
		while lives_icons.get_child_count() < count:
			var icon := TextureRect.new()
			icon.texture = frog_icon_texture
			icon.expand_mode = TextureRect.EXPAND_IGNORE_SIZE
			icon.stretch_mode = TextureRect.STRETCH_KEEP_ASPECT_CENTERED
			icon.custom_minimum_size = Vector2(36, 36)
			lives_icons.add_child(icon)
		for i in range(lives_icons.get_child_count()):
			lives_icons.get_child(i).visible = (i < count)
	timer_bar.value = clampf(float(BoardVisuals.at(state, 0x83dd)) / 60.0, 0.0, 1.0) * 100.0
	message_hint.text = ""
	message_panel.modulate.a = 1.0
	update_level_intro()
	if started and not paused:
		if BoardVisuals.at(state, 0x83fe) == 0:
			message_label.text = "GAME OVER"
			message_hint.text = restart_prompt()
		elif level_intro_waiting or level_intro_fade > 0.0:
			message_label.text = "LEVEL %d" % level_intro_number
			message_hint.text = "GET READY" if level_intro_waiting else "GO"
			message_panel.modulate.a = level_intro_fade
		else:
			message_label.text = ""
	message_panel.visible = message_label.text.length() > 0
	message_hint.visible = not message_hint.text.is_empty()
	update_message_layout()
	if fps_label != null and show_fps:
		var fps: float = Engine.get_frames_per_second()
		var frame_ms: float = presentation_delta * 1000.0
		var sim_ms: float = prof_sim_us / 1000.0
		var audio_ms: float = prof_audio_us / 1000.0
		var actors_ms: float = prof_actors_us / 1000.0
		var script_ms: float = prof_total_script_us / 1000.0
		var gpu_ms: float = maxf(0.0, frame_ms - script_ms)
		fps_label.text = "FPS: %3d (%4.1f ms)\nSim:   %4.1f ms (%d step%s)\nAudio: %4.1f ms\nActors:%4.1f ms\nGPU:   %4.1f ms" % [
			int(fps), frame_ms,
			sim_ms, prof_last_steps, ("s" if prof_last_steps != 1 else " "),
			audio_ms,
			actors_ms,
			gpu_ms
		]

func bcd_score(addr: int) -> int:
	var low: int = BoardVisuals.at(state, addr)
	var high: int = BoardVisuals.at(state, addr + 1)
	return 10 * ((low & 15) + 10 * (low >> 4) + 100 * (high & 15) + 1000 * (high >> 4))

func update_camera(delta: float) -> void:
	var view_size: Vector2 = get_viewport().get_visible_rect().size
	var resized: bool = view_size != camera_view_size
	camera_view_size = view_size
	var aspect: float = maxf(0.1 if touch_device else 0.55, view_size.x / maxf(1.0, view_size.y))
	var portrait: bool = touch_device and aspect < 1.0
	var portrait_follow: bool = portrait and follow_camera and started
	var entering_portrait_follow: bool = portrait_follow and camera.keep_aspect != Camera3D.KEEP_WIDTH
	var horizontal_follow_limit: float = 4.0 if portrait_follow else 2.5
	camera.projection = Camera3D.PROJECTION_PERSPECTIVE if perspective_view else Camera3D.PROJECTION_ORTHOGONAL
	camera.keep_aspect = Camera3D.KEEP_WIDTH if portrait_follow else Camera3D.KEEP_HEIGHT
	camera.size = 9.8 if (follow_camera and started) else maxf(16.8, 16.2 / aspect)
	if portrait_follow and not perspective_view:
		camera.size = 10.4
	# Frame roughly nine nearby columns on phones. Follow toward each side as
	# the frog approaches, keeping the action large rather than fitting all 14.
	camera.fov = 48.0 if portrait_follow else (54.0 if (follow_camera and started) else 52.0)

	var holding_home: bool = portrait_follow and home_arrival.active(state.get("frame", 0), render_fraction())
	var tracking: bool = follow_camera and started and (holding_home or FrogVisualState.player_on_board(state))
	var tx: float = 0.0
	var tz: float = 0.0
	var followed_row: float = 0.0
	if tracking:
		var x: float = float(frog_visual.death_x) if frog_visual.dying else displayed_frog_x
		var row: float = float(frog_visual.death_row) if frog_visual.dying else displayed_frog_row
		if holding_home:
			x = home_arrival.visual_x(state.get("frame", 0), render_fraction())
			row = home_arrival.visual_row(state.get("frame", 0), render_fraction())
		var p: Vector3 = pos3(x, row)
		followed_row = p.z
		tx = clampf(p.x, -horizontal_follow_limit, horizontal_follow_limit)
		tz = p.z if portrait_follow else clampf(p.z, -4.0, 4.0)
	elif follow_camera and started:
		tx = clampf(camera.position.x, -horizontal_follow_limit, horizontal_follow_limit)
		if portrait_follow or top_down_camera or not is_zero_approx(follow_zoom_percent()):
			# Retain the followed row while the player is temporarily off board.
			tz = camera_look_target.z if portrait_follow else clampf(camera_look_target.z + 0.80, -4.0, 4.0)
		else:
			tz = clampf(camera.position.z - (5.8 if perspective_view else 6.5), -4.0, 4.0)
		followed_row = tz
	var desired: Vector3
	if tracking or (follow_camera and started):
		desired = Vector3(tx, 9.0 if perspective_view else 11.2, tz + (5.8 if perspective_view else 6.5))
	else:
		desired = Vector3(0, 15.5, 9.5) if perspective_view else Vector3(0, 19, 9.8)
	var desired_look: Vector3 = Vector3(tx, 0, tz - 0.80) if (tracking or (follow_camera and started)) else Vector3(0, 0, -0.12)
	if portrait_follow:
		# Center vertically on the frog while preserving the existing follow
		# distance and angle. No board-edge limits or projected-image offsets.
		desired.z += 0.80
		desired_look.z = tz
	if top_down_camera:
		# Angle and projection are independent: either projection can look down.
		var height: float = desired.distance_to(desired_look) if perspective_view else desired.y
		desired = desired_look + Vector3.UP * height
	elif portrait and perspective_view:
		# Less board tilt on a tall screen: 22 degrees from overhead. Keep the
		# horizontal follow scale while giving the lanes more vertical space.
		var distance: float = desired.distance_to(desired_look)
		desired = desired_look + Vector3(0, cos(deg_to_rad(22.0)), sin(deg_to_rad(22.0))) * distance
	if follow_camera and started and not is_zero_approx(follow_zoom_percent()):
		var zoom_factor: float = 1.0 + follow_zoom_percent() / 100.0
		if perspective_view:
			desired = desired_look + (desired - desired_look) / zoom_factor
		else:
			camera.size /= zoom_factor
	if portrait_follow:
		var pan_limit: float = portrait_follow_pan_limit(desired, desired_look, followed_row)
		desired.x = clampf(desired.x, -pan_limit, pan_limit)
		desired_look.x = clampf(desired_look.x, -pan_limit, pan_limit)
	if not (follow_camera and started):
		var framing := fit_board_overview(desired, desired_look, view_size, portrait)
		desired = framing["position"]
		desired_look = framing["target"]
		camera.size = framing["size"]
	if entering_portrait_follow or (resized and not (follow_camera and started)):
		# Keep overview fitting immediate on resize, and switch portrait follow
		# projection and framing together on start or phone rotation.
		camera.position = desired
		camera_look_target = desired_look
	var responsiveness: float = 1.0 - exp(-6.0 * minf(delta, 0.1))
	camera.position = camera.position.lerp(desired, responsiveness)
	camera_look_target = camera_look_target.lerp(desired_look, responsiveness)
	if portrait_follow:
		var pan_limit: float = portrait_follow_pan_limit(camera.position, camera_look_target, followed_row)
		camera.position.x = clampf(camera.position.x, -pan_limit, pan_limit)
		camera_look_target.x = clampf(camera_look_target.x, -pan_limit, pan_limit)
	camera.look_at(camera_look_target, Vector3.FORWARD if top_down_camera else Vector3.UP)
	update_camera_clip_planes()

func portrait_follow_pan_limit(position: Vector3, target: Vector3, row: float) -> float:
	# Keep the approached board edge within the outer 16% of the screen
	# (the marked margin), adapting to projection, angle and follow zoom.
	var visible_width: float = camera.size
	if perspective_view:
		var backward := (position - target).normalized()
		var edge := Vector3(0, 0.1, clampf(row, -7.0, 7.0))
		var depth: float = (position - edge).dot(backward)
		visible_width = 2.0 * tan(deg_to_rad(camera.fov * 0.5)) * depth
	return clampf(7.4 - visible_width * 0.34, 0.0, 4.0)

func update_camera_clip_planes() -> void:
	# Orthographic directional shadows use the camera's depth range. The default
	# 4000-unit far plane wastes their resolution on empty space around this board.
	# Include the entire board, actors and hidden shader warmup models, and follow
	# the actual camera position through overview fitting and zoom transitions.
	var scene_distance: float = camera.position.length()
	camera.near = maxf(0.1, scene_distance - 16.0)
	camera.far = scene_distance + 16.0

func fit_board_overview(position: Vector3, target: Vector3, view_size: Vector2, crop_frame: bool) -> Dictionary:
	# Fit the board into the live HUD opening, preserving the chosen angle.
	# Portrait phones prioritize playable lanes over the decorative wooden frame.
	var backward := (position - target).normalized()
	var up := backward.cross(Vector3.RIGHT).normalized()
	var slope: float = tan(deg_to_rad(camera.fov * 0.5))
	# Desktop can tuck the decorative top/bottom edges behind the HUD for a
	# closer view; touch layouts keep a small gap around the playable region.
	var top_padding: float = 6.0 if touch_device else -18.0
	var bottom_padding: float = 6.0 if touch_device else -44.0
	var top: float = minf(hud_header.get_global_rect().end.y + top_padding, view_size.y * 0.4)
	var bottom: float = maxf(hud_footer.global_position.y - bottom_padding, view_size.y * 0.6)
	var top_slope: float = (1.0 - 2.0 * top / view_size.y) * slope
	var bottom_slope: float = (1.0 - 2.0 * bottom / view_size.y) * slope
	var available_width: float = maxf(1.0, view_size.x - (8.0 if crop_frame else 12.0))
	var side_slope: float = slope * available_width / view_size.y
	var upper_bound: float = -INF
	var lower_bound: float = INF
	var low_height: float = INF
	var high_height: float = -INF
	var distance: float = 1.0
	var half_width: float = 7.1 if crop_frame else 7.45
	# Actual silhouette: only the rear hedge is tall, and the deep river bed
	# is not at the front. Empty bounding-box corners would force a loose fit.
	var edges: Array[Vector2] = [Vector2(-0.94, 7.90), Vector2(0.20, 7.90), Vector2(-0.94, -7.70), Vector2(0.20, -7.70), Vector2(1.30, -7.25)]
	if crop_frame:
		edges = [Vector2(0.1, 7.0), Vector2(0.1, -7.0), Vector2(1.30, -7.25)]
	for x in [-half_width, half_width]:
		for edge in edges:
			var point := Vector3(x, edge.x, edge.y) - target
			var depth: float = point.dot(backward)
			var height: float = point.dot(up)
			distance = maxf(distance, depth + absf(point.x) / side_slope)
			upper_bound = maxf(upper_bound, height + top_slope * depth)
			lower_bound = minf(lower_bound, height + bottom_slope * depth)
			low_height = minf(low_height, height)
			high_height = maxf(high_height, height)
	var size: float = maxf(2.0 * half_width * view_size.y / available_width, (high_height - low_height) * view_size.y / (bottom - top))
	var offset: float
	if perspective_view:
		distance = maxf(distance, (upper_bound - lower_bound) / (top_slope - bottom_slope))
		offset = ((upper_bound - top_slope * distance) + (lower_bound - bottom_slope * distance)) * 0.5
	else:
		distance = position.distance_to(target)
		var center_ndc: float = 1.0 - (top + bottom) / view_size.y
		offset = (low_height + high_height) * 0.5 - center_ndc * size * 0.5
	var centered_target: Vector3 = target + up * offset
	return {"position": centered_target + backward * distance, "target": centered_target, "size": size}

func observe_frame() -> void:
	state = simulation.snapshot()
	observe_level_intro()
	river_gator_visual.observe(state)
	frog_visual.observe(state)
	lady_visual.observe(state, func(addr): return simulation.peek(addr))
	track_lady_hop()

func clear_presentation() -> void:
	frog_visual.reset()
	lady_visual.reset()
	home_arrival.reset()
	moving_visuals.reset()
	snake_visuals.clear()
	river_gator_visual.reset()
	frog_motion.reset()
	for lane in batched_lanes.values():
		lane.reset()
	for gator in home_gator_visuals:
		gator.reset()
	facing = 2
	lady_facing = 2
	lady_hop_start_frame = -1
	lady_last_motion_frame = -1
	anchored_death_frame = -1
	last_live_player_frame = -1
	level_intro_player = 0
	level_intro_home_counts.clear()
	level_intro_waiting = false
	level_intro_fade = 0.0
	beaver_phase = BeaverVisualPhase.Hidden
	beaver_phase_seconds = 0.0
	beaver_player_hit = false
	beaver_last_x = 120.0
	beaver_last_row = 80.0
	beaver_heading = 1
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
	var player = actor("player", active_frog_model())
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
			passenger = ModelActor.new(player.passenger_socket, rescue_frog_model())
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
	var cur_frame: int = state.get("frame", 0)
	# Rigid wheels share one turn clock; the per-lane offset keeps parallel
	# traffic from spinning in lockstep.
	var wheel_turn: float = TAU * fmod(float(cur_frame) * FRAME_SECONDS, 1.0)
	for lane in range(11):
		if lane == 5:
			continue
		var table: int = 0x8100 + lane * 9
		var count: int = mini(8, BoardVisuals.at(state, table))
		if lane in [1, 4]:
			count = mini(count, BoardVisuals.at(state, 0x8275 if lane == 1 else 0x827e))
		var row: int = (lane + 3) * 16
		var width: int = LaneWidths[lane]
		var model: String = LaneModels[lane]
		var is_turtle: bool = model == "turtle"
		var batch: BatchedLane = batched_lanes.get(lane)
		if batch != null:
			batch.begin(wheel_turn + float(lane) * 0.37)
		for index in range(count):
			var raw_center: float = float(BoardVisuals.at(state, table + index + 1) - (12 if lane < 5 else 3)) - float(width) / 2.0
			var center: float = moving_visuals.step(lane * 16 + index, raw_center, cur_frame, presentation_delta, paused)
			var members: int = 2 if lane == 1 else (3 if lane == 4 else 1)
			# ROM 0x20fb/0x219c stamps only the last group in each turtle
			# layout. Empty water is not evidence that a group can dive.
			var diver: bool = is_turtle and index == BoardVisuals.at(state, 0x8275 if lane == 1 else 0x827e) - 1
			var depth: float = BoardVisuals.turtle_depth(state, raw_center, row, fraction, diver) if diver else 0.0
			if is_turtle and not diver and BoardVisuals.turtle_phase(state, raw_center, row) == 2:
				continue
			for member in range(members):
				for wrap in [-1, 0, 1]:
					var x: float = center + float(wrap) * 256.0 + (float(member) - float(members - 1) / 2.0) * 16.0
					var crocodile: bool = lane == 0 and index == 0 and river_gator_visual.is_gator(x + 12.0 + float(width) / 2.0)
					if is_turtle:
						turtle_supports.append({"x": x, "row": row, "depth": depth})
					var half_width: float = (float(width) - 3.0) * 0.52 if (lane < 5 and not is_turtle) else (9.0 if is_turtle else (15.0 if lane == 6 else 10.0))
					var gator_fit = BoardVisuals.fit_river_gator(width) if crocodile else {}
					var render_x: float = x + (gator_fit.get("center_offset_pixels", 0.0) if crocodile else 0.0)
					var bounds_center: float = render_x
					if crocodile:
						half_width = BoardVisuals.RiverGatorLengthPixels / 2.0
						bounds_center = x + float(width) / 2.0 + 12.0 - BoardVisuals.RiverGatorTipInset - half_width
					if not BoardVisuals.intersects_playfield(bounds_center, half_width):
						continue
					if batch != null and not crocodile:
						batch.place(batched_lane_transform(lane, width, model, render_x, float(row)))
						continue
					var actor_key: String = "lane%d.%d.%d.%d.%s" % [lane, index, member, wrap, str(crocodile)]
					var obj = actor(actor_key, "river_gator" if crocodile else model)
					obj.set_active(true)
					var straddling_edge: bool = absf(bounds_center - 120.0) + half_width > 110.0
					obj.set_clipped(straddling_edge)
					obj.root.position = pos3(render_x, float(row), ((-0.22 if is_turtle else -0.18) if lane < 5 else 0.02))
					if crocodile:
						obj.root.rotation = Vector3(0, PI / 2.0, 0)
						obj.root.scale = Vector3(gator_fit["width_scale"], 0.9, gator_fit["length_scale"])
						obj.play("Bite")
					elif model == "log":
						obj.root.scale = Vector3((float(width) - 3.0) / 16.0, 1.0, 1.0)
					elif is_turtle:
						obj.root.position += Vector3(0, -depth, 0)
						obj.root.rotation = Vector3(0, -PI / 2.0, 0)
						obj.play("Dive" if depth > 0.01 else "Swim")
					else:
						obj.root.rotation = Vector3(0, (-1.0 if lane % 2 == 0 else 1.0) * PI / 2.0, 0)
						obj.root.scale = Vector3.ONE * 0.84
						obj.play("Move")
		if batch != null:
			batch.finish()

func batched_lane_transform(lane: int, width: int, model: String, render_x: float, row: float) -> Transform3D:
	if model == "log":
		# Logs keep their authored length axis on X and stretch to the lane slot;
		# the batched shader cuts them at the board edge exactly like a clipped
		# ModelActor would.
		return Transform3D(Basis().scaled(Vector3((float(width) - 3.0) / 16.0, 1.0, 1.0)), pos3(render_x, row, -0.18))
	var yaw: float = (-1.0 if lane % 2 == 0 else 1.0) * PI / 2.0
	return Transform3D(Basis(Vector3.UP, yaw).scaled(Vector3.ONE * 0.84), pos3(render_x, row, 0.02))

func turtle_ride_depth(x: float, row: float) -> float:
	var nearest: float = 10.0
	var depth: float = 0.0
	for support in turtle_supports:
		var distance: float = absf(x - support["x"])
		if absf(row - float(support["row"])) <= 7.5 and distance < nearest:
			nearest = distance
			depth = support["depth"]
	return depth if nearest <= 10.0 else 0.0

func snake_support(x: float, row: int) -> Vector4:
	# Center and local end points of the same inset, smoothed log drawn by
	# update_lanes. Store the trail in that coordinate system so drift cannot
	# pull the turning tail off its support.
	if row >= 128:
		return Vector4(120.0, -112.0, 112.0, 120.0)
	var lane: int = clampi(row / 16 - 3, 0, 4)
	var width: float = float(LaneWidths[lane])
	var table: int = 0x8100 + lane * 9
	var nearest: float = INF
	var center: float = x
	var raw_center: float = x
	for index in range(mini(8, BoardVisuals.at(state, table))):
		var native_center: float = float(BoardVisuals.at(state, table + index + 1) - 12) - width / 2.0
		var displayed = moving_visuals.display_x(lane * 16 + index)
		var candidate: float = native_center if displayed == null else float(displayed)
		candidate += 256.0 * roundf((x - candidate) / 256.0)
		if absf(candidate - x) < nearest:
			nearest = absf(candidate - x)
			center = candidate
			raw_center = native_center + 256.0 * roundf((x - native_center) / 256.0)
	return Vector4(center, -(width - 3.0) / 2.0, (width - 3.0) / 2.0, raw_center)

func update_homes_and_hazards() -> void:
	var home_base: int = 0x8263 if BoardVisuals.at(state, 0x83fd) == 2 else 0x825e
	var cur_frame: int = state.get("frame", 0)
	var frac: float = render_fraction()
	for i in range(5):
		var held_here: bool = home_arrival.active(cur_frame, frac) and absf(float(home_arrival.x) - (24.0 + 48.0 * float(i))) <= 8.0
		if BoardVisuals.at(state, home_base + i) != 0 and not held_here:
			var a = actor("home%d" % i, active_frog_model())
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

	for addr in [0x8048, 0x8050]:
		var x: int = BoardVisuals.at(state, addr)
		var y: int = BoardVisuals.at(state, addr + 3)
		if x < 8 or x > 235 or y < 32 or y > 136 or BoardVisuals.at(state, addr + 1) == 0:
			snake_visuals.erase(addr)
			continue
		var a = actor("hazard%d" % addr, "snake")
		a.set_active(true)
		# Spawn rows 0x4e/0x7e (ROM 0x2ace/0x2aaa) are the sprite's
		# inset row; the log and bank surfaces are centered at 0x50/0x80.
		var snake_row: float = float(y + 2)
		var surface: float = -0.18 + ModelFootprints.LogTopTiles if snake_row < 128.0 else BoardVisuals.surface_height(snake_row)
		var height: float = surface - 0.75 * ModelFootprints.SnakeBottomTiles + 0.008
		var motion_id: int = 1000 + addr
		# ROM 0x2a16..0x2a38 controls travel relative to the log: bit 7
		# set crawls right, clear crawls left. World velocity includes the
		# log's drift and can point opposite to the snake's own movement.
		# The authored head points along +Z, so positive yaw faces right.
		var heading: int = 1 if (BoardVisuals.at(state, addr + 1) & 0x80) != 0 else -1
		# ROM 0x2b23..0x2b30 places a second, tail sprite 15 pixels from
		# this head slot. Anchor the model's HEAD here; its tail follows the
		# head's trail on the moving log, curling around when the ROM turns.
		var head_x: float = moving_visuals.step(motion_id, float(x), cur_frame, presentation_delta, paused)
		var support: Vector4 = snake_support(head_x, int(snake_row))
		if not snake_visuals.has(addr):
			snake_visuals[addr] = SnakeVisual.new()
		var visual: SnakeVisual = snake_visuals[addr]
		var relative_x: float = float(x) - support.w
		relative_x -= 256.0 * roundf(relative_x / 256.0)
		visual.update(a, relative_x / 16.0, heading, pos3(support.x, snake_row, height),
			support.y / 16.0, support.z / 16.0, (float(cur_frame) + frac) * FRAME_SECONDS, snake_row < 128.0)

	update_beaver(cur_frame)

	if lady_visual.visible(ModelFootprints.FrogAlongX * LadyInRiverScale):
		var a = actor("lady", rescue_frog_model())
		a.set_active(true)
		a.root.position = pos3(moving_visuals.step(50000, lady_visual.x, cur_frame, presentation_delta, paused),
			float(BoardVisuals.LadyFrogRow), BoardVisuals.lady_frog_height(LadyInRiverScale))
		a.root.scale = Vector3.ONE * LadyInRiverScale
		a.root.rotation = Vector3(0, float(lady_facing) * PI / 2.0, 0)
		if lady_hop_start_frame > 0 and lady_last_motion_frame >= lady_hop_start_frame and cur_frame - lady_hop_start_frame < 12:
			a.pose("Hop", clampf((float(cur_frame - lady_hop_start_frame) + frac) / 12.0, 0.0, 1.0) * (10.0 / 60.0))
		else:
			a.play("Idle")

func begin_beaver_log_exit(player_hit: bool) -> void:
	beaver_phase = BeaverVisualPhase.Grab
	beaver_phase_seconds = 0.0
	beaver_player_hit = player_hit

func nearest_log_end(row: float, heading: int, around_x: float) -> Variant:
	var lane: int = int(roundf(row / 16.0)) - 3
	if lane < 0 or lane >= LaneModels.size() or LaneModels[lane] != "log":
		return null
	var table: int = 0x8100 + lane * 9
	var width: int = LaneWidths[lane]
	var half_width: float = (float(width) - 3.0) * 0.5
	# The authored end caps extend beyond the nominal half-tile log body.
	var batch: BatchedLane = batched_lanes.get(lane)
	if batch != null and batch.body != null:
		var bounds: AABB = batch.body.multimesh.mesh.get_aabb()
		half_width = (float(width) - 3.0) * maxf(absf(bounds.position.x), absf(bounds.end.x))
	var best: Variant = null
	var best_distance: float = 999.0
	for index in range(mini(8, BoardVisuals.at(state, table))):
		var center: float = float(BoardVisuals.at(state, table + index + 1) - 12) - float(width) / 2.0
		var displayed_center: Variant = moving_visuals.display_x(lane * 16 + index)
		if displayed_center != null:
			center = float(displayed_center)
		for wrap in [-1, 0, 1]:
			var wrapped_center: float = center + float(wrap) * 256.0
			# A left-facing beaver approaches the right end and vice versa.
			var end_x: float = wrapped_center + (half_width if heading < 0 else -half_width)
			var distance: float = absf(end_x - around_x)
			if distance < best_distance:
				best_distance = distance
				best = end_x
	return best

func update_beaver(cur_frame: int) -> void:
	var record_base: int = 0x8490 if BoardVisuals.at(state, 0x83fd) == 2 else 0x8480
	var native_state: int = BoardVisuals.at(state, record_base + 6)
	var slot_x: int = BoardVisuals.at(state, 0x8058)
	var slot_y: int = BoardVisuals.at(state, 0x805b)
	var slot_code: int = BoardVisuals.at(state, 0x8059)
	var descriptor_visible: bool = slot_x >= 8 and slot_x <= 235 and slot_y >= 32 and slot_y <= 136 and slot_code != 0

	if descriptor_visible and beaver_phase in [BeaverVisualPhase.Hidden, BeaverVisualPhase.Done, BeaverVisualPhase.Approach]:
		var displayed_slot_x: float = moving_visuals.step(1000 + 0x8058, float(slot_x), cur_frame, presentation_delta, paused)
		beaver_heading = -1 if (slot_code & 0x80) != 0 else 1
		# The native lethal probes are slot X+20 facing right and X-4 facing
		# left. Put the model's nose on that exact point so its torso never
		# phases into the log while its paws and incisors meet the end.
		var attack_x: float = displayed_slot_x + (20.0 if beaver_heading > 0 else -4.0)
		beaver_last_x = attack_x - float(beaver_heading) * 16.0 * BeaverScale * ModelFootprints.BeaverFrontTiles
		beaver_last_row = float(slot_y)
		var log_end: Variant = nearest_log_end(beaver_last_row, beaver_heading, attack_x)
		if log_end != null and (attack_x - float(log_end)) * float(beaver_heading) >= 0.0:
			beaver_last_x = float(log_end) - float(beaver_heading) * 16.0 * BeaverScale * ModelFootprints.BeaverFrontTiles
		if beaver_phase in [BeaverVisualPhase.Hidden, BeaverVisualPhase.Done] and native_state == 1:
			beaver_phase = BeaverVisualPhase.Approach
			beaver_phase_seconds = 0.0

	if beaver_phase == BeaverVisualPhase.Approach:
		if native_state >= 2:
			begin_beaver_log_exit(true)
		elif native_state == 0:
			# With no frog at the end the ROM clears the descriptor immediately.
			# Preserve only a short presentation ghost for grab/look/sink.
			begin_beaver_log_exit(false)

	if beaver_phase == BeaverVisualPhase.Hidden:
		if descriptor_visible and native_state >= 2:
			begin_beaver_log_exit(true)
		else:
			return
	if beaver_phase == BeaverVisualPhase.Done:
		if native_state == 0:
			beaver_phase = BeaverVisualPhase.Hidden
		return

	# Once contact begins, follow the rendered log end even while the native
	# descriptor remains visible. Native motion and render smoothing can otherwise
	# carry the held head through the wood before the dive finishes.
	if beaver_phase in [BeaverVisualPhase.Grab, BeaverVisualPhase.Look, BeaverVisualPhase.Bite, BeaverVisualPhase.Sink]:
		var front_pixels: float = 16.0 * BeaverScale * ModelFootprints.BeaverFrontTiles
		var prior_nose_x: float = beaver_last_x + float(beaver_heading) * front_pixels
		var tracked_end: Variant = nearest_log_end(beaver_last_row, beaver_heading, prior_nose_x)
		if tracked_end != null:
			beaver_last_x = float(tracked_end) - float(beaver_heading) * front_pixels

	var delta: float = 0.0 if paused else presentation_delta
	if beaver_phase == BeaverVisualPhase.Grab:
		beaver_phase_seconds += delta
		if beaver_phase_seconds >= BeaverGrabSeconds:
			beaver_phase = BeaverVisualPhase.Bite if beaver_player_hit else BeaverVisualPhase.Look
			beaver_phase_seconds = 0.0
	elif beaver_phase == BeaverVisualPhase.Look:
		beaver_phase_seconds += delta
		if beaver_phase_seconds >= BeaverLookSeconds:
			beaver_phase = BeaverVisualPhase.Sink
			beaver_phase_seconds = 0.0
	elif beaver_phase == BeaverVisualPhase.Bite:
		beaver_phase_seconds += delta
		if beaver_phase_seconds >= BeaverBiteSeconds:
			beaver_phase = BeaverVisualPhase.Sink
			beaver_phase_seconds = 0.0
	elif beaver_phase == BeaverVisualPhase.Sink:
		beaver_phase_seconds += delta
		if beaver_phase_seconds >= BeaverSinkSeconds + BeaverUnderHoldSeconds:
			beaver_phase = BeaverVisualPhase.Done
			return

	var sink_amount: float = clampf(beaver_phase_seconds / BeaverSinkSeconds, 0.0, 1.0) if beaver_phase == BeaverVisualPhase.Sink else 0.0
	var a = actor("hazard32856", "otter")
	a.set_active(true)
	a.set_clipped(true)
	a.root.position = pos3(beaver_last_x, beaver_last_row,
		BoardVisuals.beaver_height(BoardVisuals.WaterSurfaceHeight, BeaverScale, sink_amount))
	a.root.rotation = Vector3(0, float(beaver_heading) * PI / 2.0, 0)
	a.root.scale = Vector3.ONE * BeaverScale
	var grab_end: float = BeaverAttackSeconds * 0.55
	match beaver_phase:
		BeaverVisualPhase.Approach:
			a.play("Move")
		BeaverVisualPhase.Grab:
			a.pose("Attack", grab_end * clampf(beaver_phase_seconds / BeaverGrabSeconds, 0.0, 1.0))
		BeaverVisualPhase.Bite:
			a.pose("Attack", grab_end + (BeaverAttackSeconds - grab_end) * clampf(beaver_phase_seconds / BeaverBiteSeconds, 0.0, 1.0))
		BeaverVisualPhase.Look, BeaverVisualPhase.Sink:
			a.pose("Attack", BeaverAttackSeconds if beaver_player_hit else grab_end)

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
			panel.mouse_filter = Control.MOUSE_FILTER_IGNORE
			var st = style_box(Color(0.06, 0.10, 0.20, 0.94), 12, 2)
			st.border_color = Color("ffce57")
			st.content_margin_top = ModalVerticalPadding
			st.content_margin_bottom = ModalVerticalPadding
			panel.add_theme_stylebox_override("panel", st)
			var content := VBoxContainer.new()
			content.alignment = BoxContainer.ALIGNMENT_CENTER
			content.add_theme_constant_override("separation", 4)
			panel.add_child(content)
			var heading = make_text("TIME BONUS", 20, Cream)
			heading.add_theme_font_override("font", display_font)
			content.add_child(heading)
			content.add_child(make_text("+%d" % amount, 29, Color("7beaff")))
			panel.size = panel.get_combined_minimum_size()
			view = panel
			screen = viewport_size * 0.5
			if follow_camera:
				screen.y = hud_header.get_global_rect().end.y + 16.0 + panel.size.y * 0.5
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
			popup.view.size = popup.view.get_combined_minimum_size()
		var origin: Vector2 = popup.anchor_uv * viewport_size
		popup.view.position = origin - popup.view.size * 0.5 + (Vector2.ZERO if time_bonus else Vector2(0, -age * 36.0))
		var opacity: float = minf(1.0, age / 0.08) if time_bonus else 1.0
		opacity *= clampf((lifetime - age) / (0.22 if time_bonus else 0.35), 0.0, 1.0)
		popup.view.modulate = Color(1, 1, 1, opacity)

	update_message_layout()

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
		follow_camera = (review == "perspective-follow" or review == "ortho-follow" or review == "bonus-follow" or review == "level-transition-follow")
	var args = OS.get_cmdline_user_args()
	review_close = args.has("--review-close")
	top_down_camera = args.has("--top-down-shot")
	review_lady_close = args.has("--review-lady-close")
	review_turtle_close = args.has("--review-turtle-close")
	review_gator_close = args.has("--review-gator-close")
	review_snake_close = args.has("--review-snake-close")
	review_beaver_close = args.has("--review-beaver-close")

func review_frames_list() -> Array:
	match review:
		"footer-empty": return [1, 8]
		"level-start": return [1, 50, 100, 130, 160, 190]
		"level-transition", "level-transition-follow": return [1, 16, 40, 100, 300, 340, 360, 380]
		"two-player": return [1, 40, 100, 160, 220]
		"carry", "carry-left": return [1, 5, 9, 13, 25]
		"lady-move": return [1, 8, 17, 25, 30, 40, 45]
		"lady-hidden": return [1, 8]
		"lady-hidden-pickup": return [1, 8, 9, 12]
		"lady-goal-overwrite": return [1, 8]
		"lady-rom-clear": return [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
		"snake", "snake-left": return [1, 8]
		"snake-bank-turn": return [1, 24, 32, 40, 48, 64, 96, 120]
		"beaver", "beaver-empty": return [1, 8, 24, 40, 55, 65]
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
		"river-wrap-croc", "river-wrap-log": return [1, 3, 19, 35, 73]
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
		if review == "footer-empty":
			simulation.poke(0x83e5, 0)
		if review == "level-start":
			start_game(1)
		if review == "two-player":
			start_game(2)
			for frame in range(4000):
				simulation.step()
				if simulation.peek(0x83fd) == 2 and simulation.peek(0x8109) > 0:
					break
			observe_frame()
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
		if review in ["final-home", "level-transition", "level-transition-follow"]:
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
	if review in ["river-wrap-croc", "river-wrap-log"]:
		var tip: int = (253 + review_frame) & 255
		var outgoing: bool = review == "river-wrap-croc"
		simulation.poke(0x83b7, 2)
		simulation.poke(0x8100, 1)
		simulation.poke(0x8101, tip)
		simulation.poke(0x8150, int(outgoing if review_frame < 3 else not outgoing))
	if review in ["river-gator", "river-back", "river-snout"]:
		var x: int = 120
		simulation.poke(0x83b7, 2)
		simulation.poke(0x8100, 1)
		simulation.poke(0x8101, x + 12 + 30)
		simulation.poke(0x8150, 1)
		if review_frame == 1 and review != "river-gator":
			set_review_frog(110 if review == "river-back" else 148, 48)
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
				var diver: bool = index == BoardVisuals.at(probe, 0x8275 if lane == 1 else 0x827e) - 1
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
		simulation.poke(0x8049, 1)
		simulation.poke(0x804b, 96)
	if review == "snake-bank-turn":
		var direction: int = 1 if review_frame < 32 else -1
		var x: int = 110 + review_frame if review_frame < 32 else 174 - review_frame
		simulation.poke(0x8048, x)
		simulation.poke(0x8049, 0x2c | (0x80 if direction > 0 else 0))
		simulation.poke(0x804b, 126)
		simulation.poke(0x804c, (x - direction * 15) & 255)
	if review in ["beaver", "beaver-empty"]:
		var log_center: float = 120.0
		var best_distance: float = 999.0
		var log_count: int = simulation.peek(0x8112)
		for index in range(mini(8, log_count)):
			var raw_center: float = float(simulation.peek(0x8113 + index) - 12) - float(LaneWidths[2]) / 2.0
			for wrap in [-1, 0, 1]:
				var candidate: float = raw_center + float(wrap) * 256.0
				var right_end: float = candidate + (float(LaneWidths[2]) - 3.0) * 0.5
				var distance: float = absf(right_end - 175.0)
				if right_end >= 48.0 and right_end <= 220.0 and distance < best_distance:
					best_distance = distance
					log_center = candidate
		var log_end: float = log_center + (float(LaneWidths[2]) - 3.0) * 0.5
		# Face left toward the log's right end. The ROM's lethal/nose probe is
		# then slot X-4, leaving the torso in open water while only the muzzle
		# and forepaws meet the end grain.
		var approach_gap: float = float(maxi(1, 8 - review_frame))
		if review == "beaver-empty" and review_frame >= 8:
			for address in range(0x8058, 0x805c):
				simulation.poke(address, 0)
			simulation.poke(0x8486, 0)
		else:
			var nose_x: float = log_end + (0.0 if review_frame >= 8 else approach_gap)
			simulation.poke(0x8058, int(roundf(nose_x + 4.0)))
			simulation.poke(0x8059, 0x81)
			simulation.poke(0x805b, 80)
			simulation.poke(0x8486, 2 if review_frame >= 8 else 1)
			if review == "beaver" and review_frame >= 8:
				set_review_frog(int(roundf(log_end)), 80)
		presentation_delta = FRAME_SECONDS
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

	var player = actor("player", active_frog_model())
	var free_lady: ModelActor = actors.get("lady")
	var snake: ModelActor = actors.get("hazard32840")
	var home_gator: ModelActor = actors.get("homegator2")
	var river_gator: ModelActor = actors.get("lane0.0.0.0.%s" % str(true))
	var rider: ModelActor = actors.get("passenger")

	var rider_visible: bool = (rider != null and rider.root.is_visible_in_tree())
	if review == "river-back" and BoardVisuals.at(state, 0x8004) == 0 and not frog_visual.dying:
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
	for lane in batched_lanes.values():
		visible_logs += lane.visible_instances()

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
		"snakeTailX": BoardVisuals.at(state, 0x804c),
		"snakeDisplayX": snake.root.position.x * 16.0 + 120.0 if snake != null else 0.0,
		"snakeHeadDisplayX": snake.root.to_global(Vector3(0.0, 0.0, SnakeVisual.HeadZ)).x * 16.0 + 120.0 if snake != null else 0.0,
		"snakeLogTips": [BoardVisuals.at(state, 0x8113), BoardVisuals.at(state, 0x8114), BoardVisuals.at(state, 0x8115)],
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
		if review == "snake-motion" and snake != null and snake_visuals[0x8048].heading != (1 if (BoardVisuals.at(state, 0x8049) & 0x80) != 0 else -1):
			push_error("Snake failed to turn toward its own movement relative to the log")
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
	if not review_close and not review_lady_close and not review_turtle_close and not review_gator_close and not review_snake_close and not review_beaver_close:
		return
	var player = actor("player", active_frog_model())
	var target: Vector3
	if review_gator_close:
		target = pos3(134.0, 48.0, 0.0) if review in ["river-gator", "river-back", "river-snout"] else Vector3(0, 0.30, -6.3)
	elif review_snake_close and actors.has("hazard32840"):
		target = actors["hazard32840"].root.position + Vector3.UP * 0.1
	elif review_beaver_close and actors.has("hazard32856"):
		target = actors["hazard32856"].root.position
		target.y = 0.30
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
	update_camera_clip_planes()
