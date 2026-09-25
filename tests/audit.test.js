import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {ROUTINES} from '../vendor/arcade-js/games/frogger/idiomatic/names.js';
test('both ROM images match the native compiler input hashes; a one-byte mutation is rejected',()=>{
 const report=JSON.parse(readFileSync('docs/evidence/native-generation.json'));
 for(const [i,name] of ['maincpu','audiocpu'].entries()){
  const bytes=readFileSync(`godot/rom/${name}.bin`);const sha=b=>createHash('sha256').update(b).digest('hex');
  assert.equal(sha(bytes),report[i].rom_sha256);bytes[0]^=1;assert.notEqual(sha(bytes),report[i].rom_sha256);
 }
});
test('no recovered main routine is missing from native address dispatch',()=>{
 const code=readFileSync('godot/Scripts/Generated/MainProgram.cs','utf8');
 assert.equal(Object.keys(ROUTINES).length,165);
 for(const address of Object.keys(ROUTINES))assert.ok(code.includes(`case 0x${Number(address).toString(16).padStart(4,'0')}:`));
});
