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
| MAME function fixtures | Before/after snapshots of ROM 0x11bf, 0x14b7, 0x08e0, 0x1cff, 0x16f8; native execution starts from MAME's captured entry registers and RAM | Five functions match all 3,328 state bytes and all captured registers |
| Recovered-reference suite | Function equivalence, mutation controls, board hardware and new provenance checks | 439 pass, 0 fail; one optional recorded-audio-file check skipped |
| Lifecycle scenarios | 9,000-frame idle run through timer deaths and game-over; two-player hand-off; five forced safe home entries followed by ordinary board progression | Passed; next board reached |
| Native sound | Original sound CPU and commands running through the compiled program | 1,425,600 samples in the 1,800-frame test; non-silent output; full command sweep expands executed-code coverage |
| Blender exports | Finite geometry, topology counts, bound vertices, normalized skin weights, exact bone counts and moving animation channels | 13 assets pass |

MAME fixtures were captured with MAME 0.289, whose downloaded binary's SHA-256 matched its official release checksums. `tools/mame_functions.lua` installs opcode-read taps gated by CURPC, captures real entry registers/RAM, and waits for the actual return PC and stack depth to capture the exit. The tape uses real coin/start controls plus explicit home/death fixtures to reach the selected branches. These fixture pokes are test setup, not normal game behavior.

Re-capture fixtures, if desired:

```powershell
$env:FROGGER_EVIDENCE = "$PWD/docs/evidence/mame-functions"
./reference/mame/mame.exe frogger -rompath reference -video none -sound none -nothrottle -seconds_to_run 12 -skip_gameinfo -nocheat -noautosave -nonvram_save -cfg_directory reference/mame-test-config -nvram_directory reference/mame-test-config -autoboot_script tools/mame_functions.lua
./tools/verify.ps1
```

The equivalence gate has limits: 460 frames and five independent function fixtures are concrete evidence, not an exhaustive proof over every possible machine state. Ghidra producing C is a weaker, separate fact. `docs/evidence/audit.json` records these distinctions.

## Presentation and intentional differences

The game uses original object lists and sprite/VRAM state to place Blender models. Turtle submergence and home creatures are read from the original tile state, not independent random timers. A framebuffer is never drawn as the game world. The bank at row 0 remains open; grass is flat surface detail.

The optional forgiving road hook substitutes a swept frog-center/vehicle-box test at **0x11bf** and returns through the original stack. It retains the original kill latch; river support, diving, hazards, goals, RNG, scores, timers and progression are untouched by this hook. Disable it for reference comparisons. The hook intentionally changes road collision decisions and their instruction timing.

Both CPUs execute native code. `NativeSound.cs` models the AY tone/noise/envelope output and DC filtering. The analogue amplifier/filter network is **not** MAME netlist exact. Recorded MAME WAVs in the ignored local audio folder were research artifacts; the game has no dependency on them.

## Reproduction

```powershell
./tools/setup.ps1
./tools/verify.ps1
./tools/decompile.ps1
./BuildGame.ps1
```

Ghidra projects and original images remain local under ignored `reference/`. The exported C, inventories, function map, generator, fixtures and verification reports remain reviewable in the repository. Vendor hashes are checked so silent edits to the reference cannot make both sides agree on a new mistake.
