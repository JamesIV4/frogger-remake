import {readFileSync,existsSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {ROUTINES} from '../vendor/arcade-js/games/frogger/idiomatic/names.js';
const read=path=>JSON.parse(readFileSync(path,'utf8'));
const main=read('docs/evidence/ghidra-main/inventory.json');
const audio=read('docs/evidence/ghidra-audio/inventory.json');
const manifest=read('docs/evidence/rom-manifest.json');
const compiled=read('docs/evidence/native-generation.json');
const upstream=read('vendor/arcade-js/UPSTREAM.json');
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const failures=[];
// Verify every vendored source, not an agent-authored 'done' marker.
for(const file of upstream.files)if(sha(readFileSync('vendor/arcade-js/'+file.path))!==file.sha256)failures.push('Modified upstream: '+file.path);
const instructions=new Map(readFileSync('docs/evidence/ghidra-main/instructions.tsv','utf8').trim().split('\n').map(line=>{const [a,bytes]=line.split('\t');return [parseInt(a,16),bytes]}));
const mainSource=readFileSync('src/Generated/MainProgram.cpp','utf8');
const inventory=[];
for(const [address,routine] of Object.entries(ROUTINES)){
 const a=Number(address),hex=a.toString(16).padStart(4,'0');
 const seed=main.seeds.find(s=>parseInt(s.address,16)===a);
 const fn=seed&&main.functions.find(f=>f.address===seed.containing_function);
 const ok=Boolean(seed?.instruction&&fn?.decompiled&&existsSync(`docs/evidence/ghidra-main/${fn.address}.c`)&&mainSource.includes(`case 0x${hex}:`));
 if(!ok)failures.push('Unmapped recovered routine '+hex);
 inventory.push({address:'0x'+hex,name:routine.name,ghidraFunction:fn?.address??null,ghidraDecompiled:fn?.decompiled??false,
   firstInstruction:instructions.get(a),nativeSource:'src/Generated/MainProgram.cpp',nativeEntry:'0x'+hex,
   equivalenceFixture:existsSync(`vendor/arcade-js/games/frogger/idiomatic/test/equivalence-${hex}.test.js`)?`equivalence-${hex}.test.js`:'shared fixture',checked:ok});
}
const audioSource=readFileSync('src/Generated/SoundProgram.cpp','utf8');
const audioMap=audio.seeds.map(seed=>{
 const fn=audio.functions.find(f=>f.address===seed.containing_function);
 const ok=Boolean(seed.instruction&&fn?.decompiled&&audioSource.includes(`case 0x${seed.address}:`));
 if(!ok)failures.push('Unmapped sound entry '+seed.address);
 return {...seed,ghidraDecompiled:fn?.decompiled??false,nativeSource:'src/Generated/SoundProgram.cpp',checked:ok};
});
for(const program of compiled){
 const path=`src/Generated/${program.program}.cpp`;
 if(sha(readFileSync(path))!==program.source_sha256)failures.push('Stale generated source '+path);
 const cpu=program.program==='MainProgram'?'maincpu':'audiocpu';
 if(program.rom_sha256!==manifest.images[cpu].sha256)failures.push('ROM mismatch '+cpu);
}
const parity=read('docs/evidence/native-vs-oracle.json');
if(parity.differentBytes!==0||parity.frames<460||parity.maskedBytes!==0)failures.push('Native full-state oracle gate');
const mame=read('docs/evidence/native-vs-mame.json');
if(mame.length!==19||mame.some(f=>f.diff||f.registerDifferences))failures.push('Independent MAME gate');
const assets=read('docs/evidence/asset-validation.json');
const requiredAssets=['frog','lady_frog','turtle','gator','river_gator','fly','snake','otter','log','car','truck','racecar','dozer','sport','board'];
const validated=new Set(assets.map(a=>a.asset));
if(assets.length!==requiredAssets.length||assets.some(a=>!a.passed)||requiredAssets.some(name=>!validated.has(name)))failures.push('Blender export gate');
const lifecycle=read('docs/evidence/lifecycle-tests.json'),presentation=read('docs/evidence/presentation-tests.json');
if(!lifecycle.gameOver||!lifecycle.playerTwo||!lifecycle.nextLevel||lifecycle.timerDeaths!==3)failures.push('Native lifecycle gates');
if(![0,1,2].every(p=>presentation.turtlePhases.includes(p)))failures.push('Native turtle presentation gate');
const report={mainRecoveredRoutines:inventory.length,mainMapped:inventory.filter(r=>r.checked).length,
 ghidraMainCandidates:main.function_count,ghidraMainDecompiled:main.decompiled_count,
 ghidraAudioCandidates:audio.function_count,ghidraAudioDecompiled:audio.decompiled_count,audioRecoveredEntries:audioMap.length,audioMapped:audioMap.filter(x=>x.checked).length,
 nativeReferenceFrames:parity.frames,nativeReferenceComparedBytes:parity.comparedBytes,independentMameFunctions:new Set(mame.map(f=>f.address)).size,independentMameExecutions:mame.length,
 blenderAssets:assets.length,upstreamCommit:upstream.commit,failures,
 limitations:['A decompiler producing C is not a semantic correctness proof. MAME fixtures and whole-state tests are separate gates.',
 'The native translation retains address-level arithmetic and control flow; it is not a handwritten idiomatic C++ rewrite.',
 'Native compilation includes all populated byte offsets, including speculative data decodes. These offsets are not counted as recovered functions.',
 'Modern road and river crocodile collision are intentional optional deviations. PSG analogue output is synthesized, not netlist exact.']};
writeFileSync('docs/evidence/function-map.json',JSON.stringify(inventory,null,2)+'\n');
writeFileSync('docs/evidence/audio-function-map.json',JSON.stringify(audioMap,null,2)+'\n');
writeFileSync('docs/evidence/audit.json',JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify(report,null,2));
if(failures.length)process.exitCode=1;
