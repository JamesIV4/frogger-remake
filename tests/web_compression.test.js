import { test } from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';
import { gzipSync } from 'node:zlib';

const wasm = Uint8Array.of(0, 97, 115, 109, 1, 0, 0, 0);
const manifest = { 'index.wasm': { file: 'index.wasm.bin', size: wasm.length, type: 'application/wasm' } };
const source = fs.readFileSync('tools/web_compression.js', 'utf8').replace('__FROGGER_COMPRESSED_ASSETS__', JSON.stringify(manifest));
function setup({ body = gzipSync(wasm), status = 200, decompression = DecompressionStream } = {}) {
    const requests = [];
    const context = {
        URL, Request, Response, Blob, Headers, Uint8Array, DecompressionStream: decompression,
        location: { href: 'https://games.example/nested/game/index.html', origin: 'https://games.example' },
        fetch: async (resource, init) => {
            requests.push({ resource, init });
            return new Response(body, { status });
        },
    };
    vm.createContext(context);
    vm.runInContext(source, context);
    return { context, requests, fetch: context.FroggerCompressedAssets.fetch };
}

test('compressed WASM round-trips and supports native streaming compilation without host headers', async () => {
    const app = setup();
    const response = await app.fetch('index.wasm');
    assert.equal(String(app.requests[0].resource), 'https://games.example/nested/game/index.wasm.bin');
    assert.equal(response.headers.get('Content-Type'), 'application/wasm');
    assert.equal(response.headers.get('Content-Length'), String(wasm.length));
    assert.deepEqual(new Uint8Array(await response.clone().arrayBuffer()), wasm);
    assert.ok(await WebAssembly.compileStreaming(response));
});

test('already decoded host responses are not decompressed twice', async () => {
    const app = setup({ body: wasm, decompression: undefined });
    const response = await app.fetch('index.wasm');
    assert.deepEqual(new Uint8Array(await response.arrayBuffer()), wasm);
});

test('nested hosting, query parameters and Request options survive asset mapping', async () => {
    const app = setup();
    const request = new Request('https://games.example/nested/game/index.wasm?v=2', { headers: { 'X-Test': 'retained' }, credentials: 'include' });
    await (await app.fetch(request)).arrayBuffer();
    const mapped = app.requests[0].resource;
    assert.equal(mapped.url, 'https://games.example/nested/game/index.wasm.bin?v=2');
    assert.equal(mapped.headers.get('X-Test'), 'retained');
    assert.equal(mapped.credentials, 'include');
});

test('unrelated URLs and non-GET requests pass through unchanged', async () => {
    const app = setup();
    for (const url of ['https://other.example/index.wasm', 'https://games.example/other/index.wasm', 'frogger-audio.js']) {
        await app.fetch(url);
        assert.equal(app.requests.at(-1).resource, url);
    }
    await app.fetch('index.wasm', { method: 'POST' });
    assert.equal(app.requests.at(-1).resource, 'index.wasm');
});

test('missing and corrupt payloads fail instead of reaching the WASM engine as HTML', async () => {
    await assert.rejects(setup({ status: 404 }).fetch('index.wasm'), /HTTP 404/);
    await assert.rejects(setup({ body: '<html>not a game</html>' }).fetch('index.wasm'), /Unexpected asset/);
    const corrupt = gzipSync(wasm).subarray(0, 12);
    const response = await setup({ body: corrupt }).fetch('index.wasm');
    await assert.rejects(response.arrayBuffer());
});

test('asset cache composes with decoding when browser storage is unavailable', async () => {
    const app = setup();
    vm.runInContext(fs.readFileSync('tools/web_cache.js', 'utf8'), app.context);
    const response = await app.context.FroggerAssetCache.fetch('index.wasm');
    assert.deepEqual(new Uint8Array(await response.arrayBuffer()), wasm);
    assert.equal(app.requests.length, 1);
    assert.equal(String(app.requests[0].resource), 'https://games.example/nested/game/index.wasm.bin');
});
