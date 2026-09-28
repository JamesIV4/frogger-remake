extends SceneTree

var failures: Array[String] = []

func check(condition: bool, message: String) -> void:
	if not condition and not failures.has(message):
		failures.append(message)
		push_error(message)

func _initialize() -> void:
	call_deferred("run")

func run() -> void:
	var game = load("res://Scripts/FroggerGame.gd").new()
	root.add_child(game)
	game.set_process(false)
	for render_interval in [1, 3]:
		game.reset_machine()
		for frame in range(180):
			game.simulation.step()
		game.start_game(1)
		for frame in range(80):
			game.simulation.step()
		game.simulation.poke(0x83b7, 3)
		var retirements: int = 0
		var last_x: float = 0.0
		var last_phase: int = game.BeaverVisualPhase.Hidden
		var last_generation: int = -1
		var last_active: bool = false
		var previous_visuals: Dictionary = {}
		var overlapping_exits: int = 0
		for frame in range(6000):
			# Exercise actual spawn, steering, lane drift, wrap and retirement.
			# Keep the player out of danger and prevent a timer death only.
			game.simulation.poke(0x8044, 120)
			game.simulation.poke(0x8047, 224)
			game.simulation.poke(0x83dd, 200)
			game.simulation.step()
			game.observe_frame()
			if frame % render_interval != 0:
				continue
			game.presentation_delta = game.FRAME_SECONDS * render_interval
			game.update_actors()
			var beaver: ModelActor = game.actors.get("hazard32856")
			var active: bool = beaver != null and beaver.is_active
			# Check each animal across sprite generations, including the old
			# model after native slot reuse. The original test skipped this case.
			var current_visuals: Dictionary = {}
			if active:
				current_visuals[game.beaver_display_generation] = beaver.root.position
			for exit_visual in game.beaver_exits:
				var exit_actor: ModelActor = game.actors.get(exit_visual.key)
				check(exit_actor != null and exit_actor.is_active, "Retained beaver exits have a visible actor until their dive finishes")
				if exit_actor != null:
					current_visuals[exit_visual.generation] = exit_actor.root.position
			for generation in previous_visuals:
				var old_position: Vector3 = previous_visuals[generation]
				if current_visuals.has(generation):
					var new_position: Vector3 = current_visuals[generation]
					check(absf(new_position.x - old_position.x) <= 0.25 * render_interval and is_equal_approx(new_position.z, old_position.z),
						"A visible animal keeps its position and row when the sprite slot is reused")
				elif absf(old_position.x) < 5.0:
					check(old_position.y + game.BeaverScale * ModelFootprints.BeaverTopTiles <= BoardVisuals.WaterSurfaceHeight - 0.049,
						"An animal in the middle of the river cannot disappear before fully submerging")
			if last_active and last_generation != game.beaver_generation and current_visuals.has(last_generation):
				overlapping_exits += 1
			previous_visuals = current_visuals
			if game.beaver_phase == game.BeaverVisualPhase.Approach:
				var slot_x: int = game.simulation.peek(0x8058)
				if slot_x < 8 or slot_x > 235:
					check(not active, "Offscreen native sprites do not flash a model at a wrapped edge")
				elif active:
					var probe: float = slot_x + (20.0 if game.beaver_heading > 0 else -4.0)
					var nose: float = game.beaver_last_x + game.beaver_heading * 16.0 * game.BeaverScale * ModelFootprints.BeaverFrontTiles
					check(absf(nose - probe - game.beaver_swim_offset) <= 3.0,
						"Native swimming motion is retained with only a fixed cap alignment offset")
			if last_active and active and last_generation == game.beaver_generation:
				check(absf(game.beaver_last_x - last_x) <= 4.0 * render_interval,
					"A continuing native beaver or its exit never teleports to a log end")
				if last_phase == game.BeaverVisualPhase.Approach and game.beaver_phase == game.BeaverVisualPhase.Grab:
					retirements += 1
			last_x = game.beaver_last_x
			last_phase = game.beaver_phase
			last_generation = game.beaver_generation
			last_active = active
		check(retirements >= 10, "Live replay exercises at least ten visible retirements at each render rate")
		check(overlapping_exits >= 10, "Replay exercises at least ten exits overlapping a new native spawn")
		# Finish without another native spawn and ensure temporary actors retire.
		game.simulation.poke(0x8486, 0)
		for address in range(0x8058, 0x805c):
			game.simulation.poke(address, 0)
		game.observe_frame()
		for frame in range(120):
			game.update_actors()
		check(game.beaver_exits.is_empty(), "Detached beaver exits are reclaimed after submersion")
	# Contact can also occur away from an end cap (including the player-hit
	# branch). An exit must never relocate the animal to start its animation.
	for hit in [false, true]:
		game.clear_presentation()
		for address in range(0x8480, 0x8490):
			game.simulation.poke(address, 0)
		game.simulation.poke(0x8112, 1)
		game.simulation.poke(0x8113, 166)
		game.simulation.poke(0x8486, 1)
		game.simulation.poke(0x8058, 120)
		game.simulation.poke(0x8059, 0x27)
		game.simulation.poke(0x805b, 80)
		game.observe_frame()
		game.update_actors()
		var before_exit: float = game.beaver_last_x
		game.simulation.poke(0x8486, 2 if hit else 0)
		if not hit:
			for address in range(0x8058, 0x805c):
				game.simulation.poke(address, 0)
		game.observe_frame()
		game.update_actors()
		check(is_equal_approx(game.beaver_last_x, before_exit), "Retirement and player contact preserve the last model position")
	# A clear and nearby replacement can both occur between rendered frames.
	game.clear_presentation()
	game.simulation.poke(0x8486, 1)
	game.simulation.poke(0x8058, 120)
	game.simulation.poke(0x8059, 0x27)
	game.simulation.poke(0x805b, 80)
	game.observe_frame()
	game.update_actors()
	game.simulation.poke(0x8486, 0)
	game.observe_frame()
	game.simulation.poke(0x8486, 1)
	game.simulation.poke(0x8058, 125)
	game.simulation.poke(0x805b, 48)
	game.observe_frame()
	game.update_actors()
	var nose: float = game.beaver_last_x + 16.0 * game.BeaverScale * ModelFootprints.BeaverFrontTiles
	check(is_equal_approx(nose, 145.0), "Slot reuse between renders resets the old spawn's smoothing")
	game.queue_free()
	if failures.is_empty():
		print("BEAVER MOTION TEST: PASS (12,000 native frames, rendering every 1 or 3 steps)")
	quit(0 if failures.is_empty() else 1)
