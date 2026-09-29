# Binary recovery and verification

## Input and architecture

The supplied `reference/frogger.zip` is the Konami parent arcade set: `frogger.26`, `frogger.27`, `frsm3.7` form the 12 KiB main program. The runtime address space pads the unpopulated 0x3000–0x3fff range with zeros. The three 2 KiB sound parts are `frogger.608/609/610`; sound ROM 0x0000–0x07ff swaps D0 and D1, as does graphics part `frogger.606`. `tools/prepare_rom.py` preserves the source archive and checks SHA-256 against the pinned set.

Main CPU: Z80, 3,072,000 Hz, 50,688 cycles per frame (2000/33 Hz). Sound CPU / AY clock: 14,318,181/8 Hz. Main RAM is 0x8000–0x87ff, video RAM 0xa800–0xabff, object RAM 0xb000–0xb0ff. Mirroring, open-bus reads, i8255 directions and sound IRQ edges are modeled from MAME's Galaxian driver.

## What the native source is

`tools/recompile.py` deterministically lowers decoded instructions into native C# cases, split into small address-page methods. Operand values and branch destinations are compiled constants. A program counter retains original jump/call/interrupt flow; there is **no runtime opcode decoder, JavaScript interpreter, process bridge or browser**. `Z80Program.cs` implements width-limited registers and shared arithmetic/flags; `ArcadeSimulationNative.cs` supplies the exact board memory and NMI schedule. The sound CPU uses the same native support and its separately generated program.

All populated offsets are compiled so computed branches and overlapping instruction streams have concrete destinations. This deliberately includes speculative decodes of data bytes. **18,432 compiled offsets are not 18,432 recovered functions.** The generated code is low-level, address-preserving C#, not a claim of a polished handwritten high-level port.

Readable, named recovery of the main program is provided by the pinned GPL `vendor/arcade-js/games/frogger/idiomatic/` reference. It contains **165** registry entries at the pinned commit. The upstream DONE document's older 164 count is stale. Its English comments are guidance, not ground truth: for example, 0x83e5/0x83e6 are remaining frogs, while 0x83dd is the visible countdown bar. The native core follows ROM operations, not those labels.

## Ghidra pass

`tools/ghidra/ExportFrogger.java` imports `z80:LE:16:default`, maps RAM/I/O, seeds reset/interrupt vectors plus computed dispatch entries, runs analysis, and exports C, instructions, calls and an inventory. `tools/ghidra/seeds.tsv` includes all registered main routines and known computed targets. Audio seeds come from actual executed calls and indirect targets in `docs/evidence/sound-execution.json`, collected while running the original sound command set natively. They can be regenerated with `tools/audio_seeds.py`.

The clean re-import maps **165/165 main recovered entries and 82/82 sound entries** to both real Ghidra instructions and compiled native entries. Ghidra emitted C for **266 main candidates and 87 sound candidates**. Candidate totals include internal entries and depend on analysis boundaries; they do not establish a final semantic function count. The audit checks the actual exported artifacts instead of trusting an agent's progress description.

One concrete false positive was caught at **0x0f8c**: automatic analysis had decoded a `JR` from operand byte 0x0f8d, leaving the real entry with a one-byte empty body. Ghidra reported decompilation completed anyway. Re-seeding the actual `LD A,(0x8118)` boundary recovered the guarded eight-row blit. The audit now requires a real instruction at every named entry, as well as C output and a native case.

Headless Ghidra can return exit code 0 even when a Java post-script fails. `tools/decompile.ps1` checks the script's own completion marker and rejects script-error logs. Computed jumps can still produce Ghidra warning text or speculative destinations; those warnings are not silently promoted to semantic proof.

## Independent checks

| Gate | What is compared | Current result |
| --- | --- | --- |
| ROM provenance | Assembled input hashes | Exact pinned set |
| Native whole-program replay | Every main RAM, VRAM and object-RAM byte over 460 frames: boot, coin, start, hop | 1,530,880 bytes, zero differences, zero masked bytes |
| MAME function fixtures | Before/after snapshots of ROM 0x11bf, 0x14b7, 0x08e0, 0x1cff, 0x16f8, three contacts at 0x28bb and eleven beaver branches at 0x2b83; native execution starts from MAME's captured entry registers and RAM | Nineteen executions of seven functions match all 3,328 state bytes and all captured registers |
| Recovered-reference suite | Function equivalence, mutation controls, board hardware and new provenance checks | 439 pass, 0 fail; one optional recorded-audio-file check skipped |
| Lifecycle scenarios | 9,000-frame idle run through timer deaths and game-over; two-player hand-off; five forced safe home entries followed by ordinary board progression | Passed; next board reached |
| Native sound | Original sound CPU and commands running through the compiled program | 1,425,600 samples in the 1,800-frame test; non-silent output; full command sweep expands executed-code coverage |
| Blender exports | Finite geometry, topology counts, bound vertices, normalized skin weights, exact bone counts and moving animation channels | 14 assets pass |

