# Web audio

All Web exports use non-threaded Godot and play the original sound CPU's 48 kHz
mono PCM through Web Audio buffer sources. The game and its C++ GDExtension need
neither SharedArrayBuffer nor cross-origin isolation headers. The browser still
renders scheduled audio on its audio rendering thread. The non-threaded
prototype was tested successfully by the user and promoted to the standard web
build; see [the investigation](CRAZYGAMES_INVESTIGATION.md).

This transport originated in an investigation of a reported delay of over one
second on iOS 27 Home Screen speakers. The lifecycle changes described below
are not a confirmed root-cause diagnosis of that earlier device report.

The native sound CPU and its timing are unchanged. The native queue is capped
at 2,400 samples (50 ms). In the previous threaded build, the Godot worklet used
a finite shared-memory ring buffer, not an unbounded queue of posted PCM chunks.
Those queues do not by themselves account
for the reported delay. The actual exported Godot 4.7 resume function already
accepts any context state other than `running`, including Safari's `interrupted`
state. An interrupted-state omission is therefore not the diagnosis either.

The browser PCM path starts Godot with its supported `--audio-driver Dummy` argument,
before engine initialization. Previously, skipping the GDScript generator still
left Godot's silent AudioContext/worklet running alongside the helper's context.
Now the helper owns the sole browser output. `?audio=godot` selects Godot's
non-threaded driver for diagnosis. Browser buffer-source rendering runs in the browser's
audio engine; the main game thread submits PCM each rendered frame.

It schedules at most 75 ms of PCM on the browser clock, drains
the producer each frame, and retains the newest 50 ms after a catch-up. It creates
the output context during the first page gesture and uses the device's default
rate, with 48 kHz declared on each source buffer. Pause, mute and page hiding
discard pending sources and suspend the same context; resume starts with fresh
PCM. Only explicit reset/navigation closes it. This avoids repeatedly creating
new contexts while old asynchronous device shutdowns may still be pending.

On iOS, feature-detected `navigator.audioSession.type = 'ambient'` explicitly
requests game semantics: mix with other apps, obey the silent switch, and avoid
Now Playing eligibility. Unsupported setters do not prevent sound. This category
is separate from the context's `latencyHint: 'interactive'`; WebKit's latencyHint
implementation remains an open issue. Pure Web Audio normally defaults to
ambient already, so this is not proof that playback classification caused lag.
No ROM sound is replaced with a recording. Other platforms and exports without
the helper use Godot's generator. If context creation fails after Dummy selection,
the helper retries on the next gesture and records the error. Reload with
`?audio=godot` for the engine fallback; it cannot switch the startup driver live.

The helper is tracked at `tools/web_audio.js` and copied/injected by `BuildWeb.ps1`.
It does not patch the engine or vendored JavaScript. iPadOS desktop-style user
agents are detected by `MacIntel` plus touch support. For device comparisons:

- Normal URL: browser PCM on every platform.
- `?audio=godot`: non-threaded Godot transport on every platform.
- `?audio=browser`: explicitly selects the default browser PCM transport.
- Add `&audio_debug=1` to either explicit URL to log the actual Godot driver name
  (`Dummy` for exclusive browser output, otherwise `AudioWorklet` or
  `ScriptProcessor`) and isolation/shared-memory availability
  at startup, plus browser-PCM statistics every five seconds while playing.
  In developer tools,
  `FroggerAudio.diagnostics()` returns the same data; `FroggerAudio.reset()`
  releases and recreates its output without restarting gameplay.
  Diagnostics include revision `single-context-1`, session type/state and
  Home Screen display mode so device reports can identify the running build.

`scheduledMs` measures only our scheduled sources, not acoustic speaker latency.
`clockDriftMs` compares a four-second context-clock interval against wall time.
`baseLatency`, `outputLatency`, and output timestamps are supplied by the browser
when available. They are diagnostics; no timestamp subtraction is treated as an
accurate speaker-latency measurement or used to trigger automatic resets.

`node --test tests/web_audio.test.js` verifies platform selection, first-gesture
startup, exclusive output selection, ambient-session support/failure, async
pause/resume races, 44.1 kHz device/48 kHz source separation, stalls, lifecycle, and
ten simulated minutes at 60 FPS. The native `tests/audio_feed.gd` checks the Godot
fallback's queue draining and mute/pause/menu gates. Neither test establishes
actual iOS speaker latency. Compare both URL modes with the same hop/death events
on the affected device, including after background/foreground and mute/unmute.

`BuildWeb.ps1` builds godot-cpp with `threads=no`, compiles the side module
without `-pthread`, and uses Godot's non-threaded GDExtension export template.
A build stamp forces existing threaded game binaries to be rebuilt on upgrade.
The `.gdextension` descriptor only advertises the web binaries for `nothreads`.
The local server deliberately omits isolation headers so local testing covers
ordinary hosting. Use HTTP on localhost or HTTPS when hosted; opening the HTML
directly through `file://` still does not work.

References checked during investigation:

- [Godot 4.7 web driver](https://github.com/godotengine/godot/blob/4.7-stable/platform/web/audio_driver_web.cpp)
- [Godot 4.7 browser audio](https://github.com/godotengine/godot/blob/4.7-stable/platform/web/js/libs/library_godot_audio.js)
- [Web Audio specification](https://www.w3.org/TR/webaudio-1.0/)
- [Apple game-audio guidelines](https://developer.apple.com/library/archive/documentation/Audio/Conceptual/AudioSessionProgrammingGuide/AudioGuidelinesByAppType/AudioGuidelinesByAppType.html)
- [WebKit latencyHint implementation issue](https://bugs.webkit.org/show_bug.cgi?id=214258)
- [WebKit AudioContext and Now Playing eligibility](https://github.com/WebKit/WebKit/blob/main/Source/WebCore/Modules/webaudio/AudioContext.cpp)
- [WebKit session category and Web Audio buffer selection](https://github.com/WebKit/WebKit/blob/main/Source/WebCore/platform/audio/cocoa/MediaSessionManagerCocoa.mm)
- [Historical WebKit context/device-clock regression](https://bugs.webkit.org/show_bug.cgi?id=232728)
  demonstrates that browser output can drift independently of game rendering;
  that old, fixed bug is not evidence that this device has the same regression.
