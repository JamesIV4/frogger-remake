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
    const standalone = navigator.standalone === true || Boolean(root.matchMedia &&
        (root.matchMedia('(display-mode: standalone)').matches || root.matchMedia('(display-mode: fullscreen)').matches));

    function geometry() {
        const canvas = root.document.getElementById('canvas');
        const rect = canvas ? canvas.getBoundingClientRect() : { left: 0, top: 0, right: root.innerWidth, bottom: root.innerHeight, width: root.innerWidth, height: root.innerHeight };
        // Restore the September 27 iOS behavior: Safari fits the viewport and
        // Godot sizes its canvas. Do not inset that already-fitted viewport again.
        if (ios) return { width: rect.width, height: rect.height, left: 0, right: 0, top: 0, bottom: 0 };
        const style = root.getComputedStyle(probe);
        const px = value => Math.max(0, parseFloat(value) || 0);
        // Subtract any inset already provided by the canvas's surrounding page.
        const area = {
            width: rect.width, height: rect.height,
            left: Math.max(0, px(style.paddingLeft) - rect.left),
            right: Math.max(0, rect.right - (root.innerWidth - px(style.paddingRight))),
            top: Math.max(0, px(style.paddingTop) - rect.top),
            bottom: Math.max(0, rect.bottom - (root.innerHeight - px(style.paddingBottom))),
        };
        return area;
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
        configure_engine(config) {
            const meta = root.document.querySelector('meta[name="viewport"]');
            if (meta) {
                const values = meta.content.split(',').map(value => value.trim()).filter(value => !value.startsWith('viewport-fit='));
                if (!ios) values.push('viewport-fit=cover');
                const content = values.join(', ');
                if (meta.content !== content) meta.content = content;
            }
            // One sizing owner in every orientation. No physical-screen sizing,
            // visualViewport sizing, or fixed-position canvas override on iOS.
            if (ios) config.canvasResizePolicy = 2;
        },
        get_insets(width, height) {
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
        diagnostics_text() {
            const round = value => Math.round(Number(value) || 0);
            const pair = object => object ? `${round(object.width)}x${round(object.height)}` : '?';
            const visible = root.visualViewport;
            const canvas = root.document.getElementById('canvas');
            const rect = canvas.getBoundingClientRect();
            const style = root.getComputedStyle(probe);
            const area = geometry();
            const meta = root.document.querySelector && root.document.querySelector('meta[name="apple-mobile-web-app-status-bar-style"]');
            return [
                'Layout: ios-baseline-v1',
                `iOS ${ios} PWA ${standalone}`,
                `Fit ${ios ? 'browser default' : 'cover'}`,
                `Screen ${pair(root.screen)}`,
                `Window ${round(root.innerWidth)}x${round(root.innerHeight)}`,
                `Visual ${pair(visible)} @ ${round(visible && visible.offsetLeft)},${round(visible && visible.offsetTop)}`,
                `Canvas ${pair(rect)} @ ${round(rect.left)},${round(rect.top)}`,
                `Safe T/R/B/L ${[style.paddingTop, style.paddingRight, style.paddingBottom, style.paddingLeft].map(value => round(parseFloat(value))).join('/')}`,
                `Applied T/B ${round(area.top)}/${round(area.bottom)}`,
                `Status ${meta ? meta.content : '?'}`,
            ].join('\n');
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
    if (root.ResizeObserver && !ios) {
        const observer = new root.ResizeObserver(schedule);
        observer.observe(probe);
        // Godot observes its viewport itself. Observing the canvas here would
        // feed engine size changes back into browser-driven UI relayout.
    }
})(window);
