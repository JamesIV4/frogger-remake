extends SceneTree

class TestGame extends FroggerGame:
	var saves: Array[int] = []
	func save_preferences() -> void:
		saves.append(high_score)
		score_save_pending = false
		score_save_seconds = 0.0

var failures: Array[String] = []

func check(condition: bool, message: String) -> void:
	if not condition:
		failures.append(message)
		push_error(message)

func _initialize() -> void:
	call_deferred("run")

func run() -> void:
	var game := TestGame.new()
	game.screenshot = "test-no-save"
	root.add_child(game)
	game.set_process(false)
	game.muted = true
	game._process(0.0165)
	game._process(0.0165)
	check(not game.warmup_node.visible and game.warmup_node.process_mode == Node.PROCESS_MODE_DISABLED,
		"Hidden warmup actors stop animation and skeleton processing")
	game.update_actors()
	var changes := [0]
	for view in game.actors.values():
		view.root.visibility_changed.connect(func(): changes[0] += 1)
	game.update_actors()
	check(changes[0] == 0, "Unchanged actors do not toggle visibility between presentation passes")
	var turtle_ids: Dictionary = {}
	for key in game.actors:
		if game.actors[key].model_name == "turtle":
			turtle_ids[key] = game.actors[key].root.get_instance_id()
	for frame in range(1200):
		game.simulation.poke(0x83dd, 60)
		game.simulation.step()
		game.observe_frame()
		game.update_actors()
	for key in game.actors:
		if game.actors[key].model_name == "turtle":
			check(turtle_ids.get(key) == game.actors[key].root.get_instance_id(), "Turtle wraps reuse prepared rigs")
	var green := game.actor("player", "frog")
	game.actor("player", "lady_frog")
	check(game.actor("player", "frog") == green, "Player handoff reuses each color's rig")
	check(not game.actor_variants["player"]["lady_frog"].is_active, "Previous player variant stays inactive")
	game.render_beaver("beaver-exit-test", 100, 80, 1, game.BeaverVisualPhase.Look, 0, false)
	var exit_actor: ModelActor = game.actors["beaver-exit-test"]
	game.release_beaver_actor("beaver-exit-test")
	check(not game.actors.has("beaver-exit-test") and not exit_actor.is_active, "Retired beaver leaves active keys")
	game.render_beaver("beaver-exit-next", 100, 80, 1, game.BeaverVisualPhase.Look, 0, false)
	check(game.actors["beaver-exit-next"] == exit_actor, "Next beaver exit reuses the retired rig")
	game.release_beaver_actor("beaver-exit-next")
	game.saves.clear()
	game.high_score = 0
	for address in range(0x83eb, 0x83f1):
		game.simulation.poke(address, 0)
	for score in [1, 2, 3]:
		game.simulation.poke(0x83ed, score)
		game.observe_frame()
		game.update_hud()
	check(game.saves.is_empty() and game.score_save_pending, "Rapid high scores coalesce without hop-time writes")
	game._notification(Node.NOTIFICATION_APPLICATION_FOCUS_OUT)
	check(game.saves == [30] and not game.score_save_pending, "Focus loss flushes the newest high score")
	game.high_score = 40
	game.score_save_pending = true
	game.paused = true
	game._process(0.0165)
	check(game.saves == [30, 40], "Pause flushes pending progress")
	game.score_save_pending = true
	game.score_save_seconds = 0.99
	game.paused = false
	game._process(0.02)
	check(not game.score_save_pending and game.saves.size() == 3, "Active play checkpoints within one second")
	game.free()
	if failures.is_empty():
		print("HITCH REGRESSION TEST: PASS")
	quit(0 if failures.is_empty() else 1)
