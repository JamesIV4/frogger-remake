// Flat-colored models merged by TintedMesh draw every palette color in one
// surface: UV2.x names each vertex's entry in `tints`. The array keeps the
// source_color hint, so every renderer converts each color exactly as it did
// the old per-surface tint uniform. Call READ_TINT first in vertex(); `tint`
// then holds the vertex's color for fragment().
uniform vec4 tints[16] : source_color;
varying flat vec3 tint;
#define TINT_INDEX int(UV2.x + 0.5)
#define READ_TINT tint = tints[TINT_INDEX].rgb
