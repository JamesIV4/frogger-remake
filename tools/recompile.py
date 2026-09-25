"""Deterministic ahead-of-time Z80 -> C# lowering, not a runtime opcode interpreter.

Every populated byte offset has a compiled entry, including overlapping entries
and computed-jump destinations. Speculative data decodes are NOT counted as
recovered functions by the coverage audit. Unsupported semantics fail generation.
The readable idiomatic JS and Ghidra C remain separate review/test references.
"""
from pathlib import Path
import sys
import re
import json
import hashlib
from collections import Counter
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'vendor/arcade-js/tools'))
from z80_decode import decode

REG16={'af','bc','de','hl','sp','ix','iy'}
REG8={'a','b','c','d','e','f','h','l','i','r','ixh','ixl','iyh','iyl'}
CONDITIONS={'nz':'(F & 64)==0','z':'(F & 64)!=0','nc':'(F & 1)==0','c':'(F & 1)!=0',
            'po':'(F & 4)==0','pe':'(F & 4)!=0','p':'(F & 128)==0','m':'(F & 128)!=0'}
def expr(s):
    return re.sub(r'\b(af|bc|de|hl|sp|ix|iy|a|b|c|d|e|f|h|l|i|r|ixh|ixl|iyh|iyl)\b',lambda m:m[0].upper(),s)
def read(s,wide=False):
    if s.startswith('('):return f'Read{16 if wide else 8}({expr(s[1:-1])})'
    return expr(s)
def write(s,value,wide=False):
    if s.startswith('('):return f'Write{16 if wide else 8}({expr(s[1:-1])}, {value});'
    return f'{expr(s)} = {value};'

