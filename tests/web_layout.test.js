import { test } from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';

const source = fs.readFileSync('tools/web_layout.js', 'utf8');
function setup(navigator = { userAgent: 'Android', platform: 'Linux armv8l', maxTouchPoints: 5 }, overrides = {}) {
    const listeners = {};
    const frames = [];
    const timers = [];
    const style = { paddingLeft: '59px', paddingRight: '59px', paddingTop: '0px', paddingBottom: '21px' };
    const rect = { left: 0, top: 0, right: 932, bottom: 430, width: 932, height: 430 };
    const canvas = {
        width: 0, height: 0, style: {},
        getBoundingClientRect() {
            const width = parseFloat(this.style.width) || rect.width;
            const height = parseFloat(this.style.height) || rect.height;
            return { ...rect, width, height, right: rect.left + width, bottom: rect.top + height };
        },
    };
    const viewportMeta = { content: 'width=device-width, user-scalable=no, initial-scale=1.0, viewport-fit=cover' };
    const root = {
        navigator,
        innerWidth: 932, innerHeight: 430, devicePixelRatio: 3,
        document: {
            createElement: () => ({ style: {} }),
            querySelector: selector => selector === 'meta[name="viewport"]' ? viewportMeta : { content: 'black' },
            body: { appendChild() {} },
            getElementById: () => canvas,
            addEventListener: (event, fn) => { listeners[event] = fn; },
        },
        getComputedStyle: () => style,
        addEventListener: (event, fn) => { listeners[event] = fn; },
        requestAnimationFrame: fn => { frames.push(fn); },
        setTimeout: fn => { timers.push(fn); },
        visualViewport: { addEventListener: (event, fn) => { listeners[`visual:${event}`] = fn; } },
    };
    Object.assign(root, overrides);
    vm.runInNewContext(source, { window: root });
    return { root, style, rect, canvas, viewportMeta, listeners, timers, flush() { while (frames.length) frames.shift()(); } };
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

test('iOS restores pre-regression viewport fitting and leaves canvas sizing entirely to Godot', () => {
    for (const navigator of [
        { userAgent: 'iPhone', platform: 'iPhone', maxTouchPoints: 5, standalone: true },
        { userAgent: 'iPhone', platform: 'iPhone', maxTouchPoints: 5 },
        { userAgent: 'Macintosh', platform: 'MacIntel', maxTouchPoints: 5, standalone: true },
    ]) {
        const app = setup(navigator, { screen: { width: 430, height: 932 } });
        const config = { canvasResizePolicy: 2 };
        app.root.FroggerLayout.configure_engine(config);
        assert.equal(app.viewportMeta.content, 'width=device-width, user-scalable=no, initial-scale=1.0');
        app.style.paddingTop = '59px';
        app.style.paddingBottom = '90px';
        for (const [width, height] of [[480, 1040], [1040, 480], [480, 1040]]) {
            app.root.innerWidth = width;
            app.root.innerHeight = height;
            app.listeners.orientationchange(); app.flush();
            while (app.timers.length) { app.timers.shift()(); app.flush(); }
            assert.equal(config.canvasResizePolicy, 2);
            assert.deepEqual(Object.values(app.root.FroggerLayout.get_insets(width, height)), [0, 0, 0, 0]);
            assert.equal(app.canvas.width + app.canvas.height, 0, 'No manual framebuffer size writes');
            assert.deepEqual(app.canvas.style, {}, 'No fixed positioning or CSS size overrides');
        }
    }
});

test('Android retains cover mode and explicit top and bottom safe-area padding', () => {
    const app = setup();
    app.viewportMeta.content = 'width=device-width, initial-scale=1.0';
    const config = { canvasResizePolicy: 2 };
    app.root.FroggerLayout.configure_engine(config);
    assert.match(app.viewportMeta.content, /viewport-fit=cover/);
    assert.equal(config.canvasResizePolicy, 2);
    app.style.paddingTop = '59px';
    const insets = app.root.FroggerLayout.get_insets(480, 1040);
    assert.ok(insets.top > 0 && insets.bottom > 0);
});

test('ignored iOS safe-area changes cannot drive a layout feedback loop', () => {
    const app = setup({ userAgent: 'iPhone', standalone: true });
    let updates = 0;
    app.root.FroggerLayout.subscribe(() => { ++updates; });
    app.flush();
    app.root.getComputedStyle = () => { throw new Error('iOS must not apply env insets'); };
    for (const bottom of ['0px', '34px', '93px', '0px']) {
        app.style.paddingBottom = bottom;
        app.listeners.resize(); app.flush();
        while (app.timers.length) { app.timers.shift()(); app.flush(); }
    }
    assert.equal(updates, 1);
});

test('device diagnostics identify the baseline behavior and raw viewport mismatch', () => {
    const app = setup({ userAgent: 'iPhone', standalone: true }, {
        innerWidth: 430, innerHeight: 839, screen: { width: 430, height: 932 },
        visualViewport: { width: 430, height: 839, addEventListener() {} },
    });
    app.root.FroggerLayout.configure_engine({ canvasResizePolicy: 2 });
    app.style.paddingTop = '59px';
    const text = app.root.FroggerLayout.diagnostics_text();
    assert.match(text, /Layout: ios-baseline-v1/);
    assert.match(text, /Fit browser default/);
    assert.match(text, /Screen 430x932/);
    assert.match(text, /Visual 430x839/);
    assert.match(text, /Applied T\/B 0\/0/);
});
