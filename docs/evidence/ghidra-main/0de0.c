
/* WARNING: Instruction at (ram,0x0ef8) overlaps instruction at (ram,0x0ef7)
    */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

code stampAttractDemoCell(undefined *param_1)

{
  bool bVar1;
  byte bVar2;
  undefined1 uVar3;
  code cVar4;
  char cVar5;
  code cVar6;
  char cVar7;
  code *pcVar8;
  code *UNRECOVERED_JUMPTABLE;
  undefined1 *puVar9;
  char *pcVar10;
  
code_r0x0de0:
  DAT_ram_800d = 3;
  DAT_ram_800f = 3;
  DAT_ram_83bc = (code)((char)DAT_ram_83bc + -1);
  if (DAT_ram_83bc != (code)0x0) {
    return DAT_ram_83bc;
  }
  DAT_ram_83bc = (code)0x20;
  cVar4 = (code)((char)DAT_ram_83d7 * '\x02');
  bVar1 = cVar4 == (code)0x0;
  pcVar8 = (code *)(ushort)(byte)cVar4;
  UNRECOVERED_JUMPTABLE = (code *)0xdff;
switchD_ram_0e00_caseD_dff:
  UNRECOVERED_JUMPTABLE = UNRECOVERED_JUMPTABLE + (short)pcVar8;
  cVar5 = (char)param_1;
  bVar2 = (byte)((ushort)param_1 >> 8);
  cVar7 = (char)((ushort)UNRECOVERED_JUMPTABLE >> 8);
  switch(UNRECOVERED_JUMPTABLE) {
  case (code *)0xdff:
    goto switchD_ram_0e00_caseD_dff;
  case (code *)0xe01:
    cVar4 = (code)dispatch_0e49();
    return cVar4;
  case (code *)0xe03:
    cVar4 = (code)dispatch_0e3f();
    return cVar4;
  case (code *)0xe05:
    cVar4 = (code)dispatch_0e35();
    return cVar4;
  case (code *)0xe07:
    cVar4 = (code)dispatch_0e2b();
    return cVar4;
  case (code *)0xe09:
    cVar4 = (code)dispatch_0e21();
    return cVar4;
  case (code *)0xe0b:
    cVar4 = (code)dispatch_0e17();
    return cVar4;
  case dispatch_0e0d:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_ab06;
    goto code_r0x0e10;
  case dispatch_0e0d:
code_r0x0e10:
    pcVar8 = (code *)&DAT_ram_8040;
  case dispatch_0e0d:
switchD_ram_0e00_caseD_e13:
    cVar4 = (code)0xd4;
switchD_ram_0e00_caseD_e15:
switchD_ram_0e00_caseD_e51:
    param_1 = (undefined *)0x1f;
switchD_ram_0e00_caseD_e54:
    *UNRECOVERED_JUMPTABLE = cVar4;
switchD_ram_0e00_caseD_e55:
    cVar4 = (code)((char)cVar4 + 1);
    UNRECOVERED_JUMPTABLE =
         (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                          (char)UNRECOVERED_JUMPTABLE + '\x01');
switchD_ram_0e00_caseD_e57:
    *UNRECOVERED_JUMPTABLE = cVar4;
    cVar4 = (code)((char)cVar4 + 1);
switchD_ram_0e00_caseD_e59:
    UNRECOVERED_JUMPTABLE = UNRECOVERED_JUMPTABLE + (short)param_1;
    *UNRECOVERED_JUMPTABLE = cVar4;
switchD_ram_0e00_caseD_e5b:
    cVar4 = (code)((char)cVar4 + 1);
    UNRECOVERED_JUMPTABLE =
         (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                          (char)UNRECOVERED_JUMPTABLE + '\x01');
switchD_ram_0e00_caseD_e5d:
    *UNRECOVERED_JUMPTABLE = cVar4;
    UNRECOVERED_JUMPTABLE = pcVar8;
switchD_ram_0e00_caseD_e5f:
    param_1 = (undefined *)0x400;
LAB_ram_0e62:
    do {
      *UNRECOVERED_JUMPTABLE = SUB21(param_1,0);
switchD_ram_0e00_caseD_e63:
      UNRECOVERED_JUMPTABLE =
           (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                            (char)UNRECOVERED_JUMPTABLE + '\x01');
      cVar5 = (char)((ushort)param_1 >> 8) + -1;
      param_1 = (undefined *)CONCAT11(cVar5,(char)param_1);
    } while (cVar5 != '\0');
    UNRECOVERED_JUMPTABLE = DAT_ram_83d7;
switchD_ram_0e00_caseD_e69:
    cVar6 = *UNRECOVERED_JUMPTABLE;
    *UNRECOVERED_JUMPTABLE = (code)((char)cVar6 - 1U);
    if ((code)((char)cVar6 - 1U) != (code)0x0) {
      return cVar4;
    }
switchD_ram_0e00_caseD_e6b:
    *UNRECOVERED_JUMPTABLE = (code)0x7;
switchD_ram_0e00_caseD_e6d:
    cVar4 = (code)0x0;
    DAT_ram_83bf = '\0';
switchD_ram_0e00_caseD_e71:
    DAT_ram_83bb = cVar4;
setAttractIdleMode:
    cVar4 = (code)0x5;
    DAT_ram_83d6 = 5;
switchD_ram_0e00_caseD_e79:
    return cVar4;
  case dispatch_0e0d:
    goto switchD_ram_0e00_caseD_e13;
  case (code *)0xe15:
    goto switchD_ram_0e00_caseD_e15;
  case dispatch_0e17:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_aaa6;
    goto code_r0x0e1a;
  case dispatch_0e17:
code_r0x0e1a:
    pcVar8 = (code *)&DAT_ram_8044;
  case dispatch_0e17:
switchD_ram_0e00_caseD_e1d:
    cVar4 = (code)0xd8;
switchD_ram_0e00_caseD_e1f:
    goto switchD_ram_0e00_caseD_e51;
  case dispatch_0e17:
    goto switchD_ram_0e00_caseD_e1d;
  case (code *)0xe1f:
    goto switchD_ram_0e00_caseD_e1f;
  case dispatch_0e21:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_aa46;
    goto code_r0x0e24;
  case dispatch_0e21:
code_r0x0e24:
    pcVar8 = (code *)&DAT_ram_8048;
  case dispatch_0e21:
switchD_ram_0e00_caseD_e27:
    cVar4 = (code)0xdc;
switchD_ram_0e00_caseD_e29:
    goto switchD_ram_0e00_caseD_e51;
  case dispatch_0e21:
    goto switchD_ram_0e00_caseD_e27;
  case (code *)0xe29:
    goto switchD_ram_0e00_caseD_e29;
  case dispatch_0e2b:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_a9e6;
    goto code_r0x0e2e;
  case dispatch_0e2b:
code_r0x0e2e:
    pcVar8 = (code *)&DAT_ram_804c;
  case dispatch_0e2b:
switchD_ram_0e00_caseD_e31:
    cVar4 = (code)0xf4;
switchD_ram_0e00_caseD_e33:
    goto switchD_ram_0e00_caseD_e51;
  case dispatch_0e2b:
    goto switchD_ram_0e00_caseD_e31;
  case (code *)0xe33:
    goto switchD_ram_0e00_caseD_e33;
  case dispatch_0e35:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_a986;
    goto code_r0x0e38;
  case dispatch_0e35:
code_r0x0e38:
    pcVar8 = (code *)&DAT_ram_8050;
  case dispatch_0e35:
switchD_ram_0e00_caseD_e3b:
    cVar4 = (code)0xf4;
switchD_ram_0e00_caseD_e3d:
    goto switchD_ram_0e00_caseD_e51;
  case dispatch_0e35:
    pcVar8 = (code *)CONCAT11(bVar2,cVar4);
    goto switchD_ram_0e00_caseD_e3b;
  case (code *)0xe3d:
    goto switchD_ram_0e00_caseD_e3d;
  case dispatch_0e3f:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_a926;
    goto code_r0x0e42;
  case dispatch_0e3f:
code_r0x0e42:
    pcVar8 = (code *)&UNK_ram_8054;
  case dispatch_0e3f:
switchD_ram_0e00_caseD_e45:
    cVar4 = (code)0xf8;
switchD_ram_0e00_caseD_e47:
    goto switchD_ram_0e00_caseD_e51;
  case dispatch_0e3f:
    pcVar8 = (code *)CONCAT11(cVar7,cVar4);
    goto switchD_ram_0e00_caseD_e45;
  case (code *)0xe47:
    goto switchD_ram_0e00_caseD_e47;
  case dispatch_0e49:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_a8c6;
    goto code_r0x0e4c;
  case (code *)0xe4b:
code_r0x0e4c:
    pcVar8 = (code *)&DAT_ram_8058;
  case (code *)0xe4f:
switchD_ram_0e00_caseD_e4f:
    cVar4 = (code)0xd8;
    goto switchD_ram_0e00_caseD_e51;
  case (code *)0xe4d:
    pcVar8 = (code *)(ushort)bVar2;
    goto switchD_ram_0e00_caseD_e4f;
  case (code *)0xe51:
    goto switchD_ram_0e00_caseD_e51;
  case (code *)0xe54:
    goto switchD_ram_0e00_caseD_e54;
  case (code *)0xe55:
    goto switchD_ram_0e00_caseD_e55;
  case (code *)0xe57:
    goto switchD_ram_0e00_caseD_e57;
  case (code *)0xe59:
    goto switchD_ram_0e00_caseD_e59;
  case (code *)0xe5b:
    goto switchD_ram_0e00_caseD_e5b;
  case (code *)0xe5d:
    goto switchD_ram_0e00_caseD_e5d;
  case (code *)0xe5f:
    goto switchD_ram_0e00_caseD_e5f;
  case (code *)0xe61:
    param_1 = (undefined *)CONCAT11(bVar2 + 1,cVar5);
    goto LAB_ram_0e62;
  case (code *)0xe63:
    goto switchD_ram_0e00_caseD_e63;
  case (code *)0xe65:
    cVar6 = cVar4;
    if ((char)cVar4 < '\0') {
      *(undefined2 *)((short)register0x44 + -2) = 0xe68;
      cVar4 = (code)func_0xd721();
      cVar6 = SUB21(pcVar8,0);
    }
    goto code_r0x0e68;
  case (code *)0xe67:
    *(undefined2 *)((short)register0x44 + -2) = 0xe68;
    cVar4 = (code)func_0x0010();
    cVar6 = SUB21(pcVar8,0);
code_r0x0e68:
    cVar4 = (code)((char)cVar4 + (char)cVar6);
  case (code *)0xe69:
    goto switchD_ram_0e00_caseD_e69;
  case (code *)0xe6b:
    goto switchD_ram_0e00_caseD_e6b;
  case (code *)0xe6d:
    goto switchD_ram_0e00_caseD_e6d;
  case (code *)0xe6f:
    cVar4 = (code)((char)DAT_ram_83d7 << 2);
  case (code *)0xe71:
    goto switchD_ram_0e00_caseD_e71;
  case (code *)0xe73:
    goto setAttractIdleMode;
  case setAttractIdleMode:
    goto setAttractIdleMode;
  case setAttractIdleMode:
    DAT_ram_800d = 3;
    DAT_ram_800f = 3;
    DAT_ram_83bc = (code)0x20;
    return (code)((char)cVar4 + 0x7d);
  case (code *)0xe79:
    goto switchD_ram_0e00_caseD_e79;
  case (code *)0xe7b:
    register0x44 = (BADSPACEBASE *)((short)register0x44 + 2);
    cVar4 = (code)((char)DAT_ram_83d7 << 2);
  case (code *)0xe7d:
    if (cVar4 != (code)0x0) goto setAttractIdleMode;
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_83bf;
    break;
  case (code *)0xe7f:
    if ((char)cVar4 >= '\0') {
      *(undefined2 *)((short)register0x44 + -2) = 0xe82;
      func_0xbf21();
    }
    break;
  case (code *)0xe81:
    break;
  case (code *)0xe85:
    goto switchD_ram_0e00_caseD_e85;
  case (code *)0xe87:
    goto switchD_ram_0e00_caseD_e87;
  case (code *)0xe89:
    bVar2 = (char)DAT_ram_83d7 << 2 | ((byte)DAT_ram_83d7 & 0x7f) >> 6;
    goto code_r0x0e8a;
  case (code *)0xe8b:
    param_1 = (undefined *)0x2100;
  case (code *)0xe8f:
    cVar4 = (code)((char)cVar4 + (char)((ushort)param_1 >> 8));
    goto code_r0x0e90;
  case (code *)0xe8d:
    goto switchD_ram_0e00_caseD_e8d;
  case (code *)0xe91:
    param_1 = param_1 + 1;
    cVar4 = (code)((char)DAT_ram_83d7 << 2 | ((byte)DAT_ram_83d7 & 0x7f) >> 6);
  case (code *)0xe93:
    goto switchD_ram_0e00_caseD_e93;
  case (code *)0xe95:
    cVar4 = (code)((char)cVar4 + cVar5);
    goto LAB_ram_0e96;
  case (code *)0xe97:
    goto switchD_ram_0e00_caseD_e97;
  case (code *)0xe99:
    goto switchD_ram_0e00_caseD_e99;
  case (code *)0xe9b:
    goto switchD_ram_0e00_caseD_e9b;
  case (code *)0xe9d:
    goto switchD_ram_0e00_caseD_e9d;
  case (code *)0xe9f:
    goto switchD_ram_0e00_caseD_e9f;
  case (code *)0xea1:
    _DAT_ram_83bd = UNRECOVERED_JUMPTABLE;
    goto switchD_ram_0e00_caseD_ea5;
  case (code *)0xea3:
    cVar4 = (code)((char)DAT_ram_83d7 << 2);
  case (code *)0xea5:
    goto switchD_ram_0e00_caseD_ea5;
  case (code *)0xea7:
    cVar4 = (code)((char)DAT_ram_83d7 << 2);
    goto switchD_ram_0e00_caseD_ea5;
  case (code *)0xea9:
    cVar4 = (code)((char)DAT_ram_83d7 << 2 | ((byte)DAT_ram_83d7 & 0x7f) >> 6);
    goto switchD_ram_0e00_caseD_ea5;
  case (code *)0xeab:
    cVar4 = (code)((char)DAT_ram_83d7 << 2);
  case (code *)0xead:
    goto switchD_ram_0e00_caseD_ead;
  case (code *)0xeaf:
    goto switchD_ram_0e00_caseD_eaf;
  case (code *)0xeb1:
    cVar4 = (code)((char)DAT_ram_83d7 << 2);
    goto switchD_ram_0e00_caseD_eaf;
  case (code *)0xeb3:
    goto switchD_ram_0e00_caseD_eb3;
  case (code *)0xeb5:
    goto switchD_ram_0e00_caseD_eb5;
  case (code *)0xeb7:
    goto switchD_ram_0e00_caseD_eb7;
  case (code *)0xeb9:
    bVar2 = (char)DAT_ram_83d7 << 2;
    goto code_r0x0eba;
  case (code *)0xebb:
    goto switchD_ram_0e00_caseD_ebb;
  case (code *)0xebd:
    goto switchD_ram_0e00_caseD_ebd;
  case (code *)0xebf:
    goto switchD_ram_0ec2_switchD;
  case (code *)0xec1:
    goto switchD_ram_0ec2_caseD_ec1;
  case dispatch_0ec3:
  case (code *)0xef9:
switchD_ram_0ec2_caseD_ef9:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8058;
code_r0x0efc:
    cVar6 = (code)0xc1;
LAB_ram_0efe:
    *(undefined2 *)((short)register0x44 + -2) = 0xf01;
    uVar3 = FUN_ram_0f3e(cVar4);
    *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE - 1);
    *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE - 1);
    *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE - 1);
    *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE - 1);
    cVar4 = *UNRECOVERED_JUMPTABLE;
    puVar9 = (undefined1 *)
             CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                      (char)UNRECOVERED_JUMPTABLE + '\x01');
    *puVar9 = uVar3;
    if ((byte)cVar6 <= (byte)cVar4) {
      return cVar4;
    }
    *puVar9 = 0x1e;
    DAT_ram_83d7 = (code)((char)DAT_ram_83d7 - 1);
    if (DAT_ram_83d7 != (code)0x0) {
      return cVar4;
    }
    DAT_ram_83d7 = (code)0x14;
    goto switchD_ram_0e00_caseD_eaf;
  case dispatch_0ec5:
    goto FUN_ram_0ef2;
  case dispatch_0ec7:
  case caseD_eeb:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8050;