def lower(ins):
    text=ins.text.split(';')[0].strip();tokens=text.split(' ',1);op=tokens[0];arg=tokens[1].split(',') if len(tokens)>1 else []
    raw=ins.raw;idx=raw[0] in (0xdd,0xfd);ed=raw[0]==0xed;cb=raw[0]==0xcb or (idx and len(raw)>1 and raw[1]==0xcb)
    extra=4 if idx else 0
    nextpc=f'0x{ins.end&65535:04x}'
    body='';cycles=4+extra;flow=False
    mem=any(a.startswith('(') for a in arg)
    indexed=any(a.startswith('(ix') or a.startswith('(iy') for a in arg)
    if op=='ld':
        dst,src=arg;wide=dst in REG16 or src in REG16
        body=write(dst,read(src,wide),wide)
        if wide:
            if mem:cycles=20 if ed else 16+extra
            elif dst=='sp' and src in ('hl','ix','iy'):cycles=6+extra
            else:cycles=10+extra
        elif mem:
            if indexed:cycles=19
            elif '(0x' in dst or '(0x' in src:cycles=13+extra
            elif src.startswith('0x'):cycles=10+extra
            else:cycles=7+extra
        else:cycles=(7 if src.startswith('0x') else 4)+extra
        if src in ('i','r'):
            body += ' F=(F&1)|Sz(A)|(Iff2?4:0);';cycles=9
        elif dst in ('i','r'):cycles=9
    elif op in ('inc','dec'):
        operand=arg[0];wide=operand in REG16;inc=op=='inc'
        value=f'({read(operand)} {"+" if inc else "-"} 1)' if wide else f'{"Inc8" if inc else "Dec8"}({read(operand)})'
        body=write(operand,value);cycles=(6 if wide else 11 if mem else 4)+extra
        if indexed:cycles=23
    elif op in ('add','adc','sub','sbc','and','xor','or','cp'):
        wide=len(arg)==2 and arg[0] in REG16
        src=arg[-1]
        if wide:
            dest=expr(arg[0]);body=f'{dest} = Add16({dest}, {read(src)});' if op=='add' else f'{op.title()}Hl({read(src)});'
            cycles=15 if ed else 11+extra
        else:
            body=f'{op.title()}({read(src)});';cycles=(7 if mem or src.startswith('0x') else 4)+extra
            if indexed:cycles=19
    elif op in ('jp','jr','call','rst'):
        target=expr(arg[-1].strip('()'));cond=CONDITIONS.get(arg[0]) if len(arg)>1 else None
        call=op in ('call','rst')
        action=(f'Push({nextpc}); ' if call else '')+(f'IndirectTargets.Add({target}); ' if op=='jp' and mem else '')+f'PC={target};'
        if op=='jr':cycles=12
        elif op=='call':cycles=17+extra
        elif op=='rst':cycles=11+extra
        else:cycles=(4 if mem else 10)+extra
        if cond:
            untaken=(7 if op=='jr' else 10)+extra
            body=f'if({cond}) {{ {action} return {cycles}; }} PC={nextpc}; return {untaken};'
        else:body=f'{action} return {cycles};'
        flow=True
    elif op=='djnz':
        body=f'B--; if(B!=0) {{ PC={arg[0]}; return {13+extra}; }} PC={nextpc}; return {8+extra};';flow=True
    elif op in ('ret','reti','retn'):
        action='PC=Pop();'
        if op in ('reti','retn'):action+=' Iff1=Iff2;'
        cycles=14 if ed else 10+extra
        if arg:body=f'if({CONDITIONS[arg[0]]}) {{ {action} return {11+extra}; }} PC={nextpc}; return {5+extra};'
        else:body=f'{action} return {cycles};'
        flow=True
    elif op=='push':body=f'Push({read(arg[0])});';cycles=11+extra
    elif op=='pop':body=write(arg[0],'Pop()');cycles=10+extra
    elif op=='exx':body='(BC, BCAlt)=(BCAlt, BC); (DE, DEAlt)=(DEAlt, DE); (HL, HLAlt)=(HLAlt, HL);'
    elif op=='ex':
        if arg[0]=='af':body='(AF, AFAlt)=(AFAlt, AF);'
        elif arg[0]=='de':body='(DE,HL)=(HL,DE);'
        else:body=f'int temp=Read16(SP); Write16(SP,{expr(arg[1])}); {expr(arg[1])}=temp;';cycles=19+extra
    elif op in ('bit','res','set'):
        n=int(arg[0]);operand=arg[1]
        if op=='bit':body=f'Bit({n},{read(operand)}, {"("+expr(operand[1:-1])+")>>8" if indexed else read(operand)});'
        else:
            value=f'({read(operand)} {"& ~" if op=="res" else "|"} (1<<{n}))'
            body=f'int temp={value}; '+write(operand,'temp')
            if len(arg)>2:body+=write(arg[2],'temp')
        cycles=20 if indexed and op=='bit' else 23 if indexed else 12 if mem and op=='bit' else 15 if mem else 8
    elif op in ('rlc','rrc','rl','rr','sla','sra','sll','srl'):
        mode=['rlc','rrc','rl','rr','sla','sra','sll','srl'].index(op)
        body=f'int temp=Rotate({read(arg[0])},{mode}); '+write(arg[0],'temp')
        if len(arg)>1:body+=write(arg[1],'temp')
        cycles=23 if indexed else 15 if mem else 8
    elif op in ('rlca','rrca','rla','rra'):
        body=f'RotateA({["rlca","rrca","rla","rra"].index(op)});'
    elif op=='daa':body='Daa();'
    elif op=='cpl':body='A=~A; F=(F&0xc5)|0x12|(A&0x28);'
    elif op=='scf':body='F=(F&0xc4)|1|(A&0x28);'
    elif op=='ccf':body='F=(F&0xc4)|((F&1)!=0?16:1)|(A&0x28);'
    elif op=='neg':body='int temp=A; A=0; Sub(temp);';cycles=8
    elif op in ('ldi','ldd','ldir','lddr','cpi','cpd','cpir','cpdr'):
        decrement=op.startswith('ldd') or op.startswith('cpd');compare=op.startswith('cp');repeat=op.endswith('r')
        body=f'Block({str(decrement).lower()},{str(compare).lower()});'
        if repeat:
            condition='BC!=0'+(' && (F&64)==0' if compare else '')
            body+=f' if({condition}) {{ F=(F&~0x28)|(0x{ins.addr>>8:02x}&0x28); PC=0x{ins.addr:04x}; return 21; }}'
        cycles=16
    elif op in ('rld','rrd'):body=f'NibbleRotate({str(op=="rld").lower()});';cycles=18
    elif op=='in':
        if len(arg)==1:body='int temp=PortRead(BC); F=(F&1)|Sz(temp)|Parity(temp);';cycles=12
        elif arg[1]=='(c)':body=write(arg[0],'PortRead(BC)')+f' F=(F&1)|Sz({read(arg[0])})|Parity({read(arg[0])});';cycles=12
        else:body=f'A=PortRead((A<<8)|{arg[1].strip("()")});';cycles=11+extra
    elif op=='out':
        port='BC' if arg[0]=='(c)' else f'(A<<8)|{arg[0].strip("()")}'
        body=f'PortWrite({port},{read(arg[1])});';cycles=12 if ed else 11+extra
    elif op in ('ini','ind','inir','indr','outi','outd','otir','otdr'):
        body=f'BlockIo({str(op.startswith("in")).lower()},{str("d" in op).lower()});'
        if op in ('inir','indr','otir','otdr'):body+=f' if(B!=0) {{ PC=0x{ins.addr:04x}; return 21; }}'
        cycles=16
    elif op=='di':body='Iff1=Iff2=false;'
    elif op=='ei':body='Iff1=Iff2=true; EiDelay=2;'
    elif op=='im':body=f'InterruptMode={arg[0]};';cycles=8
    elif op=='halt':body='Halted=true;'
    elif op in ('nop','defb'):cycles=8 if ed else 4
    else:raise ValueError(f'Unsupported instruction {ins.addr:04x} {ins.text}')
    if not flow:body+=f' PC={nextpc}; return {cycles};'
    return body

