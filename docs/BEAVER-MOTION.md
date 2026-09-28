# Beaver movement verification

The remaining teleport was in presentation. At native retirement or contact,
`update_beaver()` moved the model directly to the nearest rendered log cap.
Live native traces showed retirement jumps of roughly 9–19 pixels. The exit now
retains its initial offset and follows only subsequent log movement. Offscreen
descriptors no longer flash a model or leave a ghost that drifts onto the board.
Native slot lifetimes are observed on every simulation step to handle a clear
and replacement between rendered frames.

The model's cap alignment is calculated once per spawn from the ROM destination
(`record +0/+1`, relative to the lane source at `0x8000 | record[+11]`). This
constant visual offset keeps the larger model outside the log at arrival while
preserving the sprite's speed, including its one-pixel relative swim step every
eight simulation frames. There is no moving-log clamp during the swim.

The instruction port was checked independently against MAME 0.289 executing
the verified original ROM. **2,000 consecutive executions of dispatcher
0x2b83 matched all 6,656,000 RAM/VRAM/object-RAM bytes and all seven captured
registers per execution.** These calls cover idle, spawning, waiting, relative
swim steps, byte wrapping and retirement in both directions. The test stages
level 3, holds the frog on the bank and refreshes its timer; it does not cover
the player-hit branches or establish correctness for every possible state.
Regenerating `src/Generated/` left the instruction bodies unchanged.

`docs/evidence/beaver-native-replay.json` records the branch counts and retained
examples. Eleven MAME branch fixtures join the previous eight in the normal
native host verification. The full local capture can be reproduced with:

```powershell
New-Item -ItemType Directory -Force scratch/beaver-mame
$env:FROGGER_EVIDENCE = "$PWD/scratch/beaver-mame"
./reference/mame/mame.exe frogger -rompath reference -video none -sound none -nothrottle -seconds_to_run 45 -skip_gameinfo -nocheat -noautosave -nonvram_save -cfg_directory reference/mame-test-config -nvram_directory reference/mame-test-config -autoboot_script tools/mame_beaver.lua
./tools/verify.ps1
./tests/Host/test_host.exe --mame-fixtures scratch/beaver-mame
python tools/select_beaver_fixtures.py scratch/beaver-mame
```

`tests/beaver_motion_test.gd` runs 12,000 real native simulation frames, with
rendering every one or three steps. It checks visible retirement continuity,
offscreen sprite handling and slot reuse between renders. Restoring the old
log-cap snap makes this regression test fail. No simulation speed, spawning,
collision or generated instruction changes are part of this fix.
