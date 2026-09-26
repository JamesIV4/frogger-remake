import { test } from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';
const script = fs.readFileSync('tools/web_audio.js', 'utf8');

function setup({ search = '', platform = 'iPhone', userAgent = 'iPhone', maxTouchPoints = 5, rate = 44100, failCreate = false } = {}) {
    const listeners = {};
    const contexts = [];
    let wall = 0;
    const root = {
        location: { search }, navigator: { userAgent, platform, maxTouchPoints },
        document: { hidden: false, addEventListener: (name, callback) => { listeners[name] = callback; } },
        addEventListener: (name, callback) => { listeners[name] = callback; },
        performance: { now: () => wall },
        atob: value => Buffer.from(value, 'base64').toString('binary'),
    };
    root.AudioContext = class {
        constructor(options) {
            if (failCreate) throw new Error('Output unavailable');
            this.options = options;
            this.currentTime = 0;
            this.sampleRate = rate;
            this.state = 'suspended';
            this.destination = {};
            this.nodes = [];
            contexts.push(this);
        }
        addEventListener(_name, callback) { this.onstate = callback; }
        resume() { this.state = 'running'; return Promise.resolve(); }
        suspend() { this.state = 'suspended'; return Promise.resolve(); }
        close() { this.state = 'closed'; return Promise.resolve(); }
        createGain() { return { gain: {}, connect() {} }; }
        createBuffer(channels, length, sampleRate) {
            return { channels, length, sampleRate, copyToChannel(samples) { this.samples = Float32Array.from(samples); } };
        }
        createBufferSource() {
            const node = { stopped: false, disconnected: false, connect() {},
                start(time) { this.startTime = time; }, stop() { this.stopped = true; },
                disconnect() { this.disconnected = true; } };
            this.nodes.push(node);
            return node;
        }
    };
    vm.runInNewContext(script, { window: root, URLSearchParams, Uint8Array, Float32Array });
    return { root, api: root.FroggerAudio, contexts,
        event: name => listeners[name] && listeners[name](),
        advance(seconds) { wall += seconds * 1000; contexts.at(-1).currentTime += seconds; } };
}
function pcm(length = 792) {
    const samples = Float32Array.from({ length }, (_, i) => i / length);
    return Buffer.from(samples.buffer).toString('base64');
}

test('only iOS uses native browser transport by default; explicit A/B overrides work', () => {
    assert.equal(setup().api.enabled, true);
    assert.equal(setup({ platform: 'MacIntel', userAgent: 'Macintosh' }).api.enabled, true);
    const desktop = setup({ platform: 'Win32', userAgent: 'Chrome', maxTouchPoints: 0 });
    assert.equal(desktop.api.enabled, false);
    desktop.event('pointerdown');
    assert.equal(desktop.contexts.length, 0);
    assert.equal(setup({ search: '?audio=godot' }).api.enabled, false);
    assert.equal(setup({ search: '?audio=browser', platform: 'Win32', userAgent: 'Chrome' }).api.enabled, true);
});

test('first Play gesture creates output before next-frame gameplay; source rate remains 48 kHz', () => {
    const app = setup();
    app.api.set_active(false);
    app.event('pointerdown');
    assert.equal(app.contexts.length, 1);
    app.api.set_active(true);
    app.api.push_pcm(pcm());
    const ctx = app.contexts[0];
    assert.equal(ctx.options.latencyHint, 'interactive');
    assert.equal(ctx.sampleRate, 44100);
    assert.equal(ctx.nodes[0].buffer.sampleRate, 48000);
    assert.equal(ctx.nodes[0].buffer.length, 792);
    assert.equal(ctx.nodes[0].startTime, 0.020);
    assert.ok(app.api.diagnostics().scheduledMs < 75);
});

test('failed context creation disables the alternate transport so Godot can take over', () => {
    const app = setup({ failCreate: true });
    app.event('pointerdown');
    assert.equal(app.api.enabled, false);
    assert.match(app.api.diagnostics().error, /Output unavailable/);
});

test('stalled output cannot retain stale seconds: both timeline and live nodes stay bounded', () => {
    const app = setup();
    app.event('pointerdown');
    app.api.set_active(true);
    for (let i = 0; i < 600; ++i) {
        app.api.push_pcm(pcm());
        const status = app.api.diagnostics();
        assert.ok(status.scheduledMs <= 75.0001);
        assert.ok(status.sources <= 4);
    }
    assert.ok(app.api.diagnostics().resets > 100);
    assert.ok(app.contexts[0].nodes.slice(0, -4).every(node => node.stopped && node.disconnected));
});

test('catch-up retains only newest 50 ms and mute cancels scheduled nodes immediately', () => {
    const app = setup();
    app.event('pointerdown');
    app.api.set_active(true);
    app.api.push_pcm(pcm(6000));
    const node = app.contexts[0].nodes[0];
    assert.equal(node.buffer.length, 2400);
    assert.ok(Math.abs(node.buffer.samples[0] - 0.6) < 0.0001);
    assert.ok(app.api.diagnostics().scheduledMs <= 70.001);
    app.api.set_active(false);
    assert.equal(node.stopped, true);
    assert.equal(app.api.diagnostics().sources, 0);
    app.api.push_pcm(pcm());
    assert.equal(app.contexts[0].nodes.length, 1);
});

