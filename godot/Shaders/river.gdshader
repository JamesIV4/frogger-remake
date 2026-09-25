shader_type spatial;
render_mode cull_disabled;

uniform float clock = 0.0;
varying vec2 river_position;

void vertex() {
    river_position = VERTEX.xz;
    // Gentle continuous surface motion. No cell quantization or square noise.
    VERTEX.y += sin(VERTEX.x * 1.7 + VERTEX.z * 2.2 + clock * 1.1) * 0.014;
    VERTEX.y += sin(VERTEX.x * 3.1 - VERTEX.z * 1.4 - clock * 0.8) * 0.007;
}

void fragment() {
    vec2 p = river_position;
    float a = sin(p.x * 1.6 + p.y * 2.1 + clock * 1.1);
    float b = sin(p.x * 2.9 - p.y * 1.4 - clock * 0.8);
    float wave = a * 0.65 + b * 0.35;
    float ribbons = sin(p.y * 9.0 + sin(p.x * 1.8 + clock * 0.4) * 1.8 - clock * 1.4);
    float shine = smoothstep(0.93, 1.0, ribbons) * (0.4 + 0.6 * sin(p.x * 2.3 + clock) * sin(p.x * 2.3 + clock));
    ALBEDO = mix(vec3(0.003, 0.028, 0.40), vec3(0.005, 0.16, 0.78), wave * 0.35 + 0.5);
    ALBEDO += vec3(0.08, 0.19, 0.34) * shine * 0.4;
    NORMAL_MAP = normalize(vec3(-0.10 * cos(p.x * 1.6 + p.y * 2.1 + clock * 1.1),
                               -0.06 * cos(p.x * 2.9 - p.y * 1.4 - clock * 0.8), 1.0)) * 0.5 + 0.5;
    ROUGHNESS = 0.26;
    METALLIC = 0.1;
    SPECULAR = 0.7;
}
