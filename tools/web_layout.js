// Browser safe-area measurements in CSS pixels, mapped to Godot UI units.
(function (root) {
    'use strict';
    const probe = root.document.createElement('div');
    probe.id = 'frogger-safe-area';
    probe.style.cssText = 'position:fixed;visibility:hidden;pointer-events:none;inset:0 auto auto 0;width:0;height:0;padding:env(safe-area-inset-top,0px) env(safe-area-inset-right,0px) env(safe-area-inset-bottom,0px) env(safe-area-inset-left,0px)';
    root.document.body.appendChild(probe);
    let callback = null;
    let framePending = false;
    let signature = '';
    const navigator = root.navigator || {};
    const ios = /iPad|iPhone|iPod/.test(navigator.userAgent || '') ||
        (navigator.platform === 'MacIntel' && navigator.maxTouchPoints > 1);

    function geometry() {
        const canvas = root.document.getElementById('canvas');
        const rect = canvas ? canvas.getBoundingClientRect() : { left: 0, top: 0, right: root.innerWidth, bottom: root.innerHeight, width: root.innerWidth, height: root.innerHeight };
        const style = root.getComputedStyle(probe);
        const px = value => Math.max(0, parseFloat(value) || 0);
        // Subtract any inset already provided by the canvas's surrounding page.
        return {
            width: rect.width, height: rect.height,
            left: Math.max(0, px(style.paddingLeft) - rect.left),
            right: Math.max(0, rect.right - (root.innerWidth - px(style.paddingRight))),
            top: Math.max(0, px(style.paddingTop) - rect.top),
            bottom: Math.max(0, rect.bottom - (root.innerHeight - px(style.paddingBottom))),
        };
    }

    function notify() {
        framePending = false;
        const next = JSON.stringify(geometry());
        if (next !== signature) {
            signature = next;
            if (callback) callback();
        }
    }

    function schedule() {
        if (!framePending) {
            framePending = true;
            root.requestAnimationFrame(notify);
        }
    }

    root.FroggerLayout = {
        get_insets(width, height) {
            // iOS portrait deliberately uses the whole edge-to-edge PWA canvas,
            // including rounded edges. Do not lift the footer for reported insets.
            // Landscape still needs explicit protection for either notch side.
            if (ios && height >= width) return { left: 0, right: 0, top: 0, bottom: 0 };
            const area = geometry();
            // Ratios account for both Godot stretch and devicePixelRatio.
            const x = width / Math.max(1, area.width);
            const y = height / Math.max(1, area.height);
            return { left: area.left * x, right: area.right * x, top: area.top * y, bottom: area.bottom * y };
        },
        subscribe(listener) {
            callback = listener;
            signature = '';
            schedule();
        },
    };
    for (const event of ['resize', 'orientationchange', 'pageshow']) {
        root.addEventListener(event, () => {
            schedule();
            // WebKit may settle the safe-area env values after the resize event.
            root.setTimeout(schedule, 100);
            root.setTimeout(schedule, 300);
        });
    }
    root.document.addEventListener('fullscreenchange', schedule);
    if (root.visualViewport) {
        root.visualViewport.addEventListener('resize', schedule);
        root.visualViewport.addEventListener('scroll', schedule);
    }
    if (root.ResizeObserver) {
        const observer = new root.ResizeObserver(schedule);
        observer.observe(probe);
        const canvas = root.document.getElementById('canvas');
        if (canvas) observer.observe(canvas);
    }
})(window);
