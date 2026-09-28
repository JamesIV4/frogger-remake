import { test } from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';

const source = fs.readFileSync('tools/web_crazygames.js', 'utf8');
async function setup({ missing = false, initFails = false, adThrows = false, disabled = false, muteAudio = false } = {}) {
    const timers = new Map();
    const requests = [];
    const events = [];
    let script;
    let serial = 0;
    let settingsListener;
    const root = {
        document: { createElement: () => ({}), head: { appendChild: s => { script = s; } } },
        setTimeout: (fn, ms) => { const id = ++serial; timers.set(id, { fn, ms }); return id; },
        clearTimeout: id => timers.delete(id),
        FroggerAudio: { set_active: active => events.push(['audio', active]) },
        CrazyGames: { SDK: {
            environment: disabled ? 'disabled' : 'local',
            init: async () => { if (initFails) throw Error('blocked'); },
            game: {
                settings: { muteAudio },
                addSettingsChangeListener: listener => { settingsListener = listener; },
                gameplayStart: () => events.push('start'), gameplayStop: () => events.push('stop'),
            },
            ad: { requestAd: (type, callbacks) => {
                if (adThrows) throw Error('offline');
                requests.push({ type, callbacks });
            } },
        } },
    };
    vm.runInNewContext(source, { window: root });
    if (!missing) await script.onload();
    return { api: root.FroggerCrazyGames, requests, events, timers, script,
        settings: settings => settingsListener(settings),
        fire: () => { for (const [id, { fn, ms }] of [...timers]) { assert.equal(ms, 1000); timers.delete(id); fn(); } } };
}

test('initial SDK mute silences audio and survives a new game', async () => {
    const h = await setup({ muteAudio: true });
    assert.equal(h.api.audio_muted, true);
    assert.deepEqual(h.events, [['audio', false]]);
    h.api.begin_game();
    h.api.update(true, true, true);
    assert.equal(h.api.audio_muted, true);
});

test('live settings mute immediately; clearing host mute never forces audio on', async () => {
    const h = await setup();
    assert.equal(h.api.audio_muted, false);
    h.settings({ muteAudio: true, disableChat: true });
    assert.equal(h.api.audio_muted, true);
    assert.deepEqual(h.events, [['audio', false]]);
    h.settings({ muteAudio: false });
    assert.equal(h.api.audio_muted, false);
    // Godot alone resumes audio after checking the player's mute and pause state.
    assert.deepEqual(h.events, [['audio', false]]);
});

test('ad completion does not clear host mute, and host unmute does not end an ad', async () => {
    const h = await setup();
    h.api.update(true, false, false);
    h.fire();
    h.settings({ muteAudio: true });
    h.requests[0].callbacks.adFinished();
    assert.equal(h.api.audio_muted, true);
    h.api.begin_game();
    h.api.update(true, false, false);
    h.fire();
    h.settings({ muteAudio: false });
    assert.equal(h.api.busy, true);
    assert.equal(h.events.some(event => Array.isArray(event) && event[1] === true), false);
});

test('initial menu, active play, pauses and player handoffs never arm ads', async () => {
    const h = await setup();
    h.api.update(false, false, false);
    assert.equal(h.api.request_start(1), true);
    h.api.begin_game();
    h.api.update(true, 1, true);
    h.api.update(true, 1, false);
    h.api.update(true, 2, true);
    assert.equal(h.timers.size, 0);
    assert.equal(h.requests.length, 0);
    assert.deepEqual(h.events, ['start', 'stop', 'start']);
});

test('full game over schedules exactly one ad after one second', async () => {
    const h = await setup();
    h.api.update(true, true, true);
    h.api.update(true, false, false);
    h.api.update(true, false, false);
    assert.equal(h.timers.size, 1);
    assert.equal(h.requests.length, 0);
    h.fire();
    assert.equal(h.requests[0].type, 'midgame');
    assert.equal(h.api.busy, true);
    assert.deepEqual(h.events.at(-1), ['audio', false]);
    h.requests[0].callbacks.adFinished();
    h.api.update(true, false, false);
    h.fire();
    assert.equal(h.requests.length, 1);
    assert.equal(h.api.take_restart(), 0);
    assert.equal(h.api.request_start(1), true);
});

test('early restart requests the ad immediately and resumes selected mode only once', async () => {
    const h = await setup();
    h.api.update(true, false, false);
    assert.equal(h.api.request_start(2), false);
    assert.equal(h.api.request_start(1), false);
    assert.equal(h.timers.size, 0);
    assert.equal(h.requests.length, 1);
    assert.equal(h.api.take_restart(), 0);
    h.requests[0].callbacks.adFinished();
    assert.equal(h.api.take_restart(), 2);
    assert.equal(h.api.take_restart(), 0);
    h.api.begin_game();
    h.api.update(true, true, true);
    h.requests[0].callbacks.adError(); // duplicate/late completion is harmless
    assert.equal(h.api.busy, false);
    h.api.update(true, false, false);
    h.fire();
    assert.equal(h.requests.length, 2);
});

test('restart during timed ad waits for adError, including no fill and cooldown', async () => {
    const h = await setup();
    h.api.update(true, false, false);
    h.fire();
    assert.equal(h.api.request_start(1), false);
    h.requests[0].callbacks.adError({ code: 'adCooldown' });
    assert.equal(h.api.busy, false);
    assert.equal(h.api.take_restart(), 1);
});

for (const option of ['missing', 'initFails', 'adThrows', 'disabled']) {
    test(`${option} does not block restart`, async () => {
        const h = await setup({ [option]: true });
        h.api.update(true, false, false);
        assert.equal(h.api.request_start(1), true);
        assert.equal(h.api.busy, false);
    });
}

test('returning to title cancels timer; late SDK initialization cannot interrupt a new game', async () => {
    const h = await setup({ missing: true });
    h.api.update(true, false, false);
    h.api.update(false, false, false);
    assert.equal(h.timers.size, 0);
    h.api.update(true, false, false);
    assert.equal(h.api.request_start(1), true);
    h.api.begin_game();
    h.api.update(true, true, true);
    await h.script.onload();
    h.fire();
    assert.equal(h.requests.length, 0);
});
