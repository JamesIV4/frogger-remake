/* CrazyGames v3 bridge. Only a completed arcade game can arm a midgame ad. */
(function (root) {
    'use strict';
    let sdk = null;
    let over = false;
    let consumed = false;
    let timer = null;
    let pendingRestart = 0;
    let playing = false;
    let reportedPlaying = false;

    function reportGameplay() {
        const next = playing && !api.busy;
        if (!sdk || next === reportedPlaying) return;
        reportedPlaying = next;
        try {
            Promise.resolve(sdk.game[next ? 'gameplayStart' : 'gameplayStop']()).catch(() => {});
        } catch (_) { /* Analytics must never block play. */ }
    }

    function cancelTimer() {
        if (timer !== null) root.clearTimeout(timer);
        timer = null;
    }

    function requestAd() {
        if (!over || consumed) return;
        consumed = true;
        cancelTimer();
        // Do not defer ads until a late SDK initialization: that could interrupt a new game.
        if (!sdk) return;
        api.busy = true;
        reportGameplay();
        if (root.FroggerAudio) root.FroggerAudio.set_active(false);
        let finished = false;
        const finish = () => {
            if (finished) return;
            finished = true;
            api.busy = false;
            reportGameplay();
        };
        try {
            const result = sdk.ad.requestAd('midgame', {
                adStarted: () => {}, adFinished: finish, adError: finish,
            });
            // Some SDK failures reject instead of invoking adError.
            if (result && typeof result.catch === 'function') result.catch(finish);
        } catch (_) { finish(); }
    }

    const api = root.FroggerCrazyGames = {
        busy: false,
        update(started, active, gameplay) {
            playing = Boolean(gameplay);
            if (!started) {
                cancelTimer();
                over = false;
                consumed = false;
            } else if (!active && !over) {
                over = true;
                consumed = false;
                timer = root.setTimeout(requestAd, 1000);
            }
            reportGameplay();
        },
        request_start(players) {
            requestAd();
            if (!api.busy) return true;
            // First press wins; held/repeated input cannot create more requests.
            if (!pendingRestart) pendingRestart = players;
            return false;
        },
        take_restart() {
            if (api.busy) return 0;
            const players = pendingRestart;
            pendingRestart = 0;
            return players;
        },
        begin_game() {
            cancelTimer();
            over = false;
            consumed = false;
            pendingRestart = 0;
        },
    };

    // Async loading keeps blocked/offline SDK requests from delaying the game.
    const script = root.document.createElement('script');
    script.src = 'https://sdk.crazygames.com/crazygames-sdk-v3.js';
    script.async = true;
    script.onload = async () => {
        try {
            const candidate = root.CrazyGames.SDK;
            await candidate.init();
            if (candidate.environment === 'disabled') return;
            sdk = candidate;
            reportGameplay();
        } catch (_) { /* Offline, blocked, or unsupported host: play without ads. */ }
    };
    script.onerror = () => {};
    root.document.head.appendChild(script);
})(window);
