extends SceneTree

var failures: Array[String] = []

class SafeAreaGame extends FroggerGame:
	var test_insets := Vector4.ZERO
	func get_ui_safe_rect(view_size: Vector2) -> Rect2:
		return Rect2(Vector2(test_insets.x, test_insets.y), view_size - Vector2(test_insets.x + test_insets.z, test_insets.y + test_insets.w))

func check(condition: bool, message: String) -> void:
	if not condition:
		failures.append(message)
		push_error(message)

func _initialize() -> void:
	run.call_deferred()

func measure(game: FroggerGame, window_size: Vector2i, density: float) -> Dictionary:
	root.size = window_size
	# Exercise the actual Window stretch transform, not an unscaled SubViewport.
	for i in range(3):
		await process_frame
	game.layout_ui.call()
	game.update_hud()
	await process_frame
	var ui_scale: float = root.get_final_transform().get_scale().y / density
	var result := {
		"gear": game.gameplay_options_button.size.y * ui_scale,
		"score": game.score_label.get_theme_font_size("font_size") * ui_scale,
		"lives": game.lives_label.get_theme_font_size("font_size") * ui_scale,
		"message": game.message_label.get_theme_font_size("font_size") * ui_scale,
	}
	var view: Vector2 = root.get_visible_rect().size
	var header := game.hud_header.get_global_rect()
	var footer := game.hud_footer.get_global_rect()
	check(Rect2(Vector2.ZERO, view).grow(1).encloses(header), "Header fits %s" % window_size)
	check(Rect2(Vector2.ZERO, view).grow(1).encloses(footer), "Footer fits %s" % window_size)
	if window_size.x > window_size.y:
		check(header.position.is_equal_approx(Vector2.ZERO) and is_equal_approx(header.size.y, view.y), "Former header occupies the left edge")
		check(is_equal_approx(footer.end.x, view.x) and is_zero_approx(footer.position.y) and is_equal_approx(footer.size.y, view.y), "Former footer occupies the right edge")
		check(game.high_label.global_position.y < game.score_label.global_position.y, "Portrait ordering puts high score above player score on the left")
		check(game.timer_bar.global_position.y < game.gameplay_options_button.global_position.y, "Portrait ordering puts the timer above the gear on the right")
		check(game.timer_bar.size.y > game.timer_bar.size.x and game.timer_bar.fill_mode == ProgressBar.FILL_BOTTOM_TO_TOP, "Time meter is vertical and drains toward the bottom")
		check(is_zero_approx(game.score_label.get_global_transform().get_rotation()) and is_zero_approx(game.gameplay_options_button.get_global_transform().get_rotation()), "Sidebar text and controls stay upright")
	else:
		check(is_equal_approx(footer.end.y, view.y) and is_zero_approx(header.position.y), "Portrait keeps top and bottom HUD bars")
		check(absf(header.size.y * ui_scale - 94.0 * 430.0 / 480.0) < 1.0, "Existing portrait HUD scale is preserved")
		check(game.timer_bar.size.x > game.timer_bar.size.y and game.timer_bar.fill_mode == ProgressBar.FILL_BEGIN_TO_END, "Portrait restores the horizontal timer")
	print("MOBILE UI %s @%s: %s" % [window_size, density, result])
	return result

func run() -> void:
	root.mode = Window.MODE_WINDOWED
	var game := SafeAreaGame.new()
	game.screenshot = "test-no-save"
	root.add_child(game)
	game.set_process(false)
	game.start_game(1)
	check(game.touch_device, "Run this test with -- --mobile-ui")
	# iPhone 14 Pro Max CSS dimensions and 3x framebuffer dimensions. Rotate
	# both ways so a stale layout or a density-dependent scale cannot pass.
	for density in [1.0, 3.0]:
		var portrait := await measure(game, Vector2i(Vector2(430, 932) * density), density)
		var landscape := await measure(game, Vector2i(Vector2(932, 430) * density), density)
		var returned := await measure(game, Vector2i(Vector2(430, 932) * density), density)
		for element in portrait:
			check(absf(landscape[element] - portrait[element]) < 1.0,
				"%s keeps portrait size in landscape at density %s" % [element, density])
			check(absf(returned[element] - portrait[element]) < 1.0,
				"%s restores portrait size at density %s" % [element, density])
	await measure(game, Vector2i(932, 430), 1.0)
	for reserve_lives in [0, 2, 3]:
		game.simulation.poke(0x83e5, reserve_lives)
		game.observe_frame()
		game.update_hud()
		for i in range(3):
			await process_frame
		var footer := game.hud_footer.get_global_rect()
		for control in [game.gameplay_options_button, game.lives_icons, game.timer_bar]:
			check(footer.encloses(control.get_global_rect()), "Right sidebar contains its controls with %d lives" % reserve_lives)
	game.follow_camera = false
	for perspective in [false, true]:
		game.perspective_view = perspective
		for i in range(120):
			game.update_camera(1.0 / 60.0)
		for x in [-7.4, 7.4]:
			for edge in [Vector2(0.1, 7.8), Vector2(1.2, -7.0)]:
				var projected: Vector2 = game.camera.unproject_position(Vector3(x, edge.x, edge.y))
				check(projected.x > game.hud_header.get_global_rect().end.x and projected.x < game.hud_footer.position.x, "Overview clears both sidebars")
				check(projected.y > 0 and projected.y < root.get_visible_rect().size.y, "Overview uses available vertical space")
	var gear_position := game.gameplay_options_button.global_position
	game.gameplay_options_button.pressed.emit()
	await process_frame
	check(game.paused and game.menu.visible, "Sidebar gear still opens pause")
	check(game.gameplay_options_button.global_position.is_equal_approx(gear_position), "Pause retains gear position")
	check(is_zero_approx(game.menu.rotation) and game.menu.get_global_rect().get_center().distance_to(root.get_visible_rect().size * 0.5) < 1.0, "Menu remains upright and centered")
	game.gameplay_options_button.pressed.emit()
	check(not game.paused, "Sidebar gear still resumes play")
	# Both notch orientations and an asymmetric host/PWA inset. Values here are
	# logical UI units; the JS bridge separately tests CSS-pixel conversion.
	for insets in [Vector4(66, 0, 66, 24), Vector4(66, 0, 0, 24), Vector4(0, 0, 66, 24)]:
		game.test_insets = insets
		game.layout_ui.call()
		for i in range(3):
			await process_frame
		var safe := game.ui_safe_rect
		check(safe.grow(0.1).encloses(game.hud_header.get_global_rect()) and safe.grow(0.1).encloses(game.hud_footer.get_global_rect()), "Both sidebars respect device safe areas")
		check(game.hud_header.position.x >= insets.x and game.hud_footer.get_global_rect().end.x <= root.get_visible_rect().size.x - insets.z + 0.1, "Notch avoidance follows left and right inset changes")
		check(safe.encloses(game.gameplay_options_button.get_global_rect()), "Pause button clears the notch and home indicator")
		game.show_menu(true)
		await process_frame
		check(safe.grow(0.1).encloses(game.menu.get_global_rect()) and is_zero_approx(game.menu.rotation), "Unrotated menu stays within the safe area")
	game.queue_free()
	await process_frame
	print("MOBILE UI TEST: %s" % ("PASS" if failures.is_empty() else str(failures)))
	quit(0 if failures.is_empty() else 1)
