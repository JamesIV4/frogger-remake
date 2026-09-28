import { test } from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';

const source = fs.readFileSync('tools/web_crazygames.js', 'utf8');
async function setup({ missing = false, initFails = false, adThrows = false, disabled = false } = {}) {
    const timers = new Map();
    const requests = [];
    const events = [];
    let script;
    let serial = 0;
    const root = {
        document: { createElement: () => ({}), head: { appendChild: s => { script = s; } } },
        setTimeout: (fn, ms) => { const id = ++serial; timers.set(id, { fn, ms }); return id; },
        clearTimeout: id => timers.delete(id),
        FroggerAudio: { set_active: active => events.push(['audio', active]) },
        CrazyGames: { SDK: {
            environment: disabled ? 'disabled' : 'local',
            init: async () => { if (initFails) throw Error('blocked'); },
            game: { gameplayStart: () => events.push('start'), gameplayStop: () => events.push('stop') },
            ad: { requestAd: (type, callbacks) => {
                if (adThrows) throw Error('offline');
                requests.push({ type, callbacks });
            } },
        } },
    };
    vm.runInNewContext(source, { window: root });
    if (!missing) await script.onload();
    return { api: root.FroggerCrazyGames, requests, events, timers, script,
        fire: () => { for (const [id, { fn, ms }] of [...timers]) { assert.equal(ms, 1000); timers.delete(id); fn(); } } };
}

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