code_r0x0eee:
    param_1 = &UNK_ram_9100;
code_r0x0ef0:
    cVar6 = SUB21((ushort)param_1 >> 8,0);
    goto LAB_ram_0efe;
  case dispatch_0ec9:
    goto FUN_ram_0ee4;
  case dispatch_0ecb:
  case caseD_edd:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8048;
code_r0x0ee0:
    param_1 = &UNK_ram_6100;
code_r0x0ee2:
    cVar6 = SUB21((ushort)param_1 >> 8,0);
    goto LAB_ram_0efe;
  case dispatch_0ecd:
    goto FUN_ram_0ed6;
  case dispatch_0ecf:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8040;
    goto code_r0x0ed2;
  case dispatch_0ecf:
    cVar4 = (code)((char)cVar4 + bVar2);
code_r0x0ed2:
    cVar6 = (code)0x31;
    goto LAB_ram_0efe;
  case dispatch_0ecf:
    register0x44 = (BADSPACEBASE *)0x2818;
FUN_ram_0ed6:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8044;
  case FUN_ram_0ed6:
switchD_ram_0ec2_caseD_ed9:
    param_1 = &UNK_ram_4900;
switchD_ram_0ec2_caseD_edb:
    cVar6 = SUB21((ushort)param_1 >> 8,0);
    goto LAB_ram_0efe;
  case dispatch_0ecf:
    if (!bVar1) goto switchD_ram_0ec2_caseD_ed7;
    goto switchD_ram_0ec2_caseD_ef9;
  case (code *)0xed7:
