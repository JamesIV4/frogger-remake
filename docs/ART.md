# Art and presentation

All moving game objects use editable Blender-authored GLBs; creatures and vehicles have explicit rigs and baked animation clips. `art/models.json` is the contract for meshes, triangle counts, bone counts and animations. The frog's eight-bone rig articulates its head, forelegs, thighs and shins; turtles have six bones, and vehicles rotate their wheel bones. Models are deliberately faceted with simple material colors, following the PS1 references without copying pixel artwork into the scene.

The scene uses the supplied concept's wooden arcade-board framing, five road lanes, safe banks, logs, turtles and five home bays. Changes requested during implementation are incorporated:

- Both original bottom grass rows are traversable, including native row `0xF0`; the wooden lip sits behind the frog's feet.
- Grass is a low surface with flat triangular paint-like patches, not tall geometry.
- Neutral white lighting and a navy background remove the green cast.
- The board has a real cavity below the river, leaving roughly 1.2 world units of translucent blue water before the bed. A maximum-depth turtle still clears the bed by 0.27 units, including its paddles. Logs and turtles float half submerged; a frog riding a diving turtle follows its smooth visual depth without changing ROM dive timing.
- A dense, spiky rear bush row sits behind five open home bays; separate goal-column hedges still block the non-home gaps.
- The top-left key light casts softened shadows toward the bottom-right; screen-space ambient occlusion grounds the models.
- A body-weighted bridge keeps the frog's broad front legs connected through a hop. The exported front and rear leg spans are checked from GLB vertices. Pink-frog facing follows the ROM's sprite direction, retaining the last direction through neutral/log-drift frames.
- Moving logs, turtles, cars, snakes, otters and the pink frog use subpixel presentation smoothing. Collision, lane timing and score still read original byte positions.
- Score and time panels join the screen edges with rounded corners facing the board.
- Press Start 2P handles title/scores; Silkscreen handles small labels and menu controls. Both licenses are included.

`docs/evidence/gameplay.png` is an actual Godot capture. Four-view Blender renders are in `docs/evidence/models/`. Geometry/weight/animation validation is in `docs/evidence/asset-validation.json`. Programmatic checks are complemented by visual inspection; they do not establish physical-controller feel or subjective final art quality.

Regenerate and review:

```powershell
& $env:BLENDER_EXE --background --python art/scripts/build_assets.py
python tools/validate_assets.py
& $env:BLENDER_EXE --background --python art/scripts/render_review.py
```

Authoring coordinates are Z-up / -Y-forward; GLB is Y-up / +Z-forward. One tile is one Blender unit. Sources stay outside the Godot project so Godot does not re-run Blender during ordinary imports. Inspect exported clips after changing a rig; `.blend` files are generated, so preserve any hand edits separately.
