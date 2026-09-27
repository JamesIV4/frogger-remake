extends SceneTree

var failures: Array[String] = []

func check(condition: bool, message: String) -> void:
	if not condition:
		failures.append(message)
		push_error(message)

func _initialize() -> void:
	call_deferred("run")

func posed_model_top(actor: ModelActor) -> float:
	var top: float = -INF
	for mesh in actor.mesh_instances:
		var baked: ArrayMesh = mesh.bake_mesh_from_current_skeleton_pose()
		for surface in range(baked.get_surface_count()):
			for vertex in baked.surface_get_arrays(surface)[Mesh.ARRAY_VERTEX]:
				top = maxf(top, (mesh.global_transform * vertex).y)
	return top

func run() -> void:
	var game := FroggerGame.new()
	# Exercise the live scene without changing the user's saved preferences.
	game.screenshot = "test-no-save"
	root.add_child(game)
	game.screenshot = "test-no-save"
	game.set_process(false)
	await process_frame
	check(not game.show_fps, "FPS counter defaults off")
	game.return_to_main_menu()
	await process_frame
	check(not game.started and game.menu.visible, "Main menu stops the previous game")
	check(game.menu_items.find_children("*", "CheckButton", true, false).is_empty(), "Main menu keeps settings behind Options")
	game.show_options(false)
	await process_frame
	check(game.options_open and not game.started, "Main menu opens Options without starting")
	check(game.menu_scroll.scroll_deadzone > 0 and game.menu_items.mouse_filter == Control.MOUSE_FILTER_PASS, "Menu scroll accepts touch drags without accidental taps")
	for option_control in game.menu_items.find_children("*", "Control", true, false):
		if option_control is BaseButton or option_control is Slider:
			check(option_control.mouse_filter == Control.MOUSE_FILTER_PASS, "Options controls pass swipe gestures to the scroll container")
	game.navigate_back()
	await process_frame
	check(not game.options_open and not game.started and game.menu.visible, "Options Back returns to main menu")
	game.start_game(1)
	game.update_hud()
	await process_frame
	await process_frame
	var lives_position: Vector2 = game.lives_icons.get_parent().global_position
	var timer_position: Vector2 = game.timer_bar.global_position
	check(game.gameplay_options_button.visible, "Gameplay exposes pause gear")
	check(game.gameplay_options_button.icon != null and game.gameplay_options_button.text.is_empty(), "Gameplay pause uses a gear icon")
	game.gameplay_options_button.pressed.emit()
	await process_frame
	game.update_hud()
	await process_frame
	check(game.gameplay_options_button.visible and not game.gameplay_options_button.disabled, "Pause keeps the gear available to resume")
	check(game.lives_icons.get_parent().global_position.is_equal_approx(lives_position), "Lives stay in place while paused")
	check(game.timer_bar.global_position.is_equal_approx(timer_position), "Timer stays in place while paused")
	check(game.paused and not game.options_open and game.menu.visible, "Gear opens Pause screen")
	game.gameplay_options_button.pressed.emit()
	await process_frame
	check(not game.paused and not game.menu.visible, "Pressing gear again resumes gameplay")
	game.gameplay_options_button.pressed.emit()
	await process_frame
	var pause_buttons: Array[String] = []
	for child in game.menu_items.get_children():
		if child is Button:
			pause_buttons.append(child.text)
	check(pause_buttons == ["RESUME", "OPTIONS", "RETURN TO MAIN MENU"], "Pause menu action order")
	for child in game.menu_items.get_children():
		if child is Button and child.text == "OPTIONS":
			child.pressed.emit()
			break
	await process_frame
	check(game.options_open and game.paused, "Pause Options button opens settings")
	game.navigate_back()
	await process_frame
	check(not game.options_open and game.paused, "Options Back returns to pause menu")
	game.resume_game()
	game.touch_device = false
	game.update_mouse_visibility(1.99)
	check(Input.mouse_mode == Input.MOUSE_MODE_VISIBLE, "Desktop cursor stays visible before 2 seconds")
	game.update_mouse_visibility(0.02)
	check(game.mouse_idle_seconds >= 2.0, "Desktop idle timer reaches 2 seconds")
	if DisplayServer.get_name() != "headless":
		check(Input.mouse_mode == Input.MOUSE_MODE_HIDDEN, "Desktop cursor hides after 2 seconds")
	var motion := InputEventMouseMotion.new()
	motion.relative = Vector2.ONE
	game._input(motion)
	check(Input.mouse_mode == Input.MOUSE_MODE_VISIBLE, "Mouse movement restores cursor")
	game.touch_device = true
	game.update_mouse_visibility(3.0)
	check(Input.mouse_mode == Input.MOUSE_MODE_VISIBLE, "Touch devices never hide cursor")
	game.simulation.poke(0x83fe, 0)
	game.observe_frame()
	var key := InputEventKey.new()
	key.pressed = true
	game._input(key)
	game.update_hud()
	check(game.message_label.text == "GAME OVER" and game.message_hint.text == "Press Enter to restart", "Keyboard game-over prompt has a separate title")
	var touch := InputEventScreenTouch.new()
	touch.pressed = true
	game._input(touch)
	var emulated_mouse := InputEventMouseButton.new()
	emulated_mouse.pressed = true
	emulated_mouse.device = InputEvent.DEVICE_ID_EMULATION
	game._input(emulated_mouse)
	game.update_hud()
	check(game.message_hint.text == "Tap to restart", "Touch prompt survives emulated mouse events")
	var stick := InputEventJoypadMotion.new()
	stick.axis_value = 0.1
	game._input(stick)
	check(game.last_input_method == "touch", "Controller drift does not replace the last-used input")
	stick.axis_value = 0.8
	game._input(stick)
	game.update_hud()
	check(game.message_hint.text == "Press controller A to restart", "Controller movement updates the restart prompt")
	game._input(key)
	touch.device = InputEvent.DEVICE_ID_EMULATION
	game._input(touch)
	game.update_hud()
	check(game.message_hint.text == "Press Enter to restart", "Emulated touch does not override keyboard input")
	await process_frame
	await process_frame
	check(game.message_hint.global_position.y >= game.message_label.get_global_rect().end.y + 18.0, "Restart hint has padding below GAME OVER")
	var time_bonus := PanelContainer.new()
	game.bonus_overlay.add_child(time_bonus)
	game.popups.append(FroggerGame.BonusPopup.new({"kind": 2, "frame": game.state["frame"]}, time_bonus, Vector2(0.5, 0.33), "TIME BONUS +100"))
	game.touch_device = false
	game.update_bonuses(0.0)
	check(is_equal_approx(time_bonus.position.y, game.hud_header.get_global_rect().end.y + 16.0), "Desktop follow Time Bonus sits below the HUD with padding")
	check(is_equal_approx(time_bonus.position.x + time_bonus.size.x * 0.5, game.get_viewport().get_visible_rect().size.x * 0.5), "Time Bonus stays horizontally centered")
	game.follow_camera = false
	game.touch_device = false
	game.top_down_camera = false
	game.update_bonuses(0.0)
	check(is_equal_approx(time_bonus.position.y + time_bonus.size.y * 0.5, game.get_viewport().get_visible_rect().size.y * 2.0 / 3.0), "Non-follow Time Bonus is centered at two-thirds screen height")
	game.touch_device = true
	for perspective_mode in [false, true]:
		game.perspective_view = perspective_mode
		for top_down_mode in [false, true]:
			game.top_down_camera = top_down_mode
			for follow_mode in [false, true]:
				game.follow_camera = follow_mode
				game.update_bonuses(0.0)
				check((time_bonus.position + time_bonus.size * 0.5).is_equal_approx(game.get_viewport().get_visible_rect().size * 0.5), "Mobile Time Bonus is centered in every camera mode")
	game.touch_device = false
	game.top_down_camera = false
	game.perspective_view = true
	game.follow_camera = true
	for model in ["lady_frog", "frog"]:
		game.select_player_frog(model)
		await process_frame
		game.start_game(1)
		game.update_actors()
		var player: ModelActor = game.actors["player"]
		check(player.root.scene_file_path.ends_with("/%s.glb" % model), "Selected frog must be the player")
		check(player.root.scale.is_equal_approx(Vector3.ONE), "Either player uses full green-player scale")
		check(player.passenger_socket != null, "Either player must carry a rescue frog")
		for clip in ["Idle", "Hop", "Squash", "Drown", "Celebrate"]:
			player.pose(clip, 0.05)
			check(player.current_anim.to_lower().ends_with(clip.to_lower()), "Both players support %s" % clip)
		game.frog_visual.carrying = true
		game.update_player(0.0)
		var passenger: ModelActor = game.actors["passenger"]
		check(passenger.root.scene_file_path.ends_with("/%s.glb" % game.rescue_frog_model()), "Rescue frog must be the other color")
		check(passenger.root.scale.is_equal_approx(Vector3.ONE * game.PassengerScale), "Rescue frog retains smaller passenger scale")
		check(passenger.root.get_parent() == player.passenger_socket, "Passenger must use selected player's socket")
		for clip in ["Idle", "Hop", "Celebrate"]:
			passenger.pose(clip, 0.05)
			check(passenger.current_anim.to_lower().ends_with(clip.to_lower()), "Either rescue frog supports %s" % clip)
		game.simulation.poke(0x825e, 1)
		game.observe_frame()
		game.lady_visual.active = true
		game.lady_visual.x = 120.0
		game.update_homes_and_hazards()
		check(game.actors["home0"].root.scene_file_path.ends_with("/%s.glb" % model), "Home frog must match chosen player")
		var rescue: ModelActor = game.actors["lady"]
		check(rescue.root.scene_file_path.ends_with("/%s.glb" % game.rescue_frog_model()), "Log rescue frog must use the other color")
		check(rescue.root.scale.is_equal_approx(Vector3.ONE * game.LadyInRiverScale), "Log rescue frog retains smaller scale")
	# Log drift can carry a left-crawling snake right in world space. Its
	# heading must follow the ROM direction, including an immediate turn.
	game.clear_presentation()
	for address in [0x8048, 0x8050]:
		game.simulation.poke(address + 1, 1)
		game.simulation.poke(address + 3, 78)
		for frame in range(1, 41):
			game.simulation.poke(address, 100 + frame)
			game.observe_frame()
			game.state["frame"] = frame
			game.presentation_delta = game.FRAME_SECONDS
			game.update_homes_and_hazards()
		var snake: ModelActor = game.actors["hazard%d" % address]
		check(game.moving_visuals.velocity_x(1000 + address) > 0.0, "Snake regression exercises rightward world drift")
		check((snake.root.basis * Vector3.BACK).x < -0.7, "Snake crawling left on a right-drifting log faces left")
		game.simulation.poke(address + 1, 0x81)
		game.observe_frame()
		game.state["frame"] = 40
		game.update_homes_and_hazards()
		check((snake.root.basis * Vector3.BACK).x > 0.7, "Snake turns immediately with its own direction")
		game.simulation.poke(address + 1, 1)
		game.simulation.poke(address + 3, 126)
		game.observe_frame()
		game.state["frame"] = 40
		game.update_homes_and_hazards()
		check((snake.root.basis * Vector3.BACK).x < -0.7, "Bank snake also follows its own direction")
	# Dispatcher B's state 1 is the beaver approaching the log. It stays
	# half-submerged and cannot attack. The lethal state-2 overlap gets a bite;
	# an empty-end descriptor clear gets only grab/look. Both then sink fully.
	game.clear_presentation()
	game.simulation.poke(0x83fd, 1)
	game.simulation.poke(0x8058, 120)
	game.simulation.poke(0x8059, 1)
	game.simulation.poke(0x805b, 80)
	game.simulation.poke(0x8486, 1)
	game.observe_frame()
	game.presentation_delta = game.FRAME_SECONDS
	game.update_homes_and_hazards()
	var beaver: ModelActor = game.actors["hazard32856"]
	var water_surface: float = BoardVisuals.WaterSurfaceHeight
	var swimming_height: float = BoardVisuals.beaver_height(water_surface, game.BeaverScale, 0.0)
	check(is_equal_approx(beaver.root.position.y, swimming_height), "Approaching beaver stays half-submerged until log contact")
	check(beaver.root.position.y < water_surface and beaver.root.position.y + game.BeaverScale * ModelFootprints.BeaverTopTiles > water_surface,
		"Approaching beaver visibly crosses the waterline")
	check(beaver.current_anim.to_lower().ends_with("move"), "Approaching beaver does not bite before lethal range")
	var rendered_nose_x: float = 120.0 + 16.0 * (beaver.root.position.x + game.BeaverScale * ModelFootprints.BeaverFrontTiles)
	check(absf(rendered_nose_x - 140.0) < 0.01, "Beaver nose aligns with the ROM's right-facing lethal probe")
	game.simulation.poke(0x8059, 0x81)
	game.observe_frame()
	game.update_homes_and_hazards()
	rendered_nose_x = 120.0 + 16.0 * (beaver.root.position.x - game.BeaverScale * ModelFootprints.BeaverFrontTiles)
	check(absf(rendered_nose_x - 116.0) < 0.01, "Beaver nose aligns with the ROM's left-facing lethal probe")
	game.simulation.poke(0x8486, 2)
	game.observe_frame()
	game.update_homes_and_hazards()
	check(beaver.current_anim.to_lower().ends_with("attack"), "ROM state 2 selects the beaver log-grab attack")
	check(is_equal_approx(beaver.root.position.y, swimming_height), "Beaver grabs before beginning its under-log transition")
	for frame in range(80):
		if game.beaver_phase == game.BeaverVisualPhase.Bite:
			break
		game.update_homes_and_hazards()
	check(game.beaver_phase == game.BeaverVisualPhase.Bite, "Only the lethal player-overlap branch reaches the bite")
	for frame in range(80):
		if game.beaver_phase == game.BeaverVisualPhase.Sink:
			break
		game.update_homes_and_hazards()
	for frame in range(18):
		game.update_homes_and_hazards()
	var log_top: float = -0.18 + ModelFootprints.LogTopTiles
	check(beaver.root.position.y + game.BeaverScale * ModelFootprints.BeaverTopTiles < log_top,
		"Reached beaver finishes below the log instead of climbing on top")
	check(beaver.root.position.y + game.BeaverScale * ModelFootprints.BeaverTopTiles <= water_surface - 0.049,
		"Reached beaver sinks wholly beneath the water instead of phasing through the log")
	if DisplayServer.get_name() != "headless":
		await process_frame
		var bite_top: float = posed_model_top(beaver)
		check(is_finite(bite_top) and bite_top <= water_surface - 0.049,
			"Animated bite head stays beneath water troughs at full submersion")

	# No-player arrival: the ROM clears the descriptor at the target. Preserve a
	# short presentation-only grab/look/sink, but never sample the bite section.
	game.clear_presentation()
	game.simulation.poke(0x8058, 120)
	game.simulation.poke(0x8059, 1)
	game.simulation.poke(0x805b, 80)
	game.simulation.poke(0x8486, 1)
	game.observe_frame()
	game.presentation_delta = game.FRAME_SECONDS
	game.update_homes_and_hazards()
	for address in range(0x8058, 0x805c):
		game.simulation.poke(address, 0)
	game.simulation.poke(0x8486, 0)
	game.observe_frame()
	game.update_homes_and_hazards()
	var empty_end_bit: bool = false
	for frame in range(80):
		empty_end_bit = empty_end_bit or game.beaver_phase == game.BeaverVisualPhase.Bite
		if game.beaver_phase == game.BeaverVisualPhase.Sink:
			break
		game.update_homes_and_hazards()
	check(not empty_end_bit and not game.beaver_player_hit, "Empty log end grabs and looks without biting")
	for frame in range(18):
		game.update_homes_and_hazards()
	check(beaver.root.position.y + game.BeaverScale * ModelFootprints.BeaverTopTiles <= water_surface - 0.049,
		"Empty-end beaver also finishes fully underwater")
	if DisplayServer.get_name() != "headless":
		await process_frame
		var grab_top: float = posed_model_top(beaver)
		check(is_finite(grab_top) and grab_top <= water_surface - 0.049,
			"Animated empty-end grab stays beneath water troughs at full submersion")
	# A live descriptor may drift into the log while its grab/bite pose is held.
	# Use the actual rendered end caps, including their overhang and smoothing.
	for heading in [-1, 1]:
		for hit in [false, true]:
			game.clear_presentation()
			game.simulation.poke(0x8112, 1)
			game.simulation.poke(0x8113, 166)
			game.simulation.poke(0x8058, 157 if heading < 0 else 44)
			game.simulation.poke(0x8059, 0x81 if heading < 0 else 1)
			game.simulation.poke(0x805b, 80)
			game.simulation.poke(0x8486, 1)
			game.observe_frame()
			game.update_actors()
			game.simulation.poke(0x8486, 2 if hit else 0)
			game.simulation.poke(0x8058, (137 if heading < 0 else 64) if hit else 0)
			for frame in range(65):
				game.simulation.poke(0x8113, 166 + frame / 4)
				game.observe_frame()
				game.state["frame"] += frame
				game.update_actors()
				if not beaver.is_active:
					continue
				var logs: MultiMesh = game.batched_lanes[2].body.multimesh
				var bounds: AABB = logs.get_instance_transform(0) * logs.mesh.get_aabb()
				var end_x: float = bounds.end.x if heading < 0 else bounds.position.x
				var nose_x: float = beaver.root.position.x + heading * game.BeaverScale * ModelFootprints.BeaverFrontTiles
				check(heading * (nose_x - end_x) <= 0.0001,
					"Grab/bite/dive stays outside the moving log end in either direction")
	# Render both incarnations of slot zero while they overlap opposite edges.
	for outgoing in [false, true]:
		game.clear_presentation()
		game.simulation.poke(0x83b7, 2)
		game.simulation.poke(0x8100, 1)
		game.simulation.poke(0x8150, 1 if outgoing else 0)
		game.simulation.poke(0x8101, 254)
		game.observe_frame()
		game.simulation.poke(0x8101, 0)
		game.simulation.poke(0x8150, 0 if outgoing else 1)
		game.observe_frame()
		for tip in [0, 1, 16, 40, 80]:
			game.simulation.poke(0x8101, tip)
			game.observe_frame()
			game.moving_visuals.reset()
			game.update_actors()
			var right_gators: int = 0
			var left_gators: int = 0
			for river_key in game.actors:
				var prop: ModelActor = game.actors[river_key]
				if river_key.begins_with("lane0.0.") and prop.root.visible:
					if prop.root.position.x > 0.0:
						right_gators += 1
					else:
						left_gators += 1
			check(right_gators == (1 if outgoing and tip <= 48 else 0), "Exiting croc/log retains its own identity through wrap at %d" % tip)
			if tip >= 40:
				check(left_gators == (0 if outgoing else 1), "Incoming croc/log has its own identity at %d" % tip)
	var river_fit: Dictionary = BoardVisuals.fit_river_gator(60)
	var tip_x: float = 160.0
	var visible_tip: float = tip_x - 42.0 + river_fit.center_offset_pixels + 16.0 * ModelFootprints.RiverGatorFrontTiles * river_fit.length_scale
	var head_length: float = 16.0 * ModelFootprints.RiverGatorSnoutTiles * river_fit.length_scale
	check(absf(visible_tip - (tip_x - 25.0)) < 0.01 and absf(47.0 - 16.0 * ModelFootprints.RiverGatorLengthTiles * river_fit.length_scale) < 0.01, "Crocodile length and leading edge match the ROM sprite")
	check(absf(visible_tip - head_length - (tip_x - 40.0)) < 0.01, "Rendered back ends at the actual ROM bite boundary")
	game.simulation.poke(0x8044, 110)
	game.simulation.poke(0x8047, 48)
	game.simulation.poke(0x8101, 160)
	game.simulation.poke(0x8150, 1)
	game.simulation.poke(0x8004, 1)
	game.simulation.poke(0x829c, 0)
	game.simulation.poke(0x83cd, 0)
	game.observe_frame()
	check(game.frog_visual.dying, "Death flag on a crocodile is never concealed as a ride")
	game.free()
	await process_frame
	print("PRESENTATION TEST: %s" % ("PASS" if failures.is_empty() else str(failures)))
	quit(0 if failures.is_empty() else 1)