switchD_ram_0ec2_caseD_ed7:
    cVar4 = (code)((char)cVar4 + cVar7);
    goto switchD_ram_0ec2_caseD_ed9;
  case (code *)0xedb:
    goto switchD_ram_0ec2_caseD_edb;
  case caseD_edd:
    cVar4 = (code)((char)cVar4 + bVar2);
    goto code_r0x0ee0;
  case caseD_edd:
    UNRECOVERED_JUMPTABLE = (code *)CONCAT11(cVar5,(char)UNRECOVERED_JUMPTABLE);
    goto code_r0x0ee2;
  case caseD_edd:
    cVar4 = *pcVar8;
FUN_ram_0ee4:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_804c;
  case FUN_ram_0ee4:
switchD_ram_0ec2_caseD_ee7:
    param_1 = &UNK_ram_7900;
switchD_ram_0ec2_caseD_ee9:
    cVar6 = SUB21((ushort)param_1 >> 8,0);
    goto LAB_ram_0efe;
  case (code *)0xee5:
    cVar4 = (code)((char)cVar4 + bVar2);
    goto switchD_ram_0ec2_caseD_ee7;
  case (code *)0xee9:
    goto switchD_ram_0ec2_caseD_ee9;
  case caseD_eeb:
    cVar4 = (code)((char)cVar4 + bVar2);
    goto code_r0x0eee;
  case caseD_eeb:
    cVar4 = (code)((char)cVar4 - cVar5);
    goto code_r0x0ef0;
  case caseD_eeb:
