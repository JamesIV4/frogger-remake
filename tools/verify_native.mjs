import {readFileSync,writeFileSync} from 'node:fs';
import {Machine} from '../vendor/arcade-js/games/frogger/machine.js';
const m=new Machine(readFileSync('godot/rom/maincpu.bin'));
m.inputTape=[{frame:150,port:0xe000,bits:128,dur:6},{frame:230,port:0xe002,bits:128,dur:6},{frame:340,port:0xe004,bits:16,dur:6}];
m.runFrames(461);
if(m.stoppedBy) throw m.stoppedBy;
const native=readFileSync('docs/evidence/native-frames.bin');
const differences=[];let total=0;
for(let f=1;f<=460;f++) {
 const expected=m.frames[f],actual=native.subarray((f-1)*3328,f*3328);
 for(let off=0;off<3328;off++) if(expected[off]!==actual[off]) {
   total++;
   if(differences.length<20) differences.push({frame:f,address:((off<2048?0x8000+off:off<3072?0xa800+off-2048:0xb000+off-3072)).toString(16),expected:expected[off],actual:actual[off]});
 }
}
const report={frames:460,bytesPerFrame:3328,comparedBytes:460*3328,differentBytes:total,maskedBytes:0,firstDifferences:differences};
writeFileSync('docs/evidence/native-vs-oracle.json',JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify(report,null,2));
if(total)process.exitCode=1;
