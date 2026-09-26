# Web audio

Desktop Web exports retain Godot's threaded audio driver. On iPhone/iPad the page
instead plays the original sound CPU's 48 kHz mono PCM through Web Audio buffer
sources. This is an output-transport mitigation for the reported roughly 600 ms
delay on built-in iOS speakers; it is **not a confirmed diagnosis of the device's
delay**, and needs an audible test on that device.

The native sound CPU and its timing are unchanged. The original native queue is
capped at 2,400 samples (50 ms), and the generator and exported worklet have finite
ring buffers. The exported threaded worklet uses shared-memory Atomics, not an
unbounded queue of posted PCM chunks. Those queues do not by themselves account
for the reported delay. The actual exported Godot 4.7 resume function already
accepts any context state other than `running`, including Safari's `interrupted`
state. An interrupted-state omission is therefore not the diagnosis either.

The iOS path removes Godot's mixer thread and streaming AudioWorklet from the
audible signal path. It schedules at most 75 ms of PCM on the browser clock, drains
the producer each frame, and retains the newest 50 ms after a catch-up. It creates
the output context during the first page gesture and uses the device's default
rate, with 48 kHz declared on each source buffer. It releases output and queued
sources on mute, pause, page hiding, and navigation. Resume starts with fresh PCM.
No ROM sound is replaced with a recording. Other platforms and exports without
the helper fall back to Godot's generator.

The helper is tracked at `tools/web_audio.js` and copied/injected by `BuildWeb.ps1`.
It does not patch the engine or vendored JavaScript. iPadOS desktop-style user
agents are detected by `MacIntel` plus touch support. For device comparisons:

- Normal URL: browser PCM on iOS, threaded Godot elsewhere.
- `?audio=godot`: original Godot transport on every platform.
- `?audio=browser`: browser PCM on every platform.
- Add `&audio_debug=1` to either explicit URL to log the actual Godot driver name
  (`AudioWorklet` or `ScriptProcessor`) and isolation/shared-memory availability
  at startup, plus browser-PCM statistics every five seconds while playing.
  In developer tools,
  `FroggerAudio.diagnostics()` returns the same data; `FroggerAudio.reset()`
  releases and recreates its output without restarting gameplay.

`scheduledMs` measures only our scheduled sources, not acoustic speaker latency.
`clockDriftMs` compares a four-second context-clock interval against wall time.
`baseLatency`, `outputLatency`, and output timestamps are supplied by the browser
when available. They are diagnostics; no timestamp subtraction is treated as an
accurate speaker-latency measurement or used to trigger automatic resets.

`node --test tests/web_audio.test.js` verifies platform selection, first-gesture
startup, 44.1 kHz device/48 kHz source separation, stalls, catch-up, lifecycle, and
ten simulated minutes at 60 FPS. The native `tests/audio_feed.gd` checks the Godot
fallback's queue draining and mute/pause/menu gates. Neither test establishes
actual iOS speaker latency. Compare both URL modes with the same hop/death events
on the affected device, including after background/foreground and mute/unmute.

References checked during investigation:

- [Godot 4.7 web driver](https://github.com/godotengine/godot/blob/4.7-stable/platform/web/audio_driver_web.cpp)
- [Godot 4.7 browser audio](https://github.com/godotengine/godot/blob/4.7-stable/platform/web/js/libs/library_godot_audio.js)
- [Web Audio specification](https://www.w3.org/TR/webaudio-1.0/)
- [Historical WebKit context/device-clock regression](https://bugs.webkit.org/show_bug.cgi?id=232728)
  demonstrates that browser output can drift independently of game rendering;
  that old, fixed bug is not evidence that this device has the same regression.