test('hidden/interrupted output discards old PCM; returning reuses one context and resumes fresh', async () => {
    const app = setup();
    app.event('pointerdown');
    app.api.set_active(true);
    app.api.push_pcm(pcm());
    const first = app.contexts[0];
    first.state = 'interrupted';
    first.onstate();
    assert.equal(app.api.diagnostics().sources, 0);
    app.api.push_pcm(pcm());
    assert.equal(first.nodes.length, 1);
    app.event('touchend');
    assert.equal(first.state, 'running');
    app.root.document.hidden = true;
    app.event('visibilitychange');
    assert.equal(first.state, 'suspended');
    app.root.document.hidden = false;
    app.event('visibilitychange');
    await new Promise(resolve => setImmediate(resolve));
    assert.equal(app.contexts.length, 1);
    app.api.push_pcm(pcm());
    assert.equal(first.nodes.length, 2);
});

test('browser output selects Dummy before startup; normal desktop and A/B Godot retain engine audio', () => {
    const app = setup();
    assert.deepEqual(Array.from(app.api.engine_arguments()), ['--audio-driver', 'Dummy']);
    assert.equal(app.api.diagnostics().ownsOutput, true);
    assert.deepEqual(Array.from(setup({ search: '?audio=godot' }).api.engine_arguments()), []);
    assert.deepEqual(Array.from(setup({ platform: 'Win32', userAgent: 'Chrome' }).api.engine_arguments()), []);
    const failed = setup({ failCreate: true });
    failed.api.engine_arguments();
    failed.event('pointerdown');
    assert.equal(failed.api.enabled, true, 'Do not fall back to a silent Dummy driver');
    assert.match(failed.api.diagnostics().error, /Output unavailable/);
});

test('iOS requests ambient game semantics; unsupported session APIs do not break playback', () => {
    const app = setup();
    app.root.navigator.audioSession = { type: 'auto', state: 'inactive' };
    app.event('pointerdown');
    assert.equal(app.root.navigator.audioSession.type, 'ambient');
    assert.equal(app.api.diagnostics().audioSessionType, 'ambient');
    const rejected = setup();
    rejected.root.navigator.audioSession = { set type(value) { throw new Error('unsupported'); } };
    rejected.event('pointerdown');
    assert.equal(rejected.contexts[0].state, 'running');
});

test('rapid pause/resume survives asynchronous suspension without creating a second context', async () => {
    const app = setup();
    app.event('pointerdown');
    app.api.set_active(true);
    const ctx = app.contexts[0];
    let complete;
    ctx.suspend = () => new Promise(resolve => { complete = () => { ctx.state = 'suspended'; resolve(); }; });
    app.api.set_active(false);
    app.api.set_active(true);
    complete();
    await new Promise(resolve => setImmediate(resolve));
    assert.equal(ctx.state, 'running');
    assert.equal(app.contexts.length, 1);
    app.api.push_pcm(pcm());
    assert.equal(ctx.nodes.length, 1);
});

test('backgrounding during an in-flight resume does not leave output running', async () => {
    const app = setup();
    app.event('pointerdown');
    app.api.set_active(true);
    await new Promise(resolve => setImmediate(resolve));
    const ctx = app.contexts[0];
    ctx.state = 'suspended';
    let complete;
    ctx.resume = () => new Promise(resolve => { complete = () => { ctx.state = 'running'; resolve(); }; });
    app.event('touchend');
    app.root.document.hidden = true;
    app.event('visibilitychange');
    complete();
    await new Promise(resolve => setImmediate(resolve));
    assert.equal(ctx.state, 'suspended');
    app.api.set_active(true);
    assert.equal(ctx.state, 'suspended', 'Background render frames cannot resume output');
});

test('pausing while resume is pending reconciles to a suspended context', async () => {
    const app = setup();
    app.event('pointerdown');
    app.api.set_active(true);
    await new Promise(resolve => setImmediate(resolve));
    const ctx = app.contexts[0];
    ctx.state = 'suspended';
    let complete;
    ctx.resume = () => new Promise(resolve => { complete = () => { ctx.state = 'running'; resolve(); }; });
    app.event('touchend');
    app.api.set_active(false);
    complete();
    await new Promise(resolve => setImmediate(resolve));
    assert.equal(ctx.state, 'suspended');
});

test('10 minutes at 60 FPS stay bounded; delayed ended events do not retain nodes', () => {
    const app = setup();
    app.event('pointerdown');
    app.api.set_active(true);
    let simulationTime = 0;
    const frame = pcm();
    for (let i = 0; i < 36000; ++i) {
        app.advance(1 / 60);
        simulationTime += 1 / 60;
        while (simulationTime >= 0.0165) {
            app.api.push_pcm(frame);
            simulationTime -= 0.0165;
        }
        assert.ok(app.api.diagnostics().scheduledMs <= 75.0001);
        assert.ok(app.api.diagnostics().sources <= 5);
    }
    assert.equal(app.api.diagnostics().resets, 0);
    assert.ok(Math.abs(app.api.diagnostics().clockDriftMs) < 0.001);
});

test('irregular render frames and long hitches retain bounded fresh PCM', () => {
    const app = setup();
    app.event('pointerdown');
    app.api.set_active(true);
    let accumulator = 0;
    // Feed once per rendered frame, exactly like the game: at most four ROM
    // ticks with a native 50 ms queue. Deliberate hitches should trigger recovery.
    const frames = [0.012, 0.021, 0.016, 0.018, 0.009, 0.025, 0.080, 0.017];
    for (let i = 0; i < 800; ++i) {
        const delta = frames[i % frames.length];
        app.advance(delta);
        accumulator = Math.min(accumulator + delta, 0.066);
        let count = 0;
        while (accumulator >= 0.0165) {
            count += 792;
            accumulator -= 0.0165;
        }
        if (count) app.api.push_pcm(pcm(Math.min(count, 2400)));
        assert.ok(app.api.diagnostics().scheduledMs <= 75.0001);
        assert.ok(app.api.diagnostics().sources <= 5);
    }
});
