# Art and presentation

All moving game objects use editable Blender-authored GLBs; creatures and vehicles have explicit rigs and baked animation clips. `art/models.json` is the contract for meshes, triangle counts, bone counts and animations. The frog's eight-bone rig articulates its head, forelegs, thighs and shins; turtles have six bones, and vehicles rotate their wheel bones. Models are deliberately faceted with simple material colors, following the PS1 references without copying pixel artwork into the scene.

The scene uses the supplied concept's wooden arcade-board framing, five road lanes, safe banks, logs, turtles and five home bays. Changes requested during implementation are incorporated:

- Both original bottom grass rows are traversable, including native row `0xF0`; the wooden lip sits behind the frog's feet.
- Grass is a low surface with flat triangular paint-like patches, not tall geometry. Each card occupies its own cell and clears the bank by 0.019 world units, avoiding coplanar overlap and ground z-fighting.
- Neutral white lighting and a navy background remove the green cast.
- The board has a real cavity below the river, leaving roughly 1.2 world units of translucent blue water before the bed. A maximum-depth turtle still clears the bed by 0.27 units, including its paddles. Logs and turtles float half submerged; a frog riding a diving turtle follows its smooth visual depth without changing ROM dive timing.
- A dense, spiky rear bush row sits behind five open home bays; separate goal-column hedges still block the non-home gaps.
- The top-left key light casts softened shadows toward the bottom-right; screen-space ambient occlusion grounds the models.
- A body-weighted bridge keeps the frog's broad front legs connected through a hop. The exported front and rear leg spans are checked from GLB vertices. Pink-frog facing follows the ROM's sprite direction, retaining the last direction through neutral/log-drift frames.
- The orange stripe and dark spots sample the curved frog body surface; the pink frog inherits the same fitted markings. Turtle shell plates sample their dome so the lighter polygons sit flush with dark seams, matching the supplied PS1 river reference more closely.
- Turtle paddles are compact diagonal webbed fans tucked into the shell. The asset recipe checks that the facing feet of neighboring turtles leave at least 0.2 tile of space between them.
- The home gator stays above ground and slides from behind the rear hedge during the ROM's existing emerging-tile phase, reaching the full bay pose at phase `0x50`. A curved, spiked tail remains visible beside it within the bay. When the ROM clears its tile, the model retreats into the hedge over 12 presentation frames. Its upper snout opens on the Head bone while the lower jaw rests in the bay.
- The river gator shares the bush gator's rig and contoured, bright center snout accent, but keeps its long straight back and tail. Its front snout is 16 native pixels long and ends at the ROM's fatal collision edge; the safe body extends behind it. The whole model remains 57 pixels long and 14 pixels wide within its 60-by-16-pixel lane slot. Its tail tip stays still during the bite animation; the ROM remains responsible for contact.
- Logs use irregular tapered faceted bark, cut-end growth rings, broken grain strips and knots rather than a straight cylinder. Their authored length still scales to the native lane slots. Snakes use one continuous skinned body with color-banded segments, sit on the inset log surface, and face their actual visible travel direction even when ROM sprite flip and lane scroll disagree.
- The player frog, moving logs, turtles, cars, snakes, otters and the pink frog use subpixel presentation smoothing. Collision, lane timing and score still read original byte positions.
- Drowning starts at the turtle's current submerged height, so a failed ride never pops the frog back to the surface. It finishes at the same depth as a death that began at the water surface, rather than passing through the riverbed. At both road edges, the flattened pose rises with its squash animation above the grass while vehicles remain in front. The impact shader blends toward color lit by each vertex's intact normal as the frog compresses, retaining the pre-squash color variation.
- Home arrivals finish the last hop into the bay for the player and carried pink frog, then hold there for 0.25 seconds or end immediately on movement input. Bug and rescued-frog points are placed once in screen space so a follow camera cannot drag them around; the time bonus has its own centered banner. Game-over text wraps within a centered panel.
- The bottom HUD no longer shows help keys and uses a shorter footer; the camera framing shifts the board down to use the freed space.
- Follow camera leaves gameplay visible above and beside the rounded top HUD. The cursor is visible in menus and hidden while playing.
- Score and time panels join the screen edges with rounded corners facing the board.
- Press Start 2P handles title/scores; Silkscreen handles small labels and menu controls. Both licenses are included.

`docs/evidence/gameplay.png` is an actual Godot capture. Four-view Blender renders are in `docs/evidence/models/`. Geometry/weight/animation validation is in `docs/evidence/asset-validation.json`. Programmatic checks are complemented by visual inspection; they do not establish physical-controller feel or subjective final art quality.

Regenerate and review:

```powershell
& $env:BLENDER_EXE --background --python art/scripts/build_assets.py
python tools/validate_assets.py
& $env:BLENDER_EXE --background --python art/scripts/render_review.py
& $env:BLENDER_EXE --background --python art/scripts/render_gator_bite.py
$env:FROGGER_GATOR_REVIEW_ASSET='river_gator'; & $env:BLENDER_EXE --background --python art/scripts/render_gator_bite.py; Remove-Item Env:FROGGER_GATOR_REVIEW_ASSET
```

Authoring coordinates are Z-up / -Y-forward; GLB is Y-up / +Z-forward. One tile is one Blender unit. Sources stay outside the Godot project so Godot does not re-run Blender during ordinary imports. Inspect exported clips after changing a rig; `.blend` files are generated, so preserve any hand edits separately.
