extends SceneTree

class ProbeGame extends FroggerGame:
	func save_preferences() -> void:
		pass

# Diagnostic fixture: stop top-lane drift through its RAM speed/phase bytes;
# leave ROM instructions and the other lanes running normally.
# Add -- --screenshot=<absolute path> for a rendered occupied-home left-hop
# capture, or --classic to reproduce the original stall. Never sends OS input.
func _initialize() -> void:
	call_deferred("run")

func place_on_log(sim: RefCounted, occupied: bool) -> void:
	sim.poke(0x8044, 120)
	sim.poke(0x8047, 48)
	sim.poke(0x8004, 0)
	sim.poke(0x83cd, 0)
	sim.poke(0x8268, 0)
	for address in range(0x8248, 0x8254):
		sim.poke(address, 0)
	sim.poke(0x819b, 0)
	sim.poke(0x81a6, 0)
	sim.poke(0x8100, 1)
	sim.poke(0x8101, 160)
	sim.poke(0x8260, 1 if occupied else 0)

func run() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--screenshot="):
			await render_probe(arg.substr(13))
			return
	var rom := FileAccess.get_file_as_bytes("res://rom/maincpu.bin")
	var sound_rom := FileAccess.get_file_as_bytes("res://rom/audiocpu.bin")
	for occupied in [false, true]:
		for direction in [4, 8, 2, 1]:
			var sim: RefCounted = ClassDB.instantiate("ArcadeSimulation")
			sim.setup(rom, not OS.get_cmdline_user_args().has("--classic"), sound_rom)
			for frame in range(310):
				sim.step(16 if frame >= 150 and frame < 156 else (32 if frame >= 230 and frame < 236 else 0))
			place_on_log(sim, occupied)
			var trace: Array = []
			for frame in range(24):
				var start := Time.get_ticks_usec()
				sim.step(direction)
				trace.append([sim.peek(0x8044), sim.peek(0x8047), sim.peek(0x8004), Time.get_ticks_usec() - start])
			print(JSON.stringify({"occupied": occupied, "direction": direction, "trace_x_row_dead_us": trace}))
	quit()

func render_probe(path: String) -> void:
	var game := ProbeGame.new()
	root.add_child(game)
	game.set_process(false)
	game.muted = true
	game.modern = not OS.get_cmdline_user_args().has("--classic")
	game.start_game(1)
	while game.simulation.get_frame() < 310:
		game.simulation.step()
	place_on_log(game.simulation, true)
	game.clear_presentation()
	game.observe_frame()
	game.follow_camera = false
	for frame in range(48):
		# Settle the camera, then perform one ordinary left-button press.
		game.simulation.step(4 if frame >= 24 else 0)
		game.observe_frame()
		game.presentation_delta = FroggerGame.FRAME_SECONDS
		game.update_frog_motion()
		game.update_actors()
		game.update_camera(FroggerGame.FRAME_SECONDS)
		game.update_hud()
		await process_frame
	await RenderingServer.frame_post_draw
	var error := root.get_texture().get_image().save_png(path)
	print("TOP ROW CAPTURE: modern=", game.modern, " x=", game.simulation.peek(0x8044),
		" row=", game.simulation.peek(0x8047), " error=", error)
	var moved_correctly: bool = game.simulation.peek(0x8044) == (104 if game.modern else 120)
	game.free()
	quit(0 if error == OK and moved_correctly else 1)
