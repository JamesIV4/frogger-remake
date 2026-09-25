"""Copy the pinned, GPL-3.0-only Frogger port and its executable verification tools."""
from pathlib import Path
import hashlib
import json
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'reference/arcade-js'
DEST = ROOT / 'vendor/arcade-js'
PIN = '9387bbe817befacc235cd8f9f4dd56c652172481'
assert subprocess.check_output(['git', '-C', str(SOURCE), 'rev-parse', 'HEAD'], text=True).strip() == PIN
files = []
for folder in ['core', 'boards/frogger', 'games/frogger']:
    for src in (SOURCE / folder).rglob('*'):
        if not src.is_file() or 'rom' in src.parts or 'samples' in src.parts:
            continue
        if src.suffix not in ('.js', '.json', '.mjs', '.py', '.lua'):
            continue
        files.append(src)
for filename in ['LICENSE', 'package.json', 'tools/gen-registry.mjs', 'tools/emit-core.js',
                 'tools/z80_decode.py', 'tools/trace.py', 'tools/stepcheck.py',
                 'tools/hardware.py', 'tools/mame_golden.py', 'tools/scope.py', 'tools/frameio.py',
                 'tools/stateio.py', 'tools/writeio.py', 'tools/pixel_gate.py',
                 'tools/framediff.py', 'tools/statediff.py', 'tools/writediff.py']:
    files.append(SOURCE / filename)
records = []
for src in files:
    rel = src.relative_to(SOURCE)
    dst = DEST / rel
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(src, dst)
    records.append({'path': rel.as_posix(), 'sha256': hashlib.sha256(dst.read_bytes()).hexdigest()})
(DEST / 'UPSTREAM.json').write_text(json.dumps({'repository': 'https://github.com/qarl/arcade-js',
    'commit': PIN, 'license': 'GPL-3.0-only', 'files': records}, indent=2) + '\n')
shutil.copyfile(SOURCE / 'LICENSE', ROOT / 'LICENSE')
print(f'Vendored {len(files)} files from {PIN}')