FUN_ram_0ef2:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_8054;
  case FUN_ram_0ef2:
switchD_ram_0ec2_caseD_ef5:
    param_1 = &UNK_ram_a900;
switchD_ram_0ec2_caseD_ef7:
    cVar6 = SUB21((ushort)param_1 >> 8,0);
    goto LAB_ram_0efe;
  case (code *)0xef3:
    cVar4 = (code)((char)cVar4 + bVar2);
    goto switchD_ram_0ec2_caseD_ef5;
  case (code *)0xef7:
    goto switchD_ram_0ec2_caseD_ef7;
  case (code *)0xefb:
    cVar4 = (code)((char)cVar4 + bVar2);
    goto code_r0x0efc;
  case (code *)0xefd:
    cVar6 = SUB21((ushort)*(undefined2 *)register0x44 >> 8,0);
    register0x44 = (BADSPACEBASE *)((short)register0x44 + 2);
    goto LAB_ram_0efe;
  }
  cVar4 = *UNRECOVERED_JUMPTABLE;
  bVar1 = cVar4 == (code)0x0;
switchD_ram_0e00_caseD_e85:
  if (!bVar1) {
    cVar4 = (code)((char)cVar4 - 1);
    bVar1 = cVar4 == (code)0x0;
switchD_ram_0e00_caseD_eb5:
    if (bVar1) {
switchD_ram_0e00_caseD_eb7:
      bVar2 = (byte)DAT_ram_83d7;
code_r0x0eba:
      cVar4 = (code)(bVar2 * '\x02');
switchD_ram_0e00_caseD_ebb:
switchD_ram_0e00_caseD_ebd:
      pcVar8 = (code *)(ushort)(byte)cVar4;
      UNRECOVERED_JUMPTABLE = (code *)0xec1;
switchD_ram_0ec2_caseD_ec1:
      UNRECOVERED_JUMPTABLE = UNRECOVERED_JUMPTABLE + (short)pcVar8;
switchD_ram_0ec2_switchD:
                    /* WARNING: Could not recover jumptable at 0x0ec2. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      cVar4 = (code)(*UNRECOVERED_JUMPTABLE)();
      return cVar4;
    }
    if (cVar4 == (code)0x1) goto code_r0x0f1a;
    goto code_r0x0de0;
  }
switchD_ram_0e00_caseD_e87:
  *(undefined2 *)((short)register0x44 + -2) = 0xe8a;
  bVar2 = fillTilemapBlock28x32();
code_r0x0e8a:
  *(undefined2 *)((short)register0x44 + -2) = 0xe8d;
  cVar4 = (code)clearObjectBlocksAndMirrorToObjRam(bVar2);
switchD_ram_0e00_caseD_e8d:
  UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8040;
code_r0x0e90:
  param_1 = (undefined *)0x703;
switchD_ram_0e00_caseD_e93:
  pcVar8 = (code *)&switchD_ram:14c6::caseD_1a;
LAB_ram_0e96:
  do {
    *UNRECOVERED_JUMPTABLE = SUB21(pcVar8,0);
switchD_ram_0e00_caseD_e97:
    UNRECOVERED_JUMPTABLE =
         (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                          (char)UNRECOVERED_JUMPTABLE + '\x02');
switchD_ram_0e00_caseD_e99:
    *UNRECOVERED_JUMPTABLE = SUB21(param_1,0);
    UNRECOVERED_JUMPTABLE =
         (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                          (char)UNRECOVERED_JUMPTABLE + '\x01');
switchD_ram_0e00_caseD_e9b:
    *UNRECOVERED_JUMPTABLE = SUB21((ushort)pcVar8 >> 8,0);
    UNRECOVERED_JUMPTABLE =
         (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                          (char)UNRECOVERED_JUMPTABLE + '\x01');
switchD_ram_0e00_caseD_e9d:
    cVar5 = (char)((ushort)param_1 >> 8) + -1;
    param_1 = (undefined *)CONCAT11(cVar5,(char)param_1);
  } while (cVar5 != '\0');
switchD_ram_0e00_caseD_e9f:
  _DAT_ram_83bd = (code *)0x504;
switchD_ram_0e00_caseD_ea5:
  DAT_ram_83d7 = (code)0x7;
  UNRECOVERED_JUMPTABLE = DAT_ram_83bc;
switchD_ram_0e00_caseD_ead:
  *UNRECOVERED_JUMPTABLE = (code)0x20;
switchD_ram_0e00_caseD_eaf:
  DAT_ram_83bf = DAT_ram_83bf + '\x01';
switchD_ram_0e00_caseD_eb3:
  return cVar4;
code_r0x0f1a:
  *(undefined2 *)((short)register0x44 + -2) = 0xf1d;
  cVar5 = FUN_ram_0f3e(0);
  cVar4 = (code)0x0;
  if (DAT_ram_83d7 != (code)0x0) {
    cVar7 = '\a';
    pcVar10 = &DAT_ram_8043;
    do {
      *pcVar10 = *pcVar10 + -1;
      *pcVar10 = *pcVar10 + -1;
      *pcVar10 = *pcVar10 + -1;
      *pcVar10 = *pcVar10 + -1;
      pcVar10 = (char *)CONCAT11((char)((ushort)pcVar10 >> 8),(char)pcVar10 + -2);
      *pcVar10 = cVar5 + -3;
      pcVar10 = pcVar10 + 6;
      cVar7 = cVar7 + -1;
    } while (cVar7 != '\0');
    DAT_ram_83d7 = (code)((char)DAT_ram_83d7 - 1);
    return DAT_ram_83d7;
  }
  goto switchD_ram_0e00_caseD_ea5;
}

