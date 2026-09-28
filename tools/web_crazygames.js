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
    let loaded = false;
    let loadingStopped = false;
    let previousBest = 0;
    let newBest = false;
    let pendingCompletions = 0;
    const scoreKey = 'frogger.highScore.v1';
    let storedScoreMissing = false;
    let signedIn = false;

    function validScore(value) {
        const score = Number(value);
        return Number.isSafeInteger(score) && score >= 0 && score <= 999990 ? score : 0;
    }

    async function loadAccountScore() {
        try {
            const value = sdk.data.getItem(scoreKey);
            storedScoreMissing = value === null;
            api.high_score = validScore(value);
            api.data_ready = true;
            if (sdk.user.isUserAccountAvailable) signedIn = Boolean(await sdk.user.getUser());
            api.account_linked = signedIn && api.data_ready;
        } catch (_) { api.account_linked = false; }
    }

    function gameEvent(method, ...args) {
        if (!sdk) return;
        try {
            Promise.resolve(sdk.game[method](...args)).catch(() => {});
        } catch (_) { /* SDK telemetry must never block the game. */ }
    }

    function stopLoading() {
        if (!sdk || !loaded || loadingStopped) return;
        loadingStopped = true;
        gameEvent('loadingStop');
    }

    function reportCompletions() {
        if (!sdk) return;
        while (pendingCompletions > 0) {
            pendingCompletions--;
            gameEvent('reportGameCompletedPercentage', 100);
        }
    }

    function applySettings(settings) {
        api.audio_muted = Boolean(settings && settings.muteAudio);
        // Stop queued browser audio immediately, including between Godot frames.
        // Unmuting is left to feed_audio so pause, ads and user mute still win.
        if (api.audio_muted && root.FroggerAudio) root.FroggerAudio.set_active(false);
    }

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
        audio_muted: false,
        data_ready: false,
        account_linked: false,
        high_score: 0,
        restore_high_score(legacyScore) {
            // Migrate once only if this SDK save has no record. Never overwrite it
            // with a browser score that could belong to a different account.
            if (api.data_ready && storedScoreMissing) api.save_high_score(legacyScore);
            previousBest = Math.max(previousBest, api.high_score);
            newBest = false;
            return api.high_score;
        },
        save_high_score(score) {
            if (!sdk || !api.data_ready) return;
            try {
                const stored = validScore(sdk.data.getItem(scoreKey));
                const best = Math.max(stored, api.high_score, validScore(score));
                if (storedScoreMissing || best > stored) sdk.data.setItem(scoreKey, String(best));
                storedScoreMissing = false;
                api.high_score = best;
                api.account_linked = signedIn;
            } catch (_) { api.account_linked = false; }
        },
        loading_complete() {
            loaded = true;
            stopLoading();
        },
        check_score(score) {
            // Remember the achievement; celebrate only when the whole run ends.
            if (score > previousBest) newBest = true;
        },
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
                pendingCompletions++;
                if (newBest) gameEvent('happytime');
            }
            reportGameplay();
            reportCompletions();
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
        begin_game(highScore = 0) {
            cancelTimer();
            over = false;
            consumed = false;
            pendingRestart = 0;
            previousBest = highScore;
            newBest = false;
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
            gameEvent('loadingStart');
            stopLoading();
            applySettings(sdk.game.settings);
            sdk.game.addSettingsChangeListener(applySettings);
            reportGameplay();
            reportCompletions();
            await loadAccountScore();
        } catch (_) { /* Offline, blocked, or unsupported host: play without ads. */ }
    };
    script.onerror = () => {};
    root.document.head.appendChild(script);
})(window);
