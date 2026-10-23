// Presentation-only "rolling log" bend shared by every world shader: the
// world drops away in front of and behind the camera's focus row, as if laid
// over a log lying across the board. Gameplay positions and collision never
// see it. world_curve is 0 when the Curved world option is off.
global uniform float world_curve;
global uniform float curve_center_z;

// Model-space offset that lowers a vertex by world_curve * dz^2. The distance
// is clamped so the distant flat skirt never plunges out of sight.
vec3 curve_offset(mat4 model, vec3 vertex) {
	vec3 world = (model * vec4(vertex, 1.0)).xyz;
	float dz = clamp(world.z - curve_center_z, -30.0, 30.0);
	return inverse(mat3(model)) * vec3(0.0, -world_curve * dz * dz, 0.0);
}
