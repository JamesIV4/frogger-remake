# Art and presentation

All moving game objects use editable Blender-authored GLBs; creatures and vehicles have explicit rigs and baked animation clips. `art/models.json` is the contract for meshes, triangle counts, bone counts and animations. The frog's eight-bone rig articulates its head, forelegs, thighs and shins; turtles have six bones, and vehicles rotate their wheel bones. Models are deliberately faceted with simple material colors, following the PS1 references without copying pixel artwork into the scene.

The scene uses the supplied concept's wooden arcade-board framing, five road lanes, safe banks, logs, turtles and five home bays. Changes requested during implementation are incorporated:

- The complete lower bank remains traversable and visible above the timer panel.
- Grass is a low surface with flat triangular paint-like patches, not tall geometry.
- Neutral white lighting and a navy background remove the green cast.
- The river uses continuous displaced waves, normal variation and restrained moving highlights, with no square-cell pattern.
- Press Start 2P handles title/scores; Silkscreen handles small labels and menu controls. Both licenses are included.

`docs/evidence/gameplay.png` is an actual Godot capture. Four-view Blender renders are in `docs/evidence/models/`. Geometry/weight/animation validation is in `docs/evidence/asset-validation.json`. Programmatic checks are complemented by visual inspection; they do not establish physical-controller feel or subjective final art quality.

Regenerate and review:

```powershell
& $env:BLENDER_EXE --background --python art/scripts/build_assets.py
python tools/validate_assets.py
& $env:BLENDER_EXE --background --python art/scripts/render_review.py
```

Authoring coordinates are Z-up / -Y-forward; GLB is Y-up / +Z-forward. One tile is one Blender unit. Sources stay outside the Godot project so Godot does not re-run Blender during ordinary imports. Inspect exported clips after changing a rig; `.blend` files are generated, so preserve any hand edits separately.
