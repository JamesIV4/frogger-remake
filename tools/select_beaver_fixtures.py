"""Retain branch examples from a MAME beaver capture already replayed natively.

Usage: python tools/select_beaver_fixtures.py scratch/beaver-mame
The unabridged captures stay local; the small branch corpus is checked in.
"""
import hashlib
import json
from pathlib import Path
import shutil
import sys
from collections import Counter

source = Path(sys.argv[1])
target = Path('docs/evidence/mame-functions')
replay = json.loads((source / 'native-vs-mame.json').read_text())
captures = sorted(source.glob('*-before.bin'))
assert len(replay) == len(captures) and len(captures) >= 2000
assert all(r['diff'] == 0 and r['registerDifferences'] == 0 for r in replay)
counts = Counter()
selected = {}
for path in captures:
    before = path.read_bytes()
    after = Path(str(path).replace('-before.bin', '-after.bin')).read_bytes()
    active, next_active = before[0x486], after[0x486]
    direction = 'left' if (after if next_active else before)[0x485] else 'right'
    if active == 0:
        kind = 'spawn' if next_active else 'idle'
    elif next_active == 0:
        kind = 'retire'
    elif abs(before[0x58] - after[0x58]) > 128:
        kind = 'wrap'
    elif before[0x482] != after[0x482]:
        kind = 'step'
    else:
        kind = 'wait'
    key = kind + ('-' + direction if kind != 'idle' else '')
    counts[key] += 1
    if key in selected:
        continue
    selected[key] = path.name.removesuffix('-before.bin')
    for suffix in ['-before.bin', '-before.txt', '-after.bin', '-after.txt']:
        shutil.copyfile(source / (selected[key] + suffix), target / ('2b83-beaver-' + key + suffix))
assert len(selected) == 11, selected
report = {
    'source': 'MAME executing reference/frogger.zip via tools/mame_beaver.lua',
    'romSha256': hashlib.sha256(Path('godot/rom/maincpu.bin').read_bytes()).hexdigest(),
    'dispatcher': '0x2b83', 'executions': len(replay),
    'comparedBytes': sum(r['bytes'] for r in replay),
    'differentBytes': 0, 'registerDifferences': 0,
    'branches': dict(sorted(counts.items())), 'retainedCaptures': dict(sorted(selected.items())),
    'scope': 'Level 3, frog held on bank, timer refreshed; native RNG, lane motion, spawning, steering and retirement. Does not exercise player-hit branches.',
}
Path('docs/evidence/beaver-native-replay.json').write_text(json.dumps(report, indent=2) + '\n')
print(json.dumps(report, indent=2))
