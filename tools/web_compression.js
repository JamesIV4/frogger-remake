// Lossless asset decoding for hosts that do not expose Content-Encoding controls.
(function (root) {
    'use strict';
    const assets = __FROGGER_COMPRESSED_ASSETS__;
    const nativeFetch = root.fetch.bind(root);
    const base = new URL('.', root.location.href);

    async function fetchAsset(resource, init) {
        const url = new URL(typeof resource === 'string' || resource instanceof URL ? resource : resource.url, base);
        const name = url.pathname.split('/').pop();
        const asset = assets[name];
        const method = String((init && init.method) || (resource && resource.method) || 'GET').toUpperCase();
        if (!asset || method !== 'GET' || url.origin !== base.origin || url.pathname !== new URL(name, base).pathname) {
            return nativeFetch(resource, init);
        }
        const packedUrl = new URL(asset.file, url);
        packedUrl.search = url.search;
        const request = resource instanceof Request ? new Request(packedUrl, resource) : packedUrl;
        const response = await nativeFetch(request, init);
        if (!response.ok) throw new Error(`Unable to load ${asset.file}: HTTP ${response.status}`);
        const bytes = new Uint8Array(await response.arrayBuffer());
        let body = bytes;
        // A host may transparently decode gzip. Inspect bytes, not a header
        // that might describe an additional transport compression layer.
        if (bytes[0] === 0x1f && bytes[1] === 0x8b) {
            if (!root.DecompressionStream) throw new Error('This browser needs gzip DecompressionStream support to load the game.');
            body = new Blob([bytes]).stream().pipeThrough(new root.DecompressionStream('gzip'));
        } else if (bytes.byteLength !== asset.size) {
            throw new Error(`Unexpected asset data: ${asset.file}`);
        }
        return new Response(body, {
            headers: { 'Content-Type': asset.type, 'Content-Length': String(asset.size) },
        });
    }

    root.FroggerCompressedAssets = Object.freeze({ fetch: fetchAsset });
})(globalThis);