def generate(name,romname,length):
    data=(ROOT/f'reference/assembled/{romname}.bin').read_bytes()
    out=ROOT/'godot/Scripts/Generated';out.mkdir(parents=True,exist_ok=True)
    lines=['// Generated by tools/recompile.py from verified user ROM. GPL-3.0-only.',
           '// Every case is statically lowered native C#; no opcode fetch/decode or JS runtime.',
           f'// SHA256 {hashlib.sha256(data).hexdigest()}',
           'namespace FroggerRemake;',f'public sealed partial class {name} : Z80Program {{',
           f'public {name}(IArcadeBus bus) : base(bus) {{ }}',
           'protected override int ExecuteInstruction() { switch (PC >> 8) {']
    for page in range((length+255)//256):lines.append(f'case {page}: return Page{page:02x}();')
    lines+=['default: throw Missing(); } }']
    stats=Counter()
    for page in range((length+255)//256):
        lines.append(f'private int Page{page:02x}() {{ switch (PC) {{')
        for addr in range(page*256,min(length,(page+1)*256)):
            ins=decode(data,addr);body=lower(ins);stats[ins.text.split()[0]]+=1
            lines.append(f'case 0x{addr:04x}: {{ // {ins.hexdump()} : {ins.text}')
            lines.append(body+' }')
        lines.append('default: throw Missing(); } }')
    lines.append('}')
    target=out/(name+'.cs');target.write_text('\n'.join(lines)+'\n',encoding='utf-8',newline='\n')
    return {'program':name,'rom_sha256':hashlib.sha256(data).hexdigest(),'compiled_entry_offsets':length,
        'source_sha256':hashlib.sha256(target.read_bytes()).hexdigest(),'instruction_forms':dict(stats),
        'note':'All byte offsets compiled, including data. This is not the recovered-function coverage metric.'}

report=[generate('MainProgram','maincpu',0x3000),generate('SoundProgram','audiocpu',0x1800)]
(ROOT/'docs/evidence/native-generation.json').write_text(json.dumps(report,indent=2)+'\n')
print('Generated native C# for both CPUs; every populated address, no interpreter fallback.')
