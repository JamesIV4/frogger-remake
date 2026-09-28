import { test } from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';

const source = fs.readFileSync('tools/web_layout.js', 'utf8');
function setup(navigator = { userAgent: 'iPhone', platform: 'iPhone', maxTouchPoints: 5 }) {
    const listeners = {};
    const frames = [];
    const timers = [];
    const style = { paddingLeft: '59px', paddingRight: '59px', paddingTop: '0px', paddingBottom: '21px' };
    const rect = { left: 0, top: 0, right: 932, bottom: 430, width: 932, height: 430 };
    const root = {
        navigator,
        innerWidth: 932, innerHeight: 430, devicePixelRatio: 3,
        document: {
            createElement: () => ({ style: {} }),
            body: { appendChild() {} },
            getElementById: () => ({ getBoundingClientRect: () => rect }),
            addEventListener: (event, fn) => { listeners[event] = fn; },
        },
        getComputedStyle: () => style,
        addEventListener: (event, fn) => { listeners[event] = fn; },
        requestAnimationFrame: fn => { frames.push(fn); },
        setTimeout: fn => { timers.push(fn); },
        visualViewport: { addEventListener: (event, fn) => { listeners[`visual:${event}`] = fn; } },
    };
    vm.runInNewContext(source, { window: root });
    return { root, style, rect, listeners, timers, flush() { while (frames.length) frames.shift()(); } };
}

test('CSS safe areas map into stretched Godot units without multiplying DPR twice', () => {
    const app = setup();
    const insets = app.root.FroggerLayout.get_insets(1040, 480);
    assert.ok(Math.abs(insets.left - 59 * 1040 / 932) < 0.001);
    assert.ok(Math.abs(insets.bottom - 21 * 480 / 430) < 0.001);
    app.root.devicePixelRatio = 1;
    assert.deepEqual(app.root.FroggerLayout.get_insets(1040, 480), insets);
});

test('canvas space already reserved by the host is not inset twice', () => {
    const app = setup();
    Object.assign(app.rect, { left: 59, right: 873, width: 814, bottom: 409, height: 409 });
    const insets = app.root.FroggerLayout.get_insets(960, 480);
    assert.equal(insets.left, 0);
    assert.equal(insets.right, 0);
    assert.equal(insets.bottom, 0);
});

test('orientation and delayed WebKit inset changes relayout even at unchanged canvas size', () => {
    const app = setup();
    let updates = 0;
    app.root.FroggerLayout.subscribe(() => { ++updates; });
    app.flush();
    assert.equal(updates, 1);
    app.style.paddingRight = '0px';
    app.listeners.orientationchange();
    app.flush();
    assert.equal(updates, 2);
    app.style.paddingLeft = '0px';
    app.style.paddingRight = '59px';
    app.timers.shift()();
    app.flush();
    assert.equal(updates, 3);
    const insets = app.root.FroggerLayout.get_insets(932, 430);
    assert.equal(insets.left, 0);
    assert.equal(insets.right, 59);
    app.listeners['visual:resize']();
    app.listeners.pageshow();
    app.flush();
    assert.equal(updates, 3, 'Unchanged geometry does not cause a layout loop');
});

test('browsers with zero safe areas preserve the whole canvas', () => {
    const app = setup();
    for (const key of Object.keys(app.style)) app.style[key] = '0px';
    const insets = app.root.FroggerLayout.get_insets(1040, 480);
    assert.equal(insets.left + insets.right + insets.top + insets.bottom, 0);
});

test('iOS portrait ignores reported PWA insets and rotation restores landscape protection', () => {
    for (const navigator of [
        { userAgent: 'iPhone', platform: 'iPhone', maxTouchPoints: 5 },
        { userAgent: 'Macintosh', platform: 'MacIntel', maxTouchPoints: 5 },
    ]) {
        const app = setup(navigator);
        app.style.paddingTop = '59px';
        app.style.paddingBottom = '90px';
        for (const [width, height] of [[480, 1040], [1040, 480], [480, 1040]]) {
            const insets = app.root.FroggerLayout.get_insets(width, height);
            if (height > width) {
                assert.deepEqual(Object.values(insets), [0, 0, 0, 0]);
            } else {
                assert.ok(insets.left > 0 && insets.right > 0 && insets.bottom > 0);
            }
        }
    }
});

test('other platforms retain portrait safe-area protection', () => {
    const app = setup({ userAgent: 'Android', platform: 'Linux armv8l', maxTouchPoints: 5 });
    assert.ok(app.root.FroggerLayout.get_insets(480, 1040).bottom > 0);
});
