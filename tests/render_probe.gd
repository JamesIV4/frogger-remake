extends SceneTree

# Repeatable GPU/CPU render probe for the live game loop; timings are
# diagnostic, not a CI threshold. It needs a real window (not --headless):
#   godot --path godot --script ../tests/render_probe.gd [-- --frames=600]
# Add --rendering-method gl_compatibility to measure the web renderer.
var frames: int = 600

# The screenshot path keeps default options and skips preference I/O; the probe
# never captures or quits through it.
class ProbeGame extends FroggerGame:
	func capture_screenshot() -> void:
		pass

func _initialize() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--frames="):
			frames = maxi(60, int(arg.substr(9)))
	call_deferred("run")

func percentile(values: Array, p: float) -> float:
	var sorted := values.duplicate()
	sorted.sort()
	return sorted[clampi(int(p * float(sorted.size() - 1)), 0, sorted.size() - 1)]

func measure(game: ProbeGame, label: String) -> Dictionary:
	var viewport_rid: RID = root.get_viewport_rid()
	var cpu: Array = []
	var gpu: Array = []
	var script: Array = []
	var draws: Array = []
	var shadow_draws: Array = []
	var primitives: Array = []
	var shadow_primitives: Array = []
	var objects: Array = []
	# Let pipelines, shadows and the camera settle first.
	for i in range(90):
		game.simulation.poke(0x83dd, 60)
		await process_frame
	for i in range(frames):
		game.simulation.poke(0x83dd, 60)
		var start: int = Time.get_ticks_usec()
		await process_frame
		script.append(float(Time.get_ticks_usec() - start) / 1000.0)
		cpu.append(RenderingServer.viewport_get_measured_render_time_cpu(viewport_rid) + RenderingServer.get_frame_setup_time_cpu())
		gpu.append(RenderingServer.viewport_get_measured_render_time_gpu(viewport_rid))
		draws.append(RenderingServer.viewport_get_render_info(viewport_rid, RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, RenderingServer.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME))
		shadow_draws.append(RenderingServer.viewport_get_render_info(viewport_rid, RenderingServer.VIEWPORT_RENDER_INFO_TYPE_SHADOW, RenderingServer.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME))
		primitives.append(RenderingServer.viewport_get_render_info(viewport_rid, RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME))
		shadow_primitives.append(RenderingServer.viewport_get_render_info(viewport_rid, RenderingServer.VIEWPORT_RENDER_INFO_TYPE_SHADOW, RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME))
		objects.append(RenderingServer.viewport_get_render_info(viewport_rid, RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, RenderingServer.VIEWPORT_RENDER_INFO_OBJECTS_IN_FRAME))
	return {"view": label,
		"frame_ms_median": snappedf(percentile(script, 0.5), 0.01), "frame_ms_p99": snappedf(percentile(script, 0.99), 0.01),
		"render_cpu_ms": snappedf(percentile(cpu, 0.5), 0.01), "gpu_ms_median": snappedf(percentile(gpu, 0.5), 0.01),
		"gpu_ms_p95": snappedf(percentile(gpu, 0.95), 0.01),
		"draw_calls": percentile(draws, 0.5), "shadow_draw_calls": percentile(shadow_draws, 0.5),
		"primitives": percentile(primitives, 0.5), "shadow_primitives": percentile(shadow_primitives, 0.5), "objects": percentile(objects, 0.5)}

func run() -> void:
	DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_DISABLED)
	Engine.max_fps = 0
	RenderingServer.viewport_set_measure_render_time(root.get_viewport_rid(), true)
	var game := ProbeGame.new()
	game.screenshot = "probe-no-save"
	root.add_child(game)
	var results: Array = []
	# The screenshot path has already started a one-player game.
	results.append(await measure(game, "follow"))
	game.follow_camera = false
	results.append(await measure(game, "overview"))
	for result in results:
		print("RENDER PROBE: ", JSON.stringify(result))
	print("RENDER PROBE renderer: ", RenderingServer.get_current_rendering_method(), " ", RenderingServer.get_video_adapter_name(),
		" ", root.get_visible_rect().size)
	game.free()
	quit()
