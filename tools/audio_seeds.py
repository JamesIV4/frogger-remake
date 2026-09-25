from pathlib import Path
import json,sys
root=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(root/'vendor/arcade-js/tools'))
from z80_decode import decode
data=(root/'reference/assembled/audiocpu.bin').read_bytes()
trace=json.loads((root/'docs/evidence/sound-execution.json').read_text())
executed=set(trace['executed']);entries={0,0x38}|set(trace['indirectTargets'])
for pc in executed:
    ins=decode(data,pc)
    if ins.kind in ('call','call_cond','rst') and ins.target in executed:entries.add(ins.target)
rows=[f'{a:04x}\taudio_{a:04x}' for a in sorted(entries)]
(root/'tools/ghidra/seeds-audio.tsv').write_text('\n'.join(rows)+'\n')
print(f'{len(entries)} sound routine entries from executed calls and computed targets, {len(executed)} reached instruction addresses')
