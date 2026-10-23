# Art and presentation

All moving game objects use editable Blender-authored GLBs; creatures and vehicles have explicit rigs and baked animation clips. `art/models.json` is the contract for meshes, triangle counts, bone counts and animations. The frog's eight-bone rig articulates its head, forelegs, thighs and shins; turtles have six bones, and vehicles rotate their wheel bones. Models are deliberately faceted with simple material colors, following the PS1 references without copying pixel artwork into the scene.

The scene keeps the concept's five road lanes, safe banks, logs, turtles and five home bays, set into a living world instead of a wooden arcade frame (see "Backdrop world" below). Changes requested during implementation are incorporated:

- Both original bottom grass rows are traversable, including native row `0xF0`; the bottom hedge wall stands flush against the bank's grass edge.
- Grass is a low surface with flat triangular paint-like patches, not tall geometry. Each card occupies its own cell and clears the bank by 0.019 world units, avoiding coplanar overlap and ground z-fighting.
- Neutral white lighting removes the green cast. Beyond the board, depth fog melts the far woods into a bright sky haze; nothing outside the level is a flat dark backdrop.
- The board has a real cavity below the river, its sides lined in the bed's dark blue so no lighter frame shows through the water, leaving roughly 1.2 world units of translucent blue water before the bed. A maximum-depth turtle still clears the bed by 0.27 units, including its paddles. Logs and turtles float half submerged; a frog riding a diving turtle follows its smooth visual depth without changing ROM dive timing.
- A dense, spiky rear bush row sits behind five open home bays; separate goal-column hedges still block the non-home gaps.
- The top-left key light casts softened shadows toward the bottom-right; screen-space ambient occlusion grounds the models.
- A body-weighted bridge keeps the frog's broad front legs connected through a hop. The exported front and rear leg spans are checked from GLB vertices. Pink-frog facing follows the ROM's sprite direction, retaining the last direction through neutral/log-drift frames.
- The orange stripe and dark spots sample the curved frog body surface; the pink frog inherits the same fitted markings. Turtle shell plates sample their dome so the lighter polygons sit flush with dark seams, matching the supplied PS1 river reference more closely.
- The original-ROM pink-frog bug is handled only in presentation. At `0x1AA9`, the expired fly-bonus timer calls `0x27DE`, which wipes the shared sprite descriptor while the pink frog's pickup latch remains armed. `docs/evidence/lady-rom-bug.json` reproduces the exact native sequence: code/color/row/X become zero, X resumes moving with the other fields still blank, and the frog can still be picked up. The renderer uses the latch plus the ROM patrol table and drift counter when the shared descriptor is blank or repurposed as a goal bonus. This keeps her on her fixed patrol row, seated on the inset log, without a one-frame disappearance or a jump to the goal bonus. The translated ROM and scoring behavior stay unchanged. This matches the [arcade ROM fix analysis](https://forums.arcade-museum.com/threads/frogger-bugs-descriptions-and-fixes.482765/page-2).
- Turtle paddles are compact diagonal webbed fans tucked into the shell. The asset recipe checks that the facing feet of neighboring turtles leave at least 0.2 tile of space between them.
- The home gator stays above ground and slides from behind the rear hedge during the ROM's existing emerging-tile phase, reaching the full bay pose at phase `0x50`. A curved, spiked tail remains visible beside it within the bay. When the ROM clears its tile, the model retreats into the hedge over 12 presentation frames. Its upper snout opens on the Head bone while the lower jaw rests in the bay.
- The river gator shares the bush gator's rig and a narrow center snout accent, but keeps its long straight back and tail. The upper snout uses the same base shell color as the body. Its front snout is 16 native pixels long and ends at the ROM's fatal collision edge; Modern collision extends safe support across the rest of the visible 57-pixel body. Classic collision retains the original 24-pixel ride window. The model stays 14 pixels wide within its 16-pixel lane. Its tail tip stays still during the bite animation.
- Logs use irregular tapered faceted bark, cut-end growth rings, broken grain strips and knots rather than a straight cylinder. Their authored length still scales to the native lane slots. Snakes use a continuous tube with fourteen tail joints and baked traveling-wave clips. The first ROM sprite anchors the head; its historical path is stored relative to the moving log so the body follows through a U-turn instead of flipping past the log end. The same turn applies to both snake slots on logs and on land. The turn opens to a 0.1275-tile radius (1.5 times the initial 0.085-tile curl), then narrows as the tail follows. Each body section follows the authored log facets and taper so the tail stays seated instead of floating at the highest bark point. Log-end support, bank height, simulation-clock animation, and pause behavior are handled in presentation only; ROM movement and collision stay unchanged.
- The beaver keeps the runtime's original `otter` asset identifier, but has subtle dorsal fur ridges, a broad marked paddle tail with a deforming oval body join, visible incisors, small grasping forepaws and larger webbed hind paws with individually modeled toes and claws. Its model nose follows the ROM's directional hit probe while swimming half-submerged. The grab/look/sink exit preserves the last swimming position and follows subsequent log movement without snapping to an authored end cap; only the ROM's lethal player-overlap state adds an incisor bite. Offscreen spawns and retirements do not create visible exit ghosts. Slot reuse is observed on every simulation step, including steps between rendered frames, so a new animal never inherits the old one's position smoothing.
- The player frog, moving logs, turtles, cars, snakes, beavers and the pink frog use subpixel presentation smoothing. Collision, lane timing and score still read original byte positions.
- Drowning starts at the turtle's current submerged height, so a failed ride never pops the frog back to the surface. It finishes at the same depth as a death that began at the water surface, rather than passing through the riverbed. At both road edges, the flattened pose rises with its squash animation above the grass while vehicles remain in front. The impact shader blends toward color lit by each vertex's intact normal as the frog compresses, retaining the pre-squash color variation.
- Home arrivals finish the last hop into the bay for the player and carried pink frog, then hold there for 0.25 seconds or end immediately on movement input. Bug and rescued-frog points are placed once in screen space so a follow camera cannot drag them around; the time bonus has its own centered banner. Game-over text wraps within a centered panel.
- The bottom HUD no longer shows help keys and uses a shorter footer; the camera framing shifts the board down to use the freed space.
- Follow camera leaves gameplay visible above and beside the rounded top HUD. The cursor is visible in menus and hidden while playing.
- Score and time panels join the screen edges with rounded corners facing the board.
- Press Start 2P handles title/scores; Silkscreen handles small labels and menu controls. Both licenses are included.

