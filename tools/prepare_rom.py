"""Verify and prepare ONLY the supplied Frogger set. Never fetch game ROMs."""
from pathlib import Path
import hashlib
import json
import zipfile
import argparse

ROOT = Path(__file__).resolve().parents[1]
EXPECTED = {
    'maincpu': 'f8c0a2ef4105769c627b7bbf13d0844ada8bbe43c00d177eb90dd395d1e3a1e5',
    'gfx': '2718b9527bbd9bfdb5af4b35275b34dd8660d529bc1f4eaeb336ddd6589a238a',
    'proms': 'db633922b57b4e033df7b8a7fc710c942bdf486821d3895ca413337ade532667',
}

def prepare(source):
    with zipfile.ZipFile(source) as archive:
        parts = {Path(n).name: archive.read(n) for n in archive.namelist() if not n.endswith('/')}
    swap = lambda b: (b & 252) | ((b >> 1) & 1) | ((b << 1) & 2)
    images = {
        'maincpu': b''.join(parts[n] for n in ['frogger.26', 'frogger.27', 'frsm3.7']) + bytes(4096),
        'gfx': parts['frogger.607'] + bytes(map(swap, parts['frogger.606'])),
        'proms': parts['pr-91.6l'],
        # Sound CPU has crossed D0/D1 on its FIRST 2 KiB (MAME init_frogger).
        'audiocpu': bytes(map(swap, parts['frogger.608'])) + parts['frogger.609'] + parts['frogger.610'],
    }
    report = {'set': 'frogger (Konami, 1981, parent)', 'archive_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
              'parts': {n: {'size': len(b), 'sha256': hashlib.sha256(b).hexdigest()} for n, b in sorted(parts.items())}, 'images': {}}
    for name, data in images.items():
        digest = hashlib.sha256(data).hexdigest()
        if name in EXPECTED and digest != EXPECTED[name]:
            raise ValueError(f'{name}: wrong ROM set: expected {EXPECTED[name]}, got {digest}')
        report['images'][name] = {'size': len(data), 'sha256': digest}
    for dest in [ROOT / 'godot/rom', ROOT / 'reference/assembled', ROOT / 'vendor/arcade-js/games/frogger/rom']:
        dest.mkdir(parents=True, exist_ok=True)
        for name, data in images.items():
            (dest / f'{name}.bin').write_bytes(data)
    (ROOT / 'docs/evidence/rom-manifest.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report['images'], indent=2))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('source', nargs='?', type=Path, default=ROOT / 'reference/frogger.zip')
    prepare(parser.parse_args().source)