MAME fixtures were captured with MAME 0.289, whose downloaded binary's SHA-256 matched its official release checksums. `tools/mame_functions.lua` installs opcode-read taps gated by CURPC, captures real entry registers/RAM, and waits for the actual return PC and stack depth to capture the exit. The tape uses real coin/start controls plus explicit home/death fixtures to reach the selected branches. These fixture pokes are test setup, not normal game behavior.

Re-capture fixtures, if desired:

```powershell
$env:FROGGER_EVIDENCE = "$PWD/docs/evidence/mame-functions"
./reference/mame/mame.exe frogger -rompath reference -video none -sound none -nothrottle -seconds_to_run 12 -skip_gameinfo -nocheat -noautosave -nonvram_save -cfg_directory reference/mame-test-config -nvram_directory reference/mame-test-config -autoboot_script tools/mame_functions.lua
./tools/verify.ps1
```

The equivalence gate has limits: 460 frames and nineteen independent function fixtures are concrete evidence, not an exhaustive proof over every possible machine state. The [beaver movement audit](BEAVER-MOTION.md) additionally checked 2,000 consecutive dispatcher executions against MAME and retained eleven representative branches. Ghidra producing C is a weaker, separate fact. `docs/evidence/audit.json` records these distinctions.

## Presentation and intentional differences

The game uses original object lists and sprite/VRAM state to place Blender models. Turtle submergence and home creatures are read from the original tile state, not independent random timers. A framebuffer is never drawn as the game world. The bank at row 0 remains open; grass is flat surface detail. The visual layer smooths stepped object positions and finishes hop/drowning poses without modifying the recovered RAM or timing.

The default model-sized road hook at **0x11bf** uses vehicle front, rear and lateral bounds generated from actual Blender vertices, rotated into each lane's direction. It tests the whole frog footprint on both sides and sweeps relative vehicle/frog movement between original NMIs. The Modern collision hook at **0x28bb** recognizes the visible safe back across byte-coordinate wrapping and delegates head contacts to the original routine. Neither mode sets a death flag for a safe back landing. The menu's **Classic collision (original ROM)** option executes both original routines unchanged and is used for reference comparisons. These hooks intentionally change contact decisions and instruction timing; other river supports, diving, hazards, goals, RNG, scores, timers and progression remain in the recovered program. Held movement is forwarded without a synthetic release frame, allowing the ROM's directional latch to enforce one hop per press. Stick hysteresis prevents axis jitter from looking like a release and re-press.

Modern mode also bypasses the premature home scan at **0x1cff** for non-upward movement on the last log row (48). Occupied-home early returns otherwise starve left/right/down input until log drift carries the frog out of the bay's X band. Upward attempts and actual home-entry rows still follow the original routines; Classic retains the original stall. The [top-log input audit](TOP-LOG-INPUT.md) documents the stationary-log regression tests and independent MAME reproduction.

Both CPUs execute native code. `NativeSound.cs` models the AY tone/noise/envelope output and DC filtering. The analogue amplifier/filter network is **not** MAME netlist exact. Recorded MAME WAVs in the ignored local audio folder were research artifacts; the game has no dependency on them.

## Reproduction

```powershell
./tools/setup.ps1
./tools/verify.ps1
./tools/decompile.ps1
./BuildGame.ps1
```

Ghidra projects and original images remain local under ignored `reference/`. The exported C, inventories, function map, generator, fixtures and verification reports remain reviewable in the repository. Vendor hashes are checked so silent edits to the reference cannot make both sides agree on a new mistake.

### River crocodile correction

MAME captures from `tools/mame_river.lua` verify three executions of **0x28bb** with the lane-table position at 160: X=110 leaves the frog alive, X=130 sets death without drowning, and X=152 sets death with drowning. All three match native RAM and registers. Run the script with the same MAME command above, replacing the autoboot script path, to regenerate the `28bb-*` fixtures.

The former "ride/hold" interpretation was incorrect: **0x8004** starts the death sequence at **0x16f8**, including the branch that stamps tiles 0x68..0x6b. The full lethal head interval is table-position minus 39 through table-position, not just the final 16 pixels. The crocodile tile strip at **0x1413** begins with 16 blank pixels. Its occupied graphic is 47 pixels long, with its leading edge 25 pixels behind the table position. The model matches those bounds with 15 pixels of jaw and 32 pixels of back/tail; its back ends at table-position minus 40. The lethal interval extends ahead of the visible jaw, so fitting the jaw to the whole collision interval would make it too long. The generated ROM instructions and vendored oracle are unchanged.

The incoming and outgoing copies of slot zero retain separate visual identities when **0x8150** changes at the **0x8101** wrap. Native regressions jump from turtles onto a naturally spawned crocodile, ride it for 100 frames and hop again in both modes. Godot presentation tests exercise both croc-to-log and log-to-croc transitions while opposite edge copies are visible.

The river model length is checked against the occupied pixels decoded by `python tools/inspect_graphics.py`. Both crocodile variants have upper and lower tooth rows. Their lower mouth lining follows the jaw surface, avoiding the former intersecting ellipsoid; generated jaw surfaces and lining have outward/upward normal assertions. The rebuilt assets were checked in Godot captures.
