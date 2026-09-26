/* Browser-native output for the ROM's 48 kHz mono PCM on iOS WebKit.
 * The threaded Godot output remains the default everywhere else. This bypasses
 * its streaming mixer/worklet; it does not change the recovered sound CPU.
 * ?audio=godot disables this path; ?audio=browser enables it for comparison.
 */
(function (root) {
    'use strict';
    const RATE = 48000;
    const MAX_SECONDS = 0.075;
    const LEAD_SECONDS = 0.020;
    const mode = new URLSearchParams(root.location.search).get('audio');
    const debug = new URLSearchParams(root.location.search).has('audio_debug');
    const ios = /iPad|iPhone|iPod/.test(root.navigator.userAgent) ||
        (root.navigator.platform === 'MacIntel' && root.navigator.maxTouchPoints > 1);
    const Context = root.AudioContext || root.webkitAudioContext;
    const enabled = Boolean(Context) && mode !== 'godot' && (ios || mode === 'browser');
    let context = null;
    let gain = null;
    let active = false;
    let nextTime = 0;
    let unlocked = false;
    let resumePending = false;
    let clockAnchor = null;
    let lastReport = -Infinity;
    const sources = new Set();
    const stats = { resets: 0, droppedFrames: 0, submittedFrames: 0, clockDriftMs: 0, error: '' };

    function clear() {
        for (const source of sources) {
            source.onended = null;
            source.stop();
            source.disconnect();
        }
        sources.clear();
        nextTime = 0;
        clockAnchor = null;
    }

    function resume() {
        if (!context || context.state === 'running' || context.state === 'closed' || resumePending) return;
        // Safari also has an 'interrupted' state. Try resume inside the original
        // gesture, never await before making this call.
        resumePending = true;
        const current = context;
        current.resume().catch(error => { stats.error = String(error); }).finally(() => {
            if (context === current) resumePending = false;
        });
    }

    function create() {
        if (context || !root.FroggerAudio.enabled || root.document.hidden) return;
        try {
            // Let the device choose its rate; the AudioBuffer declares 48 kHz
            // explicitly so Web Audio resamples it without changing ROM timing.
            context = new Context({ latencyHint: 'interactive' });
            gain = context.createGain();
            gain.gain.value = Math.pow(10, -9 / 20);
            gain.connect(context.destination);
            const current = context;
            current.addEventListener('statechange', () => {
                if (context === current && current.state !== 'running') clear();
            });
            resume();
        } catch (error) {
            stats.error = String(error);
            close();
            root.FroggerAudio.enabled = false; // Let the game fall back to Godot.
        }
    }

    function close() {
        clear();
        const previous = context;
        context = null;
        gain = null;
        resumePending = false;
        if (previous && previous.state !== 'closed') previous.close().catch(() => {});
    }

    function gesture() {
        if (!root.FroggerAudio.enabled) return;
        unlocked = true;
        create();
        // A prior autoplay resume promise may still be waiting for activation.
        resumePending = false;
        resume();
    }

    root.FroggerAudio = {
        enabled,
        report_driver(name) {
            this.godotDriver = name;
            if (debug) root.console.info('Frogger audio transport', JSON.stringify({
                browserPCM: this.enabled, godotDriver: name,
                crossOriginIsolated: root.crossOriginIsolated,
                sharedArrayBuffer: typeof root.SharedArrayBuffer !== 'undefined'
            }));
        },
        set_active(value) {
            const wasActive = active;
            active = Boolean(value);
            // Muting/pausing releases the browser output too. The next gesture
            // reopens it, so output-device stalls do not survive a mute toggle.
            if (!active && wasActive) close();
            if (active && unlocked) {
                create();
                resume();
            }
        },
        push_pcm(encoded) {
            if (!this.enabled || !active || !context || context.state !== 'running' || root.document.hidden) return;
            const binary = root.atob(encoded);
            const bytes = new Uint8Array(binary.length);
            for (let i = 0; i < binary.length; ++i) bytes[i] = binary.charCodeAt(i);
            if (bytes.length % 4) return;
            let pcm = new Float32Array(bytes.buffer);
            // No pending PCM exists outside the source nodes. Catch-up frames
            // retain the newest 50 ms; a blocked device cannot build a backlog.
            if (pcm.length > RATE * 0.05) {
                stats.droppedFrames += pcm.length - RATE * 0.05;
                pcm = pcm.subarray(pcm.length - RATE * 0.05);
            }
            if (!pcm.length) return;
            const now = context.currentTime;
            const duration = pcm.length / RATE;
            if (nextTime - now + duration > MAX_SECONDS) {
                clear();
                ++stats.resets;
            }
            if (nextTime < now + 0.003) nextTime = now + LEAD_SECONDS;
            // Ended events can be delayed while the main thread is busy.
            for (const source of sources) {
                if (source.froggerEnd <= now) {
                    source.onended = null;
                    source.disconnect();
                    sources.delete(source);
                }
            }
            const buffer = context.createBuffer(1, pcm.length, RATE);
            buffer.copyToChannel(pcm, 0);
            const source = context.createBufferSource();
            source.buffer = buffer;
            source.connect(gain);
            source.onended = () => { sources.delete(source); source.disconnect(); };
            source.froggerEnd = nextTime + duration;
            sources.add(source);
            source.start(nextTime);
            nextTime += duration;
            stats.submittedFrames += pcm.length;
            const wall = root.performance.now();
            if (!clockAnchor) clockAnchor = { wall, audio: now };
            if (wall - clockAnchor.wall >= 4000) {
                stats.clockDriftMs = (now - clockAnchor.audio) * 1000 - (wall - clockAnchor.wall);
                clockAnchor = { wall, audio: now };
            }
            if (debug && wall - lastReport >= 5000) {
                root.console.info('Frogger browser audio', JSON.stringify(this.diagnostics()));
                lastReport = wall;
            }
        },
        // Exposed for device diagnosis and a gesture-driven recovery without
        // reloading gameplay. Timestamp differences are diagnostics, not a
        // purported measurement of speaker latency or an automatic reset rule.
        reset() { close(); ++stats.resets; create(); resume(); },
        diagnostics() {
            const stamp = context && context.getOutputTimestamp ? context.getOutputTimestamp() : null;
            return { ...stats, enabled: this.enabled, godotDriver: this.godotDriver, active, state: context ? context.state : 'not-created',
                sampleRate: context ? context.sampleRate : null,
                scheduledMs: context ? Math.max(0, nextTime - context.currentTime) * 1000 : 0,
                sources: sources.size, baseLatency: context ? context.baseLatency : null,
                outputLatency: context ? context.outputLatency : null, outputTimestamp: stamp };
        }
    };
    if (enabled) {
        // Installed by the page before Godot starts, so Play's first tap unlocks
        // audio even though the game processes that tap on the following frame.
        for (const name of ['pointerdown', 'touchend', 'keydown']) {
            root.addEventListener(name, gesture, { capture: true, passive: true });
        }
        root.document.addEventListener('visibilitychange', () => {
            if (root.document.hidden) close();
            else if (unlocked) { create(); resume(); }
        });
        root.addEventListener('pagehide', close);
        root.addEventListener('pageshow', () => { if (unlocked) { create(); resume(); } });
    }
})(window);
