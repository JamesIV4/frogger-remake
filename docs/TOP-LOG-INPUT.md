# Top-log input stall

The remaining top-row hitch was input starvation in the original ROM's home
scan, reproduced with stationary logs. It was not an expensive home-search loop:
native simulation steps in the diagnostic run were approximately 60 microseconds
even while the frog was unable to move.

At **0x1a8f**, the ROM sends rows below 49 to the home scan at **0x1cff**.
That includes the final log row, **48**. If X lies within an occupied home's
eight-pixel band, the handler returns at **0x1d91 + bay * 0x51** before checking
the actual home-entry boundary (**row < 42**) and before reaching input handling
at **0x1acb**. This prevents starting or continuing left, right, and down hops.
Moving logs eventually carry the frog outside the band, making the pause appear
intermittent. A stationary log exposes a persistent stall.

Modern mode now routes that premature scan directly to the original input
handler when the frog is on row 48 and is not attempting an upward hop. An
already-running left/right/down hop continues even if the player releases the
button or presses Up before it finishes. Upward attempts, all other rows, home
awards, and lethal home-entry decisions remain on their original paths.

The adjustment lives in `ArcadeSimulationCore::Step`, outside generated
instructions, under the existing Modern option. **Classic collision retains the
original ROM behavior, including this stall.** ROM files, generated instruction
bodies, and the vendored oracle are unchanged.

## Verification

- `tests/Host/test_host.cpp` checks 240 stationary cases: both players, all five
  homes, every X in each home band, and left/right/down. Filled-home movement,
  death state, and directional counters must match the same empty-home setup.
- Additional cases exercise a sideways hop entering the occupied band after
  input release or an early Up press. Upward runs are compared with Classic at
  rows 48, 46, 42, 40, and 32, for both empty and occupied destinations.
- MAME executing the original ROM reproduced ten occupied-home early returns
  (five homes for each player), never visiting the input handler and leaving an
  in-flight hop counter unchanged. Native replay matched all **33,280 captured
  RAM/VRAM/object-RAM bytes and 70 captured registers**.
- `tests/top_row_probe.gd` reports coordinates and per-step CPU timing, with
  sound enabled. Its optional rendered capture uses the live game presentation
  and a normal simulated left-button press, without desktop input. The Modern
  frog moves from X=120 to X=104; Classic remains at X=120, both on row 48.

Run the diagnostic from PowerShell:

```powershell
. ./tools/common.ps1
& (Get-FroggerGodot) --headless --path godot --script ../tests/top_row_probe.gd
& (Get-FroggerGodot) --headless --path godot --script ../tests/top_row_probe.gd -- --classic
./tools/verify.ps1
```

For a bounded rendered capture, omit `--headless` and add
`-- --screenshot=<absolute PNG path>` (optionally followed by `--classic`).
The fixture sets the top-lane speed/phase bytes `0x819b`/`0x81a6` to zero;
it does not repeatedly pin the frog's position or patch instructions.

To reproduce the independent MAME check:

```powershell
New-Item -ItemType Directory -Force scratch/top-row-mame
$env:FROGGER_EVIDENCE = "$PWD/scratch/top-row-mame"
./reference/mame/mame.exe frogger -rompath reference -video none -sound none -nothrottle -seconds_to_run 15 -skip_gameinfo -nocheat -noautosave -nonvram_save -cfg_directory reference/mame-test-config -nvram_directory reference/mame-test-config -autoboot_script tools/mame_top_row.lua
./tests/Host/test_host.exe --mame-fixtures scratch/top-row-mame
```
