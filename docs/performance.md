# Gameplay hitch audit

The September 2026 audit removed recurring presentation work and moved bounded
actor creation into loading. Recovered CPU instruction bodies, arcade timing,
collision rules, and sound synthesis are unchanged.

- Actors retain visibility and animation processing between frames. Only actors
  entering or leaving the visible set change state.
- Hidden shader-warmup models stop processing after warmup; both warmup variants
  are excluded from shadow casting.
- Turtle rigs are prepared for the ROM's fixed slots and reused across board
  wraps. Both frog colors, passengers, home animals, hazards, and the death
  ripple are prepared before gameplay. Departing beavers reuse a small pool.
  This moves allocations into startup and retains a bounded set of inactive rigs.
- Motion histories use fixed circular buffers. Turtle support records, animation
  name lookups, and snake bone indices are reused instead of recreated each frame.
- Message text is measured only when its text, font, size, or available width
  changes. The FPS overlay and browser layout diagnostics refresh four times per
  second. Its former `GPU` estimate is labeled `Other`, since frame time minus
  script time includes engine work, scheduling, and vsync.
- New high scores are coalesced into one-second checkpoints, with immediate
  flushing on focus loss, shutdown, and restart, and next-frame flushing on
  pause, menu, or game over. Settings changes still save immediately. A forced
  termination before a checkpoint can lose up to one second of new best-score
  progress; normal lifecycle exits flush the pending record.
- Browser PCM decoding reuses a 2,400-sample scratch buffer and copies only the
  newest 50 ms on overflow. The existing bounded output scheduling remains intact.

## Reproducing the checks

Run `tools/verify.ps1` for native/reference and MAME parity, browser audio/SDK
tests, presentation, actor-reuse regressions, camera, mobile UI, and audio output
queue checks. `tests/hitch_regression_test.gd` checks stable visibility, turtle
wrap reuse, player-color reuse, beaver pooling, warmup suspension, and score
checkpoint behavior.

Run the diagnostic CPU probe from PowerShell:

```powershell
. ./tools/common.ps1
& (Get-FroggerGodot) --headless --path godot --script ../tests/frame_probe.gd
```

The same 1,800-frame local workload produced:

| Measurement | Before | After |
| --- | ---: | ---: |
| Median presentation CPU | 400 us | 308 us |
| 95th percentile | 486 us | 403 us |
| 99th percentile | 913 us | 652 us |
| Observed visibility changes after frame 120 | 65,560 | 147 |

These are diagnostic samples, not performance thresholds. The probe times frog
motion, actor updates, camera, and HUD; it excludes native simulation, rendering,
and browser scheduling. It does not measure end-to-end frame pacing or prove
that every intermittent hitch is resolved.

Both Web and CrazyGames exports build successfully. Native later-level captures
and a local browser gameplay smoke check were inspected. Actual hosted
CrazyGames sessions and lower-powered/mobile devices still need frame-time
testing.
