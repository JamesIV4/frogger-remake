extends SceneTree

var failures: Array[String] = []

func check(condition: bool, message: String) -> void:
	if not condition:
		failures.append(message)
		push_error(message)

func _initialize() -> void:
	run.call_deferred()

func run() -> void:
	var scene := Node3D.new()
	root.add_child(scene)
	for asset in ["frog", "lady_frog"]:
		var flattened := ModelActor.new(scene, asset)
		var intact := ModelActor.new(scene, asset)
		flattened.set_squash_color(1.0)
		intact.set_squash_color(0.0)
		check(not flattened.surface_bindings.is_empty(), "%s has shader-bound surfaces" % asset)
		for clipped in [false, true, false]:
			flattened.set_clipped(clipped)
			intact.set_clipped(clipped)
			for i in range(flattened.surface_bindings.size()):
				var binding: Dictionary = flattened.surface_bindings[i]
				var other: Dictionary = intact.surface_bindings[i]
				var material: ShaderMaterial = binding.mesh.get_surface_override_material(binding.surface)
				var other_material: ShaderMaterial = other.mesh.get_surface_override_material(other.surface)
				check(material != other_material, "%s squash material is actor-owned" % asset)
				check(is_equal_approx(material.get_shader_parameter("squash_color"), 1.0), "%s retains intact-normal shading when clipping changes" % asset)
				check(is_zero_approx(other_material.get_shader_parameter("squash_color")), "%s passenger/other instance remains unaffected" % asset)
		flattened.set_squash_color(0.0)
		for materials in flattened.squash_materials.values():
			check(is_zero_approx(materials.unclipped.get_shader_parameter("squash_color")) and is_zero_approx(materials.clipped.get_shader_parameter("squash_color")), "%s restores ordinary lighting after respawn" % asset)
	var turtle_a := ModelActor.new(scene, "turtle")
	var turtle_b := ModelActor.new(scene, "turtle")
	check(turtle_a.surface_bindings[0].unclipped == turtle_b.surface_bindings[0].unclipped, "Non-frog materials remain shared")
	scene.queue_free()
	await process_frame
	print("FROG MATERIAL TEST: %s" % ("PASS" if failures.is_empty() else str(failures)))
	quit(0 if failures.is_empty() else 1)
