"""Compose each backdrop asset's four Blender review views into one sheet.

Run with the system Python (Pillow) after art/scripts/build_scenery.py:
hero, front, side and game-camera views side by side, then the single views
are removed so docs/evidence/models keeps one selected sheet per asset.
"""
from pathlib import Path
import json
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
EVIDENCE = ROOT / 'docs/evidence/models'
VIEWS = ['hero', 'front', 'side', 'game']

for record in json.loads((ROOT / 'art/scenery.json').read_text()):
    name = record['asset']
    paths = [EVIDENCE / f'scenery-{name}-{view}.png' for view in VIEWS]
    if not all(p.exists() for p in paths):
        continue
    views = [Image.open(p).convert('RGB') for p in paths]
    sheet = Image.new('RGB', (sum(v.width for v in views), max(v.height for v in views)), 'white')
    x = 0
    for view in views:
        sheet.paste(view, (x, 0))
        x += view.width
    sheet.save(EVIDENCE / f'scenery-{name}-sheet.png', optimize=True)
    for p in paths:
        p.unlink()
print('SCENERY_SHEETS_COMPLETE')
