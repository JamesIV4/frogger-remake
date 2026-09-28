#!/usr/bin/env python3
"""Package Godot's large assets as gzip payloads decoded by our web loader."""
import gzip
import json
from pathlib import Path


def main():
    root = Path(__file__).resolve().parents[1]
    output = root / 'builds' / 'web'
    assets = {}
    sources = sorted([*output.glob('*.wasm'), *output.glob('*.pck')])
    if not sources:
        raise RuntimeError('Run the Godot Web export before compressing assets.')
    raw_total = 0
    packed_total = 0
    for source in sources:
        raw = source.read_bytes()
        packed = gzip.compress(raw, compresslevel=9, mtime=0)
        # .bin avoids relying on host-specific .gz response headers.
        target = source.with_name(source.name + '.bin')
        target.write_bytes(packed)
        if gzip.decompress(target.read_bytes()) != raw:
            raise RuntimeError(f'Compression round trip failed: {source.name}')
        assets[source.name] = {
            'file': target.name,
            'size': len(raw),
            'type': 'application/wasm' if source.suffix == '.wasm' else 'application/octet-stream',
        }
        raw_total += len(raw)
        packed_total += len(packed)
    template = (root / 'tools' / 'web_compression.js').read_text(encoding='utf-8')
    (output / 'frogger-compression.js').write_text(
        template.replace('__FROGGER_COMPRESSED_ASSETS__', json.dumps(assets, separators=(',', ':'))),
        encoding='utf-8',
    )
    # Remove raw copies only after every payload and the loader are ready.
    for source in sources:
        source.unlink()
    expected = {asset['file'] for asset in assets.values()}
    for pattern in ('*.wasm.bin', '*.pck.bin'):
        for stale in output.glob(pattern):
            if stale.name not in expected:
                stale.unlink()
    total = sum(path.stat().st_size for path in output.rglob('*') if path.is_file())
    print(f'Compressed assets: {raw_total:,} -> {packed_total:,} bytes')
    print(f'Complete web upload: {total:,} bytes ({total / 1_000_000:.2f} MB)')
    if total > 20_000_000:
        print('WARNING: The web upload exceeds the 20 MB mobile homepage size target.')


if __name__ == '__main__':
    main()