## Backdrop world

Lanes leave the board through the world rather than vanishing at a frame. The native presentation clip stays at |x| = 7 tiles; past it, lane props fade over 0.8 tile (to black on the road, to mist on the river) before they are cut, and are drawn 13.6 px past the playfield while they fade (6.5 px for turtles, whose single rig per group must never show two wrap copies). ROM positions, collision and timing are unchanged.

- Road: a grassy portal with a sand-rimmed arch spans all five lanes on each side (`build_assets.py` `road_portal`). Six black layers inside the mouth deepen the passage to darkness. The terrain hill stands over the portal from the board edge and keeps climbing away from it, so the tunnel never ends in mid-air.
- River: the same blue river surface and material continue into a swamp on each side (the basin floor reads as the river bed's blue). An ordered wall of reeds and cattails stands along the clip line as the river's border. Beyond it, reed clumps stand in the channel water, a bed against the wall that opens within a few tiles into clear blue water with lily pads and only a faint mist; moss-hung bald cypresses stand on the lower bank. The channel floor stays below the water line so the river reads as one blue.
- Grass strips: dense hedges with the home-row hedges' proportions (big angular canopy masses every 0.84 tile, a dark 0.48-tall core and spear leaves) stand flush with the clip line on the median (where snakes enter) and the bottom strip.
- Bottom edge: a solid wall of the same hedge (`edge_hedge_front`, five 6-tile segments) stands flush against the start bank's grass edge (no curb, lip or apron), and gentle rolling hills climb away behind it; behind the home-row hedges the ground rolls away the same way. Both ends stay open and calm, with only a few bushes, trees and flowers, except a loose grove of trees with bushes along the hedge line behind the home row's left half. The wall's height and the rise stay under the lowest camera sightline to the start row (the orthographic follow camera, about 1.5 rise per tile).
- Terrain: one smooth runtime heightfield (`godot/Scripts/Scenery.gd`). The hill over each tunnel slopes across the median into the sunken swamp basin, whose banks rise back to the meadow; the meadow in front and behind stays open. Front-of-board props are height-limited so no camera angle lifts them over the start bank, and only instances whose shadows cannot reach the lanes cast shadows.

### Curved world

A presentation-only "rolling log" bend lowers every world vertex by `0.0352 * dz^2` tiles at the default 100%, where `dz` is its distance in front of or behind the camera's focus row, so the world rolls away toward the top and bottom of the screen like a log lying across the board. Options > World curve scales it from off to 200% (default 100%); it is always off in orthographic view. Every 3D object is culled with a 12-tile margin (`CurveCullMargin`) so geometry the bend lowers into view never pops in, and a cartoon-blue sky (`godot/Shaders/sky.gdshader`, horizon matched to the fog haze) fills the space the curve opens up. Low-poly 3D clouds like the key art (faceted puffs, blue shaded facets) ride a slowly turning ring around the camera at the measured pitches its views see sky through; their shader pushes their depth to the far plane, so every other 3D object always draws in front of them. The sky dome is tilted because the steep cameras mostly see rays below the true horizon once the world drops away. One shader include (`godot/Shaders/curve.gdshaderinc`, global uniforms `world_curve` and `curve_center_z`) is used by the board, actors, batched lane props, river, terrain, vegetation, mist and the drowning ripple, and the centre follows the camera's focus row. The board is sliced every half tile across its rows (`build_assets.py` `slice_board`) so wide faces bend smoothly. ROM positions, collision and timing are unaffected; screen-anchored bonus labels account for the bend.

### Backdrop vegetation

`art/scripts/build_scenery.py` (Blender) builds 16 assets: two broadleaf trees, two pines, two shrubs, two edge hedges, the reed wall, two reed clumps, grass and flower tufts, lily pads and two swamp cypresses. `art/scripts/scenery_sheets.py` composes their four Blender views (`docs/evidence/models/scenery-*-sheet.png`), and `tools/validate_assets.py` gates them (`docs/evidence/scenery-validation.json`). The production method follows the user's vegetation workflow from their other project, adapted to this game's art direction:

- Detail level matches the other game models: untextured, flat-shaded facets in palette colors carried by COLOR_0, one material, no alpha cutouts. Foliage uses the board hedges' own vocabulary: angular faceted canopy lobes broken up by large pointed spear leaves. A first pass with fine alpha-cut leaf-spray cards was rejected because its detail scale did not match the chunky board models.
- Wood is one continuous skeleton: a Skin modifier over trunk, limb and branch edges with the roots flaring out of the same surface, subdivided once, decimated to budget and faceted like the logs.
- Crowns are irregular: unequal lobes at the limb-end groups with gaps between them and darker interior lobes, never a single smooth dome or evenly spaced balls.
- Budgets: at most 6,000 triangles per asset (trees about 800). Each asset renders through one or two MultiMeshes. The whole backdrop adds about 55k primitives and 55 draw calls in the Compatibility (web) renderer.
- The manifest `art/scenery.json` records the triangles that actually ship, read back from the exported glTF; the exporter merges vertex-identical corners and drops collapsed triangles.

Workflow research (2026-10-22). The Blender manual site served a bot challenge, so the installed tools were read directly:

| Need | Source | Decision |
| --- | --- | --- |
| Branching wood as one surface | Blender 5.1 `SkinModifier` RNA (`branch_smoothing`, per-vertex skin radius, root vertex) | Skin + one Subdivision, then Decimate; roots grow from the trunk's root vertex. |
| Vertex colors to Godot | Installed glTF exporter `io_scene_gltf2/__init__.py` (`export_vertex_color` MATERIAL/ACTIVE) | A Color Attribute node drives Base Color, so COLOR_0 exports with the material. |
| Godot use of COLOR_0 | Godot `modules/gltf/gltf_document.cpp` | The importer sets albedo-from-vertex-color when COLOR_0 is present; the runtime shader multiplies it explicitly. |
| Instancing and culling | [Godot MultiMesh](https://docs.godotengine.org/en/stable/tutorials/performance/using_multimesh.html) | One MultiMesh per asset (per shadow class); culling is per MultiMesh, and the cameras see every region at once. |
| Foliage method | The user's vegetation guide and its sources ([polycount foliage](https://polycount.com/discussion/98091/best-practices-for-creating-3d-trees-for-games), [lush trees](https://gamesartist.co.uk/lush-tree/)) | Irregular crowns with gaps, continuous wood, per-part color variation and darker interiors kept; cards and textures replaced by faceted geometry to match this game. |

Regenerate the backdrop:

```powershell
& $env:BLENDER_EXE --background --python art/scripts/build_scenery.py
python art/scripts/scenery_sheets.py
python tools/validate_assets.py
```
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
