extends SceneTree

# Repeatable CPU presentation probe; timings are diagnostic, not a CI threshold.
# Run with --headless --path godot --script ../tests/frame_probe.gd.
func _initialize() -> void:
	call_deferred("run")

func run() -> void:
	var game := FroggerGame.new()
	game.screenshot = "probe-no-save"
	root.add_child(game)
	game.set_process(false)
	game.muted = true
	game.start_game(1)
	var samples: Array[int] = []
	var visibility_changes := [0]
	for frame in range(1800):
		# Keep the player alive while lanes complete several wraps.
		game.simulation.poke(0x83dd, 60)
		game.simulation.step()
		game.observe_frame()
		game.presentation_delta = FroggerGame.FRAME_SECONDS
		var start: int = Time.get_ticks_usec()
		game.update_frog_motion()
		game.update_actors()
		game.update_camera(FroggerGame.FRAME_SECONDS)
		game.update_hud()
		samples.append(Time.get_ticks_usec() - start)
		if frame == 120:
			for actor in game.actors.values():
				actor.root.visibility_changed.connect(func(): visibility_changes[0] += 1)
		if frame % 60 == 0:
			await process_frame
	samples.sort()
	print("FRAME PROBE: ", JSON.stringify({"frames": samples.size(), "median_us": samples[900],
		"p95_us": samples[1710], "p99_us": samples[1782], "max_us": samples.back(),
		"visibility_changes": visibility_changes[0], "actors": game.actors.size()}))
	game.free()
	quit()
