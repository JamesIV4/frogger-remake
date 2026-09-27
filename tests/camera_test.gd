extends SceneTree

var failures: Array[String] = []

func check(condition: bool, message: String) -> void:
	if not condition:
		failures.append(message)
		push_error(message)

func _initialize() -> void:
	run.call_deferred()

func settle(game: FroggerGame) -> void:
	for i in range(120):
		game.update_camera(1.0 / 60.0)

func place_frog(game: FroggerGame, x: float, row: float) -> void:
	game.simulation.poke(0x8044, int(x * 16.0 + 120.0))
	game.simulation.poke(0x8047, int(row * 16.0 + 128.0))
	game.observe_frame()
	game.displayed_frog_x = x * 16.0 + 120.0
	game.displayed_frog_row = row * 16.0 + 128.0

func run() -> void:
	var viewport := SubViewport.new()
	viewport.size = Vector2i(390, 844)
	root.add_child(viewport)
	var game := FroggerGame.new()
	game.screenshot = "test-no-save"
	viewport.add_child(game)
	game.set_process(false)
	game.start_game(1)
	game.perspective_view = true
	game.touch_device = true
	var camera: Camera3D = game.camera
	for size in [Vector2i(320, 932), Vector2i(390, 844), Vector2i(430, 932), Vector2i(768, 1024)]:
		viewport.size = size
		game.follow_camera = true
		for row in [-4.0, 0.0, 4.0]:
			place_frog(game, 0.0, row)
			settle(game)
			var center := camera.unproject_position(Vector3(0, 0, row))
			var next_tile := camera.unproject_position(Vector3(1, 0, row))
			var visible_columns: float = size.x / (next_tile.x - center.x)
			check(visible_columns >= 8.0 and visible_columns <= 12.0, "Portrait follow keeps nearby lanes readable while fitting vertically: %s, %s" % [size, visible_columns])
			check(absf(rad_to_deg(camera.position.direction_to(game.camera_look_target).angle_to(Vector3.DOWN)) - 22.0) < 0.01, "Portrait perspective follow uses the reduced board tilt")
			for side in [-1.0, 1.0]:
				place_frog(game, side * 6.5, row)
				settle(game)
				check(camera.position.x * side > 3.0, "Portrait camera follows toward either side within its screen margin")
				var board_edge := camera.unproject_position(Vector3(side * 7.4, 0.1, row))
				var gutter: float = board_edge.x if side < 0.0 else size.x - board_edge.x
				check(gutter <= size.x * 0.1601, "Perspective follow caps the empty side margin")
				for near_row in [row - 1.0, row, row + 1.0]:
					var edge := camera.unproject_position(Vector3(side * 6.5, 0.3, near_row))
					check(edge.x > size.x * 0.01 and edge.x < size.x * 0.99, "Approached board edge must be visible: %s, %s, %s" % [size, side, near_row])
		game.follow_camera = false
		settle(game)
		var overhead_tilt: float = rad_to_deg(camera.position.direction_to(game.camera_look_target).angle_to(Vector3.DOWN))
		check(absf(overhead_tilt - 22.0) < 0.01, "Portrait perspective reduces board tilt to 22 degrees from overhead")
		for side in [-1.0, 1.0]:
			var playable := camera.unproject_position(Vector3(side * 6.5, 0, 0))
			check(playable.x > 0 and playable.x < size.x, "Portrait overview fits playable width at board center")
			var near_bank := camera.unproject_position(Vector3(side * 7.0, 0.1, 7.0))
			check(near_bank.x > 0 and near_bank.x < size.x, "Portrait overview retains the near bank's playable edges")
			var bottom := camera.unproject_position(Vector3(side * 7.45, 0.3, 7.85))
			check(bottom.x < 0 or bottom.x > size.x, "Portrait perspective crops bottom corners")
		check(absf(camera.position.x) < 0.01, "Overview remains centered")
		game.perspective_view = false
		game.top_down_camera = true
		game.follow_camera = true
		for side in [-1.0, 0.0, 1.0]:
			place_frog(game, side * 6.5, 0)
			settle(game)
			check(camera.keep_aspect == Camera3D.KEEP_WIDTH and is_equal_approx(camera.size, 10.4), "Portrait orthographic follow keeps a readable horizontal scale")
			check((-camera.global_basis.z).dot(Vector3.DOWN) > 0.9999, "Portrait flat follow looks straight down")
			check(absf(camera.position.x) < 0.01 if side == 0.0 else camera.position.x * side > 3.5, "Portrait orthographic follows toward both sides within the margin")
			var player := camera.unproject_position(Vector3(side * 6.5, 0, 0))
			check(player.x > 0 and player.x < size.x, "Portrait orthographic keeps the player visible at either edge")
			if side != 0.0:
				var board_edge := camera.unproject_position(Vector3(side * 7.4, 0.1, 0))
				var gutter: float = board_edge.x if side < 0.0 else size.x - board_edge.x
				check(gutter <= size.x * 0.1601, "Portrait follow caps the empty side margin at the marked line")
		game.follow_camera = false
		settle(game)
		var ortho_wood := camera.unproject_position(Vector3(7.45, 0, 0))
		check(ortho_wood.x > size.x, "Portrait orthographic overview crops wooden trim")
		for side in [-1.0, 1.0]:
			var playable_edge := camera.unproject_position(Vector3(side * 7.0, 0, 0))
			check(playable_edge.x > 0 and playable_edge.x < size.x, "Flat overview retains both playable board edges")
		check((-camera.global_basis.z).dot(Vector3.DOWN) > 0.9999, "Portrait flat overview looks straight down")
		check(absf(camera.position.x) < 0.01, "Top-down overview stays horizontally centered")
		game.perspective_view = true
		game.top_down_camera = false
	game.touch_device = false
	game.follow_camera = true
	for size in [Vector2i(1100, 960), Vector2i(390, 844)]:
		viewport.size = size
		place_frog(game, 6.5, 0.0)
		settle(game)
		check(camera.keep_aspect == Camera3D.KEEP_HEIGHT and is_equal_approx(camera.fov, 54.0), "Desktop preserves original projection in either orientation")
		check(camera.position.distance_to(Vector3(2.5, 9.0, 5.8)) < 0.01, "Desktop preserves original follow position in either orientation")
		game.simulation.poke(0x8044, 0)
		game.observe_frame()
		camera.position = Vector3(1.5, 9.0, 7.8)
		game.camera_look_target = Vector3.ZERO
		game.update_camera(1.0 / 60.0)
		check(camera.position.distance_to(Vector3(1.5, 9.0, 7.8)) < 0.01, "Desktop retains original off-board camera-position fallback")
		game.perspective_view = false
		place_frog(game, 6.5, 0.0)
		settle(game)
		check(camera.keep_aspect == Camera3D.KEEP_HEIGHT and is_equal_approx(camera.size, 9.8), "Desktop orthographic keeps its original vertical scale")
		check(camera.position.distance_to(Vector3(2.5, 11.2, 6.5)) < 0.01, "Top-down off restores the original angled orthographic follow")
		game.perspective_view = true
	game.perspective_view = true
	game.follow_camera = false
	for size in [Vector2i(1100, 960), Vector2i(1920, 1080), Vector2i(2560, 1080), Vector2i(800, 960)]:
		viewport.size = size
		game.layout_ui.call()
		settle(game)
		for top_down in [false, true]:
			game.top_down_camera = top_down
			for perspective in [false, true]:
				game.perspective_view = perspective
				settle(game)
				check(camera.projection == (Camera3D.PROJECTION_PERSPECTIVE if perspective else Camera3D.PROJECTION_ORTHOGONAL), "Perspective independently selects projection")
				check(((-camera.global_basis.z).dot(Vector3.DOWN) > 0.9999) == top_down, "Top-down independently selects angle")
				for x in [-7.4, 7.4]:
					for edge in [Vector2(-0.94, 7.85), Vector2(0.15, 7.85), Vector2(0.15, -7.65), Vector2(1.2, -7.0)]:
						var point := camera.unproject_position(Vector3(x, edge.x, edge.y))
						check(point.x >= 5.9 and point.x <= size.x - 5.9, "Desktop overview retains the board sides")
						check(point.y >= game.hud_header.get_global_rect().end.y - 18.1, "Desktop overview limits its top HUD overlap")
						check(point.y <= game.hud_footer.global_position.y + 44.1, "Desktop overview limits its bottom HUD overlap")
				for world_point in [Vector3(-7.4, 0.1, 7.8), Vector3(7.4, 1.3, -7.25), Vector3(0, -3, 0)]:
					var depth: float = -camera.to_local(world_point).z
					check(depth > camera.near and depth < camera.far, "Shadow depth bounds retain the board and shader warmup")
				check(camera.far - camera.near <= 32.01, "Orthographic shadow depth stays limited to the scene")
	game.top_down_camera = false
	# A HUD change with no window resize must also adjust the overview.
	viewport.size = Vector2i(1100, 960)
	game.layout_ui.call()
	game.hud_header.size.y += 60.0
	game.hud_footer.position.y -= 50.0
	for projection in [false, true]:
		game.perspective_view = projection
		settle(game)
		for point in [Vector3(0, 1.2, -7.0), Vector3(0, -0.94, 7.85)]:
			var screen := camera.unproject_position(point)
			check(screen.y > game.hud_header.get_global_rect().end.y - 18.1 and screen.y < game.hud_footer.global_position.y + 44.1, "Overview responds to actual HUD bounds")
	game.layout_ui.call()
	viewport.size = Vector2i(390, 844)
	for mobile in [false, true]:
		game.touch_device = mobile
		game.follow_camera = true
		game.perspective_view = true
		place_frog(game, 0.0, 0.0)
		game.set_follow_zoom_percent(0.0)
		settle(game)
		var default_distance: float = camera.position.distance_to(game.camera_look_target)
		for zoom in [-50.0, 50.0, 100.0]:
			game.set_follow_zoom_percent(zoom)
			settle(game)
			var distance: float = camera.position.distance_to(game.camera_look_target)
			check(absf(distance - default_distance / (1.0 + zoom / 100.0)) < 0.01, "Perspective zoom offsets scale each device's original follow distance")
			var target: Vector3 = game.camera_look_target
			game.simulation.poke(0x8044, 0)
			game.observe_frame()
			settle(game)
			check(game.camera_look_target.distance_to(target) < 0.01, "Nonzero zoom must not move the followed row while frog is off board")
			place_frog(game, 0.0, 0.0)
		game.perspective_view = false
		game.set_follow_zoom_percent(0.0)
		settle(game)
		var ortho_position: Vector3 = camera.position
		game.set_follow_zoom_percent(-50.0)
		settle(game)
		check(is_equal_approx(camera.size, 20.8 if mobile else 19.6), "Negative orthographic zoom reveals more area")
		game.set_follow_zoom_percent(100.0)
		settle(game)
		check(is_equal_approx(camera.size, 5.2 if mobile else 4.9), "Positive orthographic zoom magnifies nearby lanes")
		check(camera.position.distance_to(ortho_position) < 0.01, "Orthographic zoom keeps the camera position unchanged")
		for perspective in [false, true]:
			game.perspective_view = perspective
			game.follow_camera = false
			game.set_follow_zoom_percent(0.0)
			settle(game)
			var overview_position: Vector3 = camera.position
			var overview_size: float = camera.size
			game.set_follow_zoom_percent(-50.0)
			settle(game)
			check(camera.position.distance_to(overview_position) < 0.01 and is_equal_approx(camera.size, overview_size), "Follow zoom never changes overview framing")
	game.touch_device = true
	game.top_down_camera = true
	game.follow_camera = true
	for size in [Vector2i(320, 932), Vector2i(390, 844), Vector2i(768, 1024)]:
		viewport.size = size
		game.layout_ui.call()
		for mode in [Vector2i(0, 1), Vector2i(1, 1), Vector2i(1, 0)]:
			game.perspective_view = mode.x == 1
			game.top_down_camera = mode.y == 1
			for zoom in [-50.0, 0.0, 100.0]:
				game.set_follow_zoom_percent(zoom)
				var reference_offsets: Array[Vector2] = []
				for row in [-6.0, 0.0, 7.0]:
					place_frog(game, 0.0, row)
					settle(game)
					var frog_center := camera.unproject_position(Vector3(0, 0.3, row))
					var offsets: Array[Vector2] = []
					for corner in [Vector3(-0.4, 0, -0.4), Vector3(0.4, 0.5, 0.4), Vector3(0.4, 0, -0.4)]:
						offsets.append(camera.unproject_position(Vector3(0, 0.3, row) + corner) - frog_center)
					if reference_offsets.is_empty():
						reference_offsets = offsets
					else:
						for index in range(offsets.size()):
							check(offsets[index].distance_to(reference_offsets[index]) < 0.05, "Portrait follow preserves frog size and perspective at every row")
					check(absf(game.camera_look_target.z - row) < 0.01, "Portrait camera stays centered on the followed row")
					check(absf(camera.unproject_position(Vector3(0, 0, row)).y - size.y * 0.5) < 0.05, "Portrait frog stays vertically centered at the top and bottom")
					check(camera.frustum_offset == Vector2.ZERO and is_zero_approx(camera.v_offset), "Portrait follow has no image panning")
	# A diagonal step should trail by the same amount on both axes.
	game.set_follow_zoom_percent(0.0)
	place_frog(game, 0.0, 0.0)
	settle(game)
	var before_step: Vector3 = camera.position
	place_frog(game, 1.0, 1.0)
	game.update_camera(1.0 / 60.0)
	var step_motion: Vector3 = camera.position - before_step
	check(step_motion.z > 0.0 and step_motion.z < 1.0, "Vertical follow trails a hop instead of snapping")
	check(absf(step_motion.x - step_motion.z) < 0.001, "Vertical and horizontal follow use the same smoothing")
	# The ROM resets to the starting bank before the visible home hop ends.
	# Keep following that visible arrival until its presentation releases it.
	place_frog(game, 0.0, 7.0)
	var home_frame: int = game.state["frame"]
	game.home_arrival.begin({"frame": home_frame, "x": 120, "row": 40}, false)
	for elapsed_frames in [0, 3, 6, 12, 20]:
		game.state["frame"] = home_frame + elapsed_frames
		game.update_player(0.0)
		var previous_row: float = game.camera_look_target.z
		game.update_camera(1.0 / 60.0)
		var visible_row: float = game.pos3(120, game.home_arrival.visual_row(game.state["frame"])).z
		check(absf(game.camera_look_target.z - lerpf(previous_row, visible_row, 1.0 - exp(-6.0 / 60.0))) < 0.01, "Camera smoothly follows the visible home arrival despite the ROM reset")
	game.home_arrival.reset()
	settle(game)
	check(absf(game.camera_look_target.z - 7.0) < 0.01, "Camera follows the new frog when home presentation ends")
	game.touch_device = false
	game.update_camera(1.0 / 60.0)
	check(camera.frustum_offset == Vector2.ZERO and is_zero_approx(camera.v_offset) and camera.projection == Camera3D.PROJECTION_PERSPECTIVE, "Desktop uses normal perspective after mobile follow")
	game.set_follow_zoom_percent(50.0)
	game.touch_device = true
	game.set_follow_zoom_percent(-25.0)
	game.follow_camera = true
	game.show_options(true)
	for control in game.menu_items.get_children():
		if control is CheckButton and control.text == "Top-down camera":
			var old_projection: bool = game.perspective_view
			control.button_pressed = true
			check(game.top_down_camera and game.perspective_view == old_projection, "Top-down option leaves projection selection independent")
	var slider: HSlider = game.menu_items.get_node("FollowZoomControl/FollowZoomSlider")
	var reset: Button = game.menu_items.find_child("FollowZoomDefault", true, false)
	check(is_equal_approx(slider.value, -25.0), "Slider displays the current device's negative offset")
	reset.pressed.emit()
	check(is_zero_approx(game.mobile_follow_zoom_percent) and is_equal_approx(game.desktop_follow_zoom_percent, 50.0), "Default resets only the current device")
	slider.value = -30.0
	for control in game.menu_items.get_children():
		if control is CheckButton and control.text == "Follow frog (closer view)":
			control.button_pressed = false
	check(not slider.editable and reset.disabled, "Zoom controls disable when follow is off")
	check(is_equal_approx(game.mobile_follow_zoom_percent, -30.0), "Disabling follow retains zoom preference")
	viewport.queue_free()
	await process_frame
	print("CAMERA TEST: %s" % ("PASS" if failures.is_empty() else str(failures)))
	quit(0 if failures.is_empty() else 1)
