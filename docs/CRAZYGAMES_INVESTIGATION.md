# CrazyGames: non-threaded web audio investigation

**Follow-up:** The user confirmed the prototype works well and requested this
configuration as the default for all web builds. `BuildWeb.ps1` now implements
that configuration; see [Web audio](WEB_AUDIO.md). The notes below describe the
original investigation and its measurement limits.

Investigated 2026-09-28 against commit `f4e0e8b`, the installed Godot
4.7 Standard templates, and Emscripten 4.0.20.

## Finding

The game can run with its existing C++ GDExtension without cross-origin
isolation or SharedArrayBuffer. An isolated local prototype successfully loaded
the simulation, rendered the menu, entered gameplay, and submitted non-silent
ROM PCM to the existing browser audio transport. It also loaded inside an
ordinary cross-origin iframe without isolation headers.

This establishes runtime compatibility, **not audio performance parity**.
The unattended Edge session rendered at approximately 1 FPS between brief
bursts of normal frames. Audio submissions therefore ran dry. Its timing data
cannot establish foreground frame rate, crackle-free playback, or speaker
latency. No CrazyGames upload or device listening test was performed.

## Why disabling threads alone was a poor audio option

The default desktop path is:

`simulation on game frame -> AudioStreamGenerator -> Godot mixer -> AudioWorklet`

Godot's threaded driver refills its shared ring buffer from a mixer thread.
The non-threaded driver instead handles worklet refill messages on the main
thread. The 4.7 source shows `start_no_threads`, `out_callback`, and
`port.postMessage` in this path. That makes render stalls relevant to mixer
refills and is consistent with the user's previous performance observations;
it does not prove their precise cause.

The project already has a second path, normally selected on iOS:

`simulation on game frame -> mono PCM bridge -> scheduled Web Audio buffers`

This selects Godot's `Dummy` driver before startup. Web Audio renders scheduled
sources on the browser's audio rendering thread without requiring application
pthreads, Atomics, or SharedArrayBuffer. It preserves the recovered sound CPU
and generates live PCM; it does not substitute recorded sound effects.

The existing helper schedules with a 20 ms lead when it must restart its
timeline, limits scheduled PCM to 75 ms, and retains at most 50 ms of catch-up
samples. These are software queue settings, **not speaker latency guarantees**.
The 48 kHz mono float stream carries about 192 KB/s before base64 encoding
(256 KB/s after encoding). Both transports still depend on the game frame to
produce new ROM audio. A long main-thread stall can exhaust either producer;
moving output into an AudioWorklet alone would not fix that.

## Prototype and checks

The ignored folder `builds/crazygames-investigation/` contains an experimental
build. The regular `builds/web/` export and tracked production configuration
were not changed.

- Rebuilt godot-cpp with `threads=no`; its `.nothreads.a` filename keeps the
  existing threaded library separate.
- Rebuilt the game side module without `-pthread`, retaining `SIDE_MODULE=1`,
  `WASM_BIGINT`, and `SUPPORT_LONGJMP=wasm`.
- Used the installed `web_dlink_nothreads_release.zip`, with the existing game
  pack and a matching non-threaded startup configuration. No custom engine was
  needed. The current extension descriptor already has `nothreads` entries.
- Enabled the existing browser PCM path by default only in this prototype.
  `?audio=godot` selects the non-threaded Godot mixer for comparison.
- Added an investigation-only overlay for feature availability, frame
  intervals, PCM submission cost, queue depletion, and browser audio state.
  These extra counters are not in the production helper.
- Served using ordinary Python HTTP, with no COOP/COEP headers or service
  worker. Tested a top-level page and a cross-origin iframe (127.0.0.1 parent,
  localhost child).
- Startup reported Godot 4.7, Emscripten 4.0.20, single-threaded, GDExtension
  support; `crossOriginIsolated=false`, `SharedArrayBuffer` unavailable, and
  audio driver `Dummy`.
- Gameplay submitted non-silent PCM to a running 48 kHz AudioContext. An early
  sample of 24 submissions averaged about 0.16 ms inside the JS `push_pcm`
  function, with a 0.4 ms maximum. This excludes simulation/GDScript/base64
  encoding cost and was not a representative foreground performance benchmark.
- All 13 existing `tests/web_audio.test.js` tests passed. They use mocks and
  establish scheduling/lifecycle behavior, not real device sound quality.

The initial prototype construction helpers and compiler logs are retained in
ignored `scratch/prepare-nothreads-investigation.py`,
`scratch/compile-nothreads-investigation.ps1`, and `scratch/nothreads-*.log`.
The generated prototype also includes later diagnostic additions; these
scratch helpers are investigation artifacts, not a supported release pipeline.

## Listening comparison

Serve the prototype without isolation headers:

```powershell
python -m http.server 8088 --bind 127.0.0.1 --directory builds/crazygames-investigation
```

Open `http://localhost:8088/index.html?audio=browser&audio_debug=1` in a
foreground browser. Collapse the diagnostic overlay to see the whole board.
Compare against both:

1. The same prototype with `?audio=godot&audio_debug=1`.
2. The normal threaded export using `RunWeb.ps1`, first normally, then with
   `?audio=browser&audio_debug=1` to isolate the output-transport change.

Compare hop/death sound timing, music continuity, and movement responsiveness
during ordinary play, effect-heavy play, pause/resume, and tab switching.
The overlay's deliberate 100 ms stall button demonstrates queue exhaustion;
it is a stress case, not a normal-play acceptance criterion. Test the actual
CrazyGames preview and target phones before promoting the prototype.

Recommended production direction, if those comparisons pass: a separate
CrazyGames export preset with thread support off, GDExtension support on,
matching non-threaded C++ binaries, and browser PCM selected before startup.
Keep the working threaded export as the comparison baseline. Do not reuse a
threaded side module in the non-threaded engine.

## Sources

- [Godot 4.7 web exporter](https://github.com/godotengine/godot/blob/4.7-stable/platform/web/export/export_plugin.cpp)
- [Godot 4.7 web audio driver](https://github.com/godotengine/godot/blob/4.7-stable/platform/web/audio_driver_web.cpp)
- [Godot 4.7 browser audio bridge](https://github.com/godotengine/godot/blob/4.7-stable/platform/web/js/libs/library_godot_audio.js)
- [Existing project audio investigation](WEB_AUDIO.md)

The general Godot web documentation's statement that extension support also
requires isolation does not describe the behavior observed with these installed
4.7 non-threaded GDExtension templates. Actual runtime testing supersedes that
earlier assumption for this checkout.
