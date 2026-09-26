extends SceneTree

var failures: Array[String] = []

func check(condition: bool, message: String) -> void:
	if not condition:
		failures.append(message)
		push_error(message)

func _initialize() -> void:
	call_deferred("run")

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
	game.update_bonuses(0.0)
	check(is_equal_approx(time_bonus.position.y, game.hud_header.get_global_rect().end.y + 16.0), "Time Bonus sits below the HUD with padding")
	check(is_equal_approx(time_bonus.position.x + time_bonus.size.x * 0.5, game.get_viewport().get_visible_rect().size.x * 0.5), "Time Bonus stays horizontally centered")
	game.follow_camera = false
	game.update_bonuses(0.0)
	check(is_equal_approx(time_bonus.position.y + time_bonus.size.y * 0.5, game.get_viewport().get_visible_rect().size.y * 2.0 / 3.0), "Non-follow Time Bonus is centered at two-thirds screen height")
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
	game.free()
	await process_frame
	print("PRESENTATION TEST: %s" % ("PASS" if failures.is_empty() else str(failures)))
	quit(0 if failures.is_empty() else 1)
