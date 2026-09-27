// SPDX-License-Identifier: GPL-3.0-only
(function (root) {
    'use strict';

    const VERSION = '__FROGGER_CACHE_VERSION__';
    const DB_PREFIX = 'frogger-assets-';
    const DB_NAME = DB_PREFIX + VERSION;
    const STORE_NAME = 'responses';
    const VERSION_KEY = 'frogger-asset-cache-version';
    const CACHEABLE_FILES = new Set([
        'index.pck',
        'index.wasm',
        'index.side.part0.wasm',
        'index.side.part1.wasm',
        'libfrogger_arcade.web.template_release.wasm32.wasm',
    ]);
    const nativeFetch = root.fetch.bind(root);
    const stats = { hits: 0, misses: 0, writes: 0 };
    let databasePromise = null;

    function resourceUrl(resource) {
        return new URL(typeof resource === 'string' ? resource : resource.url, root.location.href);
    }

    function isCacheable(resource, init) {
        const url = resourceUrl(resource);
        const method = String((init && init.method) || (resource && resource.method) || 'GET').toUpperCase();
        if (method !== 'GET' || url.origin !== root.location.origin || !CACHEABLE_FILES.has(url.pathname.split('/').pop())) {
            return false;
        }
        const headers = new Headers((init && init.headers) || (resource && resource.headers) || undefined);
        return !headers.has('Range');
    }

    function cleanOldDatabase() {
        if (!root.indexedDB) return;
        try {
            const previous = root.localStorage && root.localStorage.getItem(VERSION_KEY);
            if (previous && previous !== VERSION) root.indexedDB.deleteDatabase(DB_PREFIX + previous);
            if (root.localStorage) root.localStorage.setItem(VERSION_KEY, VERSION);
        } catch (_) {
            // A new version still uses a different database name when storage
            // introspection is unavailable, so stale bytes can never be read.
        }
    }

    function openDatabase() {
        if (!root.indexedDB) return Promise.resolve(null);
        if (databasePromise) return databasePromise;
        databasePromise = new Promise(resolve => {
            const request = root.indexedDB.open(DB_NAME, 1);
            request.onupgradeneeded = () => {
                if (!request.result.objectStoreNames.contains(STORE_NAME)) request.result.createObjectStore(STORE_NAME);
            };
            request.onsuccess = () => resolve(request.result);
            request.onerror = () => resolve(null);
            request.onblocked = () => resolve(null);
        });
        return databasePromise;
    }

    async function readResponse(url) {
        const database = await openDatabase();
        if (!database) return null;
        return new Promise(resolve => {
            const request = database.transaction(STORE_NAME, 'readonly').objectStore(STORE_NAME).get(url);
            request.onsuccess = () => {
                const saved = request.result;
                if (!saved || saved.version !== VERSION) return resolve(null);
                resolve(new Response(saved.body, {
                    status: saved.status,
                    statusText: saved.statusText,
                    headers: saved.headers,
                }));
            };
            request.onerror = () => resolve(null);
        });
    }

    async function writeResponse(url, response) {
        const database = await openDatabase();
        if (!database) return;
        const body = await response.arrayBuffer();
        const headers = [];
        response.headers.forEach((value, name) => {
            // Fetch exposes decoded response bodies. Do not preserve transport
            // encoding or byte length on the reconstructed local response.
            if (!['content-encoding', 'content-length', 'transfer-encoding'].includes(name.toLowerCase())) {
                headers.push([name, value]);
            }
        });
        await new Promise((resolve, reject) => {
            const transaction = database.transaction(STORE_NAME, 'readwrite');
            transaction.oncomplete = resolve;
            transaction.onerror = () => reject(transaction.error);
            transaction.onabort = () => reject(transaction.error);
            transaction.objectStore(STORE_NAME).put({
                version: VERSION,
                status: response.status,
                statusText: response.statusText,
                headers,
                body,
            }, url);
        });
        stats.writes += 1;
    }

    async function cachedFetch(resource, init) {
        if (!isCacheable(resource, init)) return nativeFetch(resource, init);
        const url = resourceUrl(resource).href;
        const saved = await readResponse(url);
        if (saved) {
            stats.hits += 1;
            return saved;
        }
        stats.misses += 1;
        const response = await nativeFetch(resource, init);
        if (response.ok) writeResponse(url, response.clone()).catch(() => {});
        return response;
    }

    cleanOldDatabase();
    root.FroggerAssetCache = Object.freeze({ VERSION, fetch: cachedFetch, isCacheable, stats });
})(globalThis);
