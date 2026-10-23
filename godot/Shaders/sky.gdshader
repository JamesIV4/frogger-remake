shader_type sky;

// Saturated cartoon-blue sky above the curved world, fading to the scene's haze
// colour at the horizon so distance-fogged scenery meets it without a seam.
// The low-poly 3D clouds are real geometry (godot/Scripts/Scenery.gd).
uniform vec3 zenith : source_color = vec3(0.13, 0.52, 1.0);
uniform vec3 horizon : source_color = vec3(0.45, 0.78, 1.0);

void sky() {
	// The camera looks steeply down and the curved world drops away beneath it,
	// so the visible sky is mostly rays below the true horizon. Tilt the dome so
	// those rays still climb through the gradient.
	float up = EYEDIR.y + 0.7;
	vec3 color = mix(horizon, zenith, smoothstep(0.1, 1.2, max(up, 0.0)));
	COLOR = color;
}
