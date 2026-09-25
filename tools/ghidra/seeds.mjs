import { ROUTINES } from '../../vendor/arcade-js/games/frogger/idiomatic/names.js';
import { readFileSync, writeFileSync } from 'node:fs';
const entries = new Map(Object.entries(ROUTINES).map(([a,r])=>[Number(a),r.name]));
for (const e of JSON.parse(readFileSync('vendor/arcade-js/games/frogger/entrypoints.json'))) {
  const addr = Number(e.addr);
  if (!entries.has(addr)) entries.set(addr, `dispatch_${addr.toString(16).padStart(4,'0')}`);
}
writeFileSync('tools/ghidra/seeds.tsv', [...entries].sort((a,b)=>a[0]-b[0]).map(([a,n])=>`${a.toString(16)}\t${n}`).join('\n'));
console.log(`${entries.size} explicit routine and computed-branch entries`);
