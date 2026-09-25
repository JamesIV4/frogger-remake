# Frogger Remake

A native C# / Godot 4.7 remake of the supplied **1981 Konami arcade Frogger** set. The original main and sound programs are statically translated into C#; Godot renders an animated low-poly world made in Blender. No browser or JavaScript interpreter ships in the game.

![Godot gameplay capture](docs/evidence/gameplay.png)

## Play

Open **`godot/project.godot`** in Godot 4.7 **.NET**, build C#, and press **F5**. Or run:

```powershell
./RunGame.ps1
```

Arrow keys, WASD, or a controller D-pad/left stick move the frog. Enter/Space starts a one-player game. `1` / `2` start one or two players; two players take alternating turns. Escape/P pauses, R restarts, M toggles sound, F11 toggles fullscreen, and C/5 inserts a coin. The pause menu offers the original road collision rules or optional forgiving swept collision. River support and all other game rules remain in the recovered program.

The starting bank is completely walkable. Both grassy banks use flat low-poly surface detail. Lighting is neutral, the river has continuous animated ripples, and the HUD uses licensed arcade fonts.

## First checkout

Requirements: Godot 4.7 .NET, .NET 10 SDK, and Python 3.10+. Put the user-supplied `frogger.zip` in `reference/`, then:

```powershell
./tools/setup.ps1
```

The setup verifies the exact ROM set, prepares local data, regenerates native C#, imports assets and builds the project. It never downloads a ROM. `GODOT_EXE` can override executable discovery; on this machine Godot is in `C:\GameDev\Godot_v4.7-stable_mono_win64`.

The checked-in GLBs need no Blender installation to play. To regenerate them, install Blender 5.1+, set `BLENDER_EXE` if necessary, and run `./tools/setup.ps1 -RebuildModels`. Editable source files are in `art/source/`; the reproducible recipe is `art/scripts/build_assets.py`.

For a private Windows build, install Godot's matching .NET export templates and run `./BuildGame.ps1`. The output is under `builds/windows/`. The build includes your local ROM data and is not a ROM-free distribution.

## How fidelity is checked

This project separates **decompilation**, **native translation**, and **behavior verification**. A generated function or a successful Ghidra exit is not, by itself, a proof of gameplay parity.

- Ghidra disassembles both CPUs and exports C, an instruction listing, a call inventory and explicit computed-dispatch seeds.
- The pinned, GPL-licensed [arcade-js Frogger recovery](https://github.com/qarl/arcade-js/tree/9387bbe817befacc235cd8f9f4dd56c652172481/games/frogger) supplies readable function-level references and independent regression fixtures. It is test/reference material, not the runtime.
- `tools/recompile.py` lowers the supplied bytes into compiled C# arithmetic, calls, branches and memory operations. Original address labels, byte wrapping, BCD scores, instruction timing and interrupt order are retained. There is no runtime opcode decoder or interpreter fallback.
- A 460-frame native run matches the frozen reference across **1,530,880 bytes**, with zero differences and no masked RAM. Five separately captured **MAME 0.289** function executions match both RAM and registers.
- The coverage audit checks source hashes, every recovered main entry, actual Ghidra instruction presence, nonempty C output, generated-code provenance, parity reports and Blender export validation.

Run the complete local gate (Node 24+ and Pillow are also required for the verification tools):

```powershell
./tools/verify.ps1
./tools/decompile.ps1   # re-run Ghidra; requires Ghidra + Java 21
```

See [the decompilation and verification notes](docs/DECOMPILATION.md), [machine-readable audit](docs/evidence/audit.json), [function map](docs/evidence/function-map.json), and [asset validation](docs/evidence/asset-validation.json).

The native port is a faithful address-level translation, **not a completed handwritten, idiomatic C# rewrite**. The tests cover specific executions; they do not establish universal equivalence across every possible state. Forgiving road collision is a deliberate optional change. The sound CPU executes the recovered program, while the AY output stage uses a native synthesis model rather than MAME's exact analogue netlist.

## Files and licensing

`godot/` contains the actual game. `tools/` contains reproducible import, reverse-engineering and verification tools. `vendor/arcade-js/` is the checksum-pinned GPL-3.0-only reference; its license and attribution are retained. Fonts carry their own SIL OFL licenses in `godot/Fonts/`. See [THIRD_PARTY.md](THIRD_PARTY.md).

Git ignores `reference/`, prepared ROMs, Godot's `.godot/` and `.mono/` caches, C# build output, export credentials, build products and Blender backups. Project files, scenes, `.uid` identities, `.import` settings, C# sources, Blender sources and exported models remain tracked.
