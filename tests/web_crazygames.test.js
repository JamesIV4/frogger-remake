import { test } from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';

const source = fs.readFileSync('tools/web_crazygames.js', 'utf8');
async function setup({ missing = false, initFails = false, adThrows = false, disabled = false, muteAudio = false, telemetryFails = false, storedScore = null, user = null, dataFails = false } = {}) {
    const timers = new Map();
    const requests = [];
    const events = [];
    const telemetry = [];
    const record = (...event) => {
        telemetry.push(event);
        if (telemetryFails) throw Error('SDK telemetry unavailable');
    };
    let script;
    let serial = 0;
    let settingsListener;
    const saves = [];
    const root = {
        document: { createElement: () => ({}), head: { appendChild: s => { script = s; } } },
        setTimeout: (fn, ms) => { const id = ++serial; timers.set(id, { fn, ms }); return id; },
        clearTimeout: id => timers.delete(id),
        FroggerAudio: { set_active: active => events.push(['audio', active]) },
        CrazyGames: { SDK: {
            data: {
                getItem: () => { if (dataFails) throw Error('dataModuleDisabled'); return storedScore; },
                setItem: (key, value) => { storedScore = value; saves.push([key, value]); },
            },
            user: { isUserAccountAvailable: true, getUser: async () => user },
            environment: disabled ? 'disabled' : 'local',
            init: async () => { if (initFails) throw Error('blocked'); },
            game: {
                settings: { muteAudio },
                addSettingsChangeListener: listener => { settingsListener = listener; },
                gameplayStart: () => events.push('start'), gameplayStop: () => events.push('stop'),
                loadingStart: () => record('loadingStart'), loadingStop: () => record('loadingStop'),
                happytime: () => record('happytime'),
                reportGameCompletedPercentage: value => record('completion', value),
            },
            ad: { requestAd: (type, callbacks) => {
                if (adThrows) throw Error('offline');
                requests.push({ type, callbacks });
            } },
        } },
    };
    vm.runInNewContext(source, { window: root });
    if (!missing) await script.onload();
    return { api: root.FroggerCrazyGames, requests, events, telemetry, timers, script, saves,
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

test('loading signals bracket readiness once, including late SDK initialization', async () => {
    const h = await setup();
    assert.deepEqual(h.telemetry, [['loadingStart']]);
    h.api.loading_complete();
    h.api.loading_complete();
    assert.deepEqual(h.telemetry, [['loadingStart'], ['loadingStop']]);
    const late = await setup({ missing: true });
    late.api.loading_complete();
    assert.deepEqual(late.telemetry, []);
    await late.script.onload();
    assert.deepEqual(late.telemetry, [['loadingStart'], ['loadingStop']]);
});

test('only full game over reports completion, once per run before the ad timer fires', async () => {
    const h = await setup();
    h.api.update(false, false, false);
    h.api.begin_game();
    h.api.update(true, 1, true);
    h.api.update(true, 2, true);
    h.api.update(true, 2, false);
    assert.equal(h.telemetry.filter(e => e[0] === 'completion').length, 0);
    h.api.update(true, 0, false);
    h.api.update(true, 0, false);
    assert.deepEqual(h.telemetry.at(-1), ['completion', 100]);
    assert.equal(h.requests.length, 0);
    assert.equal(h.telemetry.filter(e => e[0] === 'completion').length, 1);
    h.api.begin_game();
    h.api.update(true, 0, false);
    assert.equal(h.telemetry.filter(e => e[0] === 'completion').length, 2);
});

test('completion before SDK initialization is delivered once when ready', async () => {
    const h = await setup({ missing: true });
    h.api.update(true, 0, false);
    await h.script.onload();
    h.api.update(true, 0, false);
    assert.equal(h.telemetry.filter(e => e[0] === 'completion').length, 1);
});

test('Happytime celebrates a new best only at full game over, once per run', async () => {
    const h = await setup();
    const celebrations = () => h.telemetry.filter(e => e[0] === 'happytime').length;
    h.api.begin_game(1000);
    h.api.check_score(1010);
    h.api.check_score(2000);
    h.api.update(true, 1, true);
    h.api.update(true, 2, true); // other player's turn is not the end
    assert.equal(celebrations(), 0);
    h.api.update(true, 0, false);
    h.api.update(true, 0, false);
    assert.equal(celebrations(), 1);
    h.api.begin_game(2000);
    h.api.check_score(2000); // tied score is not a new best
    h.api.update(true, 0, false);
    assert.equal(celebrations(), 1);
    h.api.begin_game(2000);
    h.api.check_score(2010);
    h.api.begin_game(2010); // voluntary restart cancels the celebration
    h.api.update(true, 0, false);
    assert.equal(celebrations(), 1);
    h.api.begin_game(0);
    h.api.check_score(10);
    assert.equal(celebrations(), 1);
    h.api.update(true, 0, false);
    assert.equal(celebrations(), 2);
});

test('telemetry failures cannot block loading, mute settings or game-over ads', async () => {
    const h = await setup({ telemetryFails: true, muteAudio: true });
    h.api.loading_complete();
    assert.equal(h.api.audio_muted, true);
    h.api.begin_game(100);
    h.api.check_score(110);
    h.api.update(true, 0, false);
    assert.equal(h.api.request_start(1), false);
    h.requests[0].callbacks.adError();
    assert.equal(h.api.take_restart(), 1);
});

test('account record is authoritative and never replaced by another local high score', async () => {
    const h = await setup({ storedScore: '500', user: { username: 'Player' } });
    assert.equal(h.api.account_linked, true);
    assert.equal(h.api.restore_high_score(9000), 500);
    assert.equal(h.saves.length, 0);
    h.api.save_high_score(400);
    assert.equal(h.saves.length, 0);
    h.api.save_high_score(600);
    assert.deepEqual(h.saves, [['frogger.highScore.v1', '600']]);
});

test('guest SDK save migrates a missing record and preserves higher cloud scores', async () => {
    const guest = await setup();
    assert.equal(guest.api.data_ready, true);
    assert.equal(guest.api.account_linked, false);
    assert.equal(guest.api.restore_high_score(1500), 1500);
    assert.deepEqual(guest.saves, [['frogger.highScore.v1', '1500']]);
    const cloud = await setup({ storedScore: '5000' });
    assert.equal(cloud.api.restore_high_score(1500), 5000);
    assert.equal(cloud.saves.length, 0);
});

test('unavailable data does not claim account linking or block the game', async () => {
    const h = await setup({ dataFails: true, user: { username: 'Player' } });
    assert.equal(h.api.data_ready, false);
    assert.equal(h.api.account_linked, false);
    h.api.save_high_score(1000);
    assert.equal(h.saves.length, 0);
    h.api.update(true, 0, false);
    assert.equal(h.api.request_start(1), false);
    h.requests[0].callbacks.adError();
    assert.equal(h.api.take_restart(), 1);
});
