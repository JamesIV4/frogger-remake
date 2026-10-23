class_name TintedMesh

## Untextured models author one surface per palette color, and every surface
## is a separate draw call in the color pass and again in each shadow pass.
## This folds a model's surfaces into one: each vertex carries its palette slot
## in UV2.x, and the shaders read the colors from a `source_color` array
## (Shaders/tint.gdshaderinc), so the rendered colors are unchanged. Only
## position, normal and skin data are kept; no shader here samples UVs or
## tangents.

const MaxSlots: int = 16

## The slot of `value` in `palette`, appending it when new.
static func slot(palette: Array, value: Variant) -> int:
	var index: int = palette.find(value)
	if index >= 0:
		return index
	if palette.size() >= MaxSlots:
		push_error("A merged mesh needs more than %d palette slots" % MaxSlots)
		return MaxSlots - 1
	palette.append(value)
	return palette.size() - 1

## The authored albedo of an imported surface, which the merged shaders take
## over from the StandardMaterial3D.
static func surface_color(mesh: Mesh, surface: int) -> Variant:
	var material := mesh.surface_get_material(surface) as BaseMaterial3D
	return material.albedo_color if material != null else null

## Merges surfaces into one triangle surface. Each part is a Dictionary with
## `mesh`, `surface` and `slot`, plus an optional `transform` for rigid parts.
## Skinned parts must share one Skin, so their bone indices carry over as-is.
static func merge(parts: Array) -> ArrayMesh:
	var vertices := PackedVector3Array()
	var normals := PackedVector3Array()
	var slots := PackedVector2Array()
	var bones := PackedInt32Array()
	var weights := PackedFloat32Array()
	var indices := PackedInt32Array()
	var skinned: bool = false
	for part_index in range(parts.size()):
		var part: Dictionary = parts[part_index]
		var arrays: Array = (part.mesh as Mesh).surface_get_arrays(part.surface)
		var part_vertices: PackedVector3Array = arrays[Mesh.ARRAY_VERTEX]
		var part_normals: PackedVector3Array = arrays[Mesh.ARRAY_NORMAL]
		var part_skinned: bool = arrays[Mesh.ARRAY_BONES] != null and not (arrays[Mesh.ARRAY_BONES] as PackedInt32Array).is_empty()
		if part_index == 0:
			skinned = part_skinned
		elif part_skinned != skinned:
			push_error("Cannot merge skinned and rigid surfaces")
			return null
		var base: int = vertices.size()
		var transform: Transform3D = part.get("transform", Transform3D.IDENTITY)
		if transform.is_equal_approx(Transform3D.IDENTITY):
			vertices.append_array(part_vertices)
			normals.append_array(part_normals)
		else:
			var normal_basis: Basis = transform.basis.inverse().transposed()
			for vertex in part_vertices:
				vertices.append(transform * vertex)
			for normal in part_normals:
				normals.append((normal_basis * normal).normalized())
		var part_slots := PackedVector2Array()
		part_slots.resize(part_vertices.size())
		part_slots.fill(Vector2(float(part.slot), 0.0))
		slots.append_array(part_slots)
		if skinned:
			bones.append_array(arrays[Mesh.ARRAY_BONES])
			weights.append_array(arrays[Mesh.ARRAY_WEIGHTS])
		var part_indices = arrays[Mesh.ARRAY_INDEX]
		if part_indices == null or (part_indices as PackedInt32Array).is_empty():
			for i in range(part_vertices.size()):
				indices.append(base + i)
		else:
			for index in part_indices:
				indices.append(base + index)
	var merged: Array = []
	merged.resize(Mesh.ARRAY_MAX)
	merged[Mesh.ARRAY_VERTEX] = vertices
	merged[Mesh.ARRAY_NORMAL] = normals
	merged[Mesh.ARRAY_TEX_UV2] = slots
	merged[Mesh.ARRAY_INDEX] = indices
	if skinned:
		merged[Mesh.ARRAY_BONES] = bones
		merged[Mesh.ARRAY_WEIGHTS] = weights
	var mesh := ArrayMesh.new()
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, merged)
	return mesh

## Every surface of `mesh` as merge parts, slotting each surface's color into
## `palette`. Returns an empty array when a surface has no authored color.
static func color_parts(mesh: Mesh, palette: Array, transform: Transform3D = Transform3D.IDENTITY) -> Array:
	var parts: Array = []
	for surface in range(mesh.get_surface_count()):
		var color: Variant = surface_color(mesh, surface)
		if color == null:
			push_error("Merged surfaces need an authored tint")
			return []
		parts.append({"mesh": mesh, "surface": surface, "slot": slot(palette, color), "transform": transform})
	return parts

## The palette as the shaders' `tints` uniform value.
static func tints(palette: Array) -> PackedColorArray:
	var colors := PackedColorArray()
	for color in palette:
		colors.append(color)
	colors.resize(MaxSlots)
	return colors
