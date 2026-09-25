
/* WARNING: Instruction at (ram,0x0fe5) overlaps instruction at (ram,0x0fe2)
    */
/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Removing unreachable block (ram,0x0fc9) */
/* WARNING: Removing unreachable block (ram,0x0fcb) */
/* WARNING: Removing unreachable block (ram,0x0fe5) */
/* WARNING: Removing unreachable block (ram,0x0fcd) */
/* WARNING: Removing unreachable block (ram,0x0ffd) */
/* WARNING: Removing unreachable block (ram,0x1008) */
/* WARNING: Removing unreachable block (ram,0x100f) */
/* WARNING: Removing unreachable block (ram,0x101e) */
/* WARNING: Removing unreachable block (ram,0x1029) */
/* WARNING: Removing unreachable block (ram,0x1033) */
/* WARNING: Removing unreachable block (ram,0x1038) */
/* WARNING: Removing unreachable block (ram,0x0fc5) */
/* WARNING: Removing unreachable block (ram,0x0fc7) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

byte driveAttractDemoSequencer(code *param_1,short param_2)

{
  bool bVar1;
  ushort uVar2;
  byte bVar3;
  undefined1 in_F;
  ushort uVar5;
  byte bVar7;
  code cVar8;
  byte bVar9;
  undefined *puVar6;
  byte bVar11;
  char cVar12;
  undefined2 uVar10;
  char cVar13;
  code *pcVar14;
  code *pcVar15;
  code *UNRECOVERED_JUMPTABLE;
  undefined2 *puVar16;
  undefined2 *puVar17;
  undefined2 *puVar18;
  undefined2 *puVar19;
  byte bVar4;
  
  puVar6 = (undefined *)CONCAT11(DAT_ram_83e1,in_F);
switchD_ram_0e00_caseD_e7d:
  uVar5 = (ushort)puVar6 >> 8;
  puVar6 = (undefined *)(ushort)((byte)puVar6 & 0x28);
  if ((char)uVar5 == '\0') {
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_83bf;
switchD_ram_0e00_caseD_e83:
    puVar6 = (undefined *)
             (CONCAT11(*UNRECOVERED_JUMPTABLE,
                       (byte)puVar6 & 0xae | (*UNRECOVERED_JUMPTABLE == (code)0x0) << 6) & 0xff7b);
switchD_ram_0e00_caseD_e85:
    if (((byte)puVar6 >> 6 & 1) == 0) {
      cVar12 = (char)((ushort)puVar6 >> 8) + -1;
      puVar6 = (undefined *)(CONCAT11(cVar12,(byte)puVar6 & 0xab | (cVar12 == '\0') << 6) & 0xff7f);
switchD_ram_0e00_caseD_eb5:
      if (((byte)puVar6 >> 6 & 1) == 0) goto LAB_ram_0f16;
switchD_ram_0e00_caseD_eb7:
      cVar12 = DAT_ram_83d7 * '\x02';
      puVar6 = (undefined *)
               CONCAT11(cVar12,(byte)puVar6 & 0x28 | SCARRY1(DAT_ram_83d7,DAT_ram_83d7) << 2 |
                               (cVar12 == '\0') << 6 | (cVar12 < '\0') << 7);
switchD_ram_0e00_caseD_ebb:
      goto switchD_ram_0e00_caseD_ebd;
    }
switchD_ram_0e00_caseD_e87:
    *(undefined2 *)((short)register0x44 + -2) = 0xe8a;
    fillTilemapBlock28x32();
    *(undefined2 *)((short)register0x44 + -2) = 0xe8d;
    bVar9 = clearObjectBlocksAndMirrorToObjRam();
    puVar6 = (undefined *)((ushort)bVar9 << 8);
switchD_ram_0e00_caseD_e8d:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8040;
    param_1 = (code *)0x703;
switchD_ram_0e00_caseD_e93:
    pcVar14 = (code *)&switchD_ram:14c6::caseD_1a;
LAB_ram_0e96:
    do {
      *UNRECOVERED_JUMPTABLE = SUB21(pcVar14,0);
switchD_ram_0e00_caseD_e97:
      UNRECOVERED_JUMPTABLE =
           (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                            (char)UNRECOVERED_JUMPTABLE + '\x02');
      puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
switchD_ram_0e00_caseD_e99:
      *UNRECOVERED_JUMPTABLE = SUB21(param_1,0);
      UNRECOVERED_JUMPTABLE =
           (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                            (char)UNRECOVERED_JUMPTABLE + '\x01');
      puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
switchD_ram_0e00_caseD_e9b:
      *UNRECOVERED_JUMPTABLE = SUB21((ushort)pcVar14 >> 8,0);
      UNRECOVERED_JUMPTABLE =
           (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                            (char)UNRECOVERED_JUMPTABLE + '\x01');
      puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
switchD_ram_0e00_caseD_e9d:
      cVar12 = (char)((ushort)param_1 >> 8) + -1;
      param_1 = (code *)CONCAT11(cVar12,(char)param_1);
    } while (cVar12 != '\0');
switchD_ram_0e00_caseD_e9f:
    _DAT_ram_83bd = 0x504;
switchD_ram_0e00_caseD_ea5:
    DAT_ram_83d7 = 7;
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_83bc;
switchD_ram_0e00_caseD_ead:
    *UNRECOVERED_JUMPTABLE = (code)0x20;
switchD_ram_0e00_caseD_eaf:
    DAT_ram_83bf = DAT_ram_83bf + '\x01';
    puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
switchD_ram_0e00_caseD_eb3:
    return (byte)((ushort)puVar6 >> 8);
  }
  goto setAttractIdleMode;
switchD_ram_0e00_caseD_ebd:
  pcVar14 = (code *)((ushort)puVar6 >> 8);
  UNRECOVERED_JUMPTABLE = (code *)0xec1;
switchD_ram_0ec2_caseD_ec1:
  puVar6 = (undefined *)
           CONCAT11((char)((ushort)puVar6 >> 8),
                    (byte)puVar6 & 0xee |
                    (((ushort)(pcVar14 + ((ushort)UNRECOVERED_JUMPTABLE & 0xfff)) & 0x1000) != 0) <<
                    4 | CARRY2((ushort)UNRECOVERED_JUMPTABLE,(ushort)pcVar14));
  UNRECOVERED_JUMPTABLE = UNRECOVERED_JUMPTABLE + (short)pcVar14;
switchD_ram_0ec2_switchD:
  bVar3 = (byte)puVar6;
  bVar9 = (byte)((ushort)puVar6 >> 8);
  cVar13 = (char)pcVar14;
  cVar12 = (char)param_1;
  bVar4 = (byte)((ushort)param_1 >> 8);
  bVar7 = (byte)((ushort)UNRECOVERED_JUMPTABLE >> 8);
  pcVar15 = pcVar14;
  puVar18 = (undefined2 *)register0x44;
  switch(UNRECOVERED_JUMPTABLE) {
  case (code *)0xec1:
    goto switchD_ram_0ec2_caseD_ec1;
  case dispatch_0ec3:
    goto code_r0x0ec3;
  case dispatch_0ec5:
    goto code_r0x0ec5;
  case dispatch_0ec7:
    goto code_r0x0ec7;
  case dispatch_0ec9:
    goto code_r0x0ec9;
  case dispatch_0ecb:
    goto code_r0x0ecb;
  case dispatch_0ecd:
    goto code_r0x0ecd;
  case dispatch_0ecf:
    goto code_r0x0ecf;
  case dispatch_0ecf:
    goto switchD_ram_0ec2_caseD_ed1;
  case dispatch_0ecf:
    goto switchD_ram_0ec2_caseD_ed3;
  case dispatch_0ecf:
    goto switchD_ram_0ec2_caseD_ed5;
  case FUN_ram_0ed6:
    goto switchD_ram_0ec2_caseD_ed7;
  case FUN_ram_0ed6:
    goto switchD_ram_0ec2_caseD_ed9;
  case FUN_ram_0ed6:
    goto switchD_ram_0ec2_caseD_edb;
  case caseD_edd:
    goto code_r0x0edd;
  case caseD_edd:
    goto switchD_ram_0ec2_caseD_edf;
  case caseD_edd:
    goto switchD_ram_0ec2_caseD_ee1;
  case caseD_edd:
    goto switchD_ram_0ec2_caseD_ee3;
  case FUN_ram_0ee4:
    goto switchD_ram_0ec2_caseD_ee5;
  case FUN_ram_0ee4:
    goto switchD_ram_0ec2_caseD_ee7;
  case FUN_ram_0ee4:
    goto switchD_ram_0ec2_caseD_ee9;
  case caseD_eeb:
    goto code_r0x0eeb;
  case caseD_eeb:
    goto switchD_ram_0ec2_caseD_eed;
  case caseD_eeb:
    goto switchD_ram_0ec2_caseD_eef;
  case caseD_eeb:
    goto switchD_ram_0ec2_caseD_ef1;
  case FUN_ram_0ef2:
    goto switchD_ram_0ec2_caseD_ef3;
  case FUN_ram_0ef2:
    goto switchD_ram_0ec2_caseD_ef5;
  case FUN_ram_0ef2:
    goto switchD_ram_0ec2_caseD_ef7;
  case (code *)0xef9:
    goto switchD_ram_0ec2_caseD_ef9;
  case (code *)0xefb:
    goto switchD_ram_0ec2_caseD_efb;
  case (code *)0xefd:
    goto switchD_ram_0ec2_caseD_efd;
  case (code *)0xeff:
    puVar6 = (undefined *)0xf00;
  case (code *)0xf01:
    goto switchD_ram_0ec2_caseD_f01;
  case (code *)0xf03:
    goto switchD_ram_0ec2_caseD_f03;
  case (code *)0xf05:
    goto switchD_ram_0ec2_caseD_f05;
  case (code *)0xf07:
    goto switchD_ram_0ec2_caseD_f07;
  case (code *)0xf09:
    goto switchD_ram_0ec2_caseD_f09;
  case (code *)0xf0b:
    goto switchD_ram_0ec2_caseD_f0b;
  case (code *)0xf0d:
    goto switchD_ram_0ec2_caseD_f0d;
  case (code *)0xf0f:
    puVar6 = (undefined *)((ushort)(byte)(bVar9 + cVar13) << 8);
    goto code_r0x0f10;
  case (code *)0xf11:
    goto switchD_ram_0ec2_caseD_f11;
  case (code *)0xf13:
    puVar6 = (undefined *)((ushort)bVar9 << 8);
    goto code_r0x0f12;
  case (code *)0xf15:
    puVar6 = (undefined *)(CONCAT11((bVar9 - cVar12) - (bVar3 & 1),bVar3) & 0xff2a);
LAB_ram_0f16:
    cVar12 = (char)((ushort)puVar6 >> 8) + -1;
    puVar6 = (undefined *)(CONCAT11(cVar12,(byte)puVar6 & 0xa9 | (cVar12 == '\0') << 6) & 0xff7f);
  case (code *)0xf17:
    if (((byte)puVar6 >> 6 & 1) != 0) {
code_r0x0f1a:
      *(undefined2 *)((short)register0x44 + -2) = 0xf1d;
      bVar9 = FUN_ram_0f3e();
      puVar6 = (undefined *)((ushort)bVar9 << 8);
      goto switchD_ram_0ec2_caseD_f1d;
    }
    DAT_ram_800d = 3;
    DAT_ram_800f = 3;
    DAT_ram_83bc = DAT_ram_83bc - 1;
    if (DAT_ram_83bc != 0) {
      return DAT_ram_83bc;
    }
    DAT_ram_83bc = ' ';
    uVar5 = CONCAT11(DAT_ram_83d7,(byte)puVar6) & 0xff29;
    cVar12 = (char)(uVar5 >> 8);
    bVar9 = cVar12 * '\x02';
    puVar6 = (undefined *)
             CONCAT11(bVar9,(byte)uVar5 & 0xa8 | SCARRY1(cVar12,cVar12) << 2 | (bVar9 == 0) << 6 |
                            ((char)bVar9 < '\0') << 7);
    pcVar14 = (code *)(ushort)bVar9;
    UNRECOVERED_JUMPTABLE = (code *)0xdff;
    goto switchD_ram_0e00_caseD_dff;
  case (code *)0xf19:
    goto code_r0x0f1a;
  case (code *)0xf1b:
    puVar6 = (undefined *)0xf00;
  case (code *)0xf1d:
switchD_ram_0ec2_caseD_f1d:
    puVar6 = (undefined *)((ushort)(byte)((char)((ushort)puVar6 >> 8) - 3) << 8);
switchD_ram_0ec2_caseD_f1f:
    param_1 = (code *)((ushort)puVar6 >> 8);
    puVar6 = (undefined *)((ushort)DAT_ram_83d7 << 8);
    break;
  case (code *)0xf1f:
    goto switchD_ram_0ec2_caseD_f1f;
  case (code *)0xf21:
    *(undefined2 *)((short)register0x44 + -2) = 0xf22;
    cVar12 = func_0x0010();
    puVar6 = (undefined *)((ushort)(byte)(cVar12 + (char)pcVar14) << 8);
    break;
  case (code *)0xf25:
    bVar9 = bVar9 & (byte)UNRECOVERED_JUMPTABLE;
    param_1 = (code *)CONCAT11(bVar4,6);
    puVar6 = (undefined *)((ushort)(byte)(bVar9 << 1 | bVar9 >> 7) << 8);
    goto switchD_ram_0ec2_caseD_f29;
  case (code *)0xf27:
    goto switchD_ram_0ec2_caseD_f27;
  case (code *)0xf29:
    goto switchD_ram_0ec2_caseD_f29;
  case (code *)0xf2c:
    goto switchD_ram_0ec2_caseD_f2c;
  case (code *)0xf2d:
    param_1 = (code *)CONCAT11(cVar13,cVar12);
    puVar6 = (undefined *)((ushort)(byte)(bVar9 + cVar13) << 8);
  case (code *)0xf2f:
    goto switchD_ram_0ec2_caseD_f2f;
  case (code *)0xf31:
    goto switchD_ram_0ec2_caseD_f31;
  case (code *)0xf33:
    goto switchD_ram_0ec2_caseD_f33;
  case (code *)0xf35:
    goto switchD_ram_0ec2_caseD_f35;
  case (code *)0xf37:
    goto switchD_ram_0ec2_caseD_f37;
  case (code *)0xf39:
    goto switchD_ram_0ec2_caseD_f39;
  case (code *)0xf3b:
    *(undefined2 *)((short)register0x44 + -2) = 0xf3c;
    cVar12 = func_0x0010();
    puVar6 = (undefined *)((ushort)(byte)(cVar12 + (char)pcVar14) << 8);
  case (code *)0xf3d:
    goto switchD_ram_0ec2_caseD_f3d;
  case FUN_ram_0f3e:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_83bd;
    goto code_r0x0f42;
  case FUN_ram_0f3e:
code_r0x0f42:
    cVar8 = *UNRECOVERED_JUMPTABLE;
    *UNRECOVERED_JUMPTABLE = (code)((char)cVar8 + -1);
    puVar6 = (undefined *)(ushort)(byte)(((code)((char)cVar8 + -1) == (code)0x0) << 6);
switchD_ram_0ec2_caseD_f43:
    if (((byte)puVar6 >> 6 & 1) == 0) {
      register0x44 = (BADSPACEBASE *)((short)register0x44 + 2);
switchD_ram_0ec2_caseD_f57:
      return (byte)((ushort)*(undefined2 *)register0x44 >> 8);
    }
switchD_ram_0ec2_caseD_f45:
    *UNRECOVERED_JUMPTABLE = (code)0x8;
switchD_ram_0ec2_caseD_f47:
    UNRECOVERED_JUMPTABLE =
         (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                          (char)UNRECOVERED_JUMPTABLE + '\x01');
    cVar8 = *UNRECOVERED_JUMPTABLE;
    *UNRECOVERED_JUMPTABLE = (code)((char)cVar8 + -1);
    puVar6 = (undefined *)(ushort)(byte)(((code)((char)cVar8 + -1) == (code)0x0) << 6);
switchD_ram_0ec2_caseD_f49:
    if (((byte)puVar6 >> 6 & 1) != 0) {
switchD_ram_0ec2_caseD_f4b:
      *UNRECOVERED_JUMPTABLE = (code)0x4;
    }
switchD_ram_0ec2_caseD_f4d:
    puVar6 = (undefined *)((ushort)(byte)*UNRECOVERED_JUMPTABLE << 8);
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_2e1b;
switchD_ram_0ec2_caseD_f51:
    puVar6 = (undefined *)
             ((ushort)(byte)((char)((ushort)puVar6 >> 8) + (char)UNRECOVERED_JUMPTABLE) << 8);
code_r0x0f52:
    UNRECOVERED_JUMPTABLE =
         (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),(char)((ushort)puVar6 >> 8));
switchD_ram_0ec2_caseD_f53:
    puVar6 = (undefined *)((ushort)(byte)*UNRECOVERED_JUMPTABLE << 8);
switchD_ram_0ec2_caseD_f55:
    return (byte)((ushort)puVar6 >> 8);
  case (code *)0xf43:
    goto switchD_ram_0ec2_caseD_f43;
  case (code *)0xf45:
    goto switchD_ram_0ec2_caseD_f45;
  case (code *)0xf47:
    goto switchD_ram_0ec2_caseD_f47;
  case (code *)0xf49:
    goto switchD_ram_0ec2_caseD_f49;
  case (code *)0xf4b:
    goto switchD_ram_0ec2_caseD_f4b;
  case (code *)0xf4d:
    goto switchD_ram_0ec2_caseD_f4d;
  case (code *)0xf4f:
    UNRECOVERED_JUMPTABLE = (code *)((ushort)bVar7 << 8);
    goto code_r0x0f52;
  case (code *)0xf51:
    goto switchD_ram_0ec2_caseD_f51;
  case (code *)0xf53:
    goto switchD_ram_0ec2_caseD_f53;
  case (code *)0xf55:
    goto switchD_ram_0ec2_caseD_f55;
  case (code *)0xf57:
    goto switchD_ram_0ec2_caseD_f57;
  case blitGameOverLine:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_a850;
    goto code_r0x0f5c;
  case (code *)0xf5b:
    puVar6 = (undefined *)((ushort)(bVar9 ^ bVar4) << 8);
code_r0x0f5c:
    *(undefined2 *)((short)register0x44 + -2) = 0xf5f;
    blitFourTileGroupColumn((char)((ushort)puVar6 >> 8),UNRECOVERED_JUMPTABLE);
switchD_ram_0ec2_caseD_f5f:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_aa70;
code_r0x0f62:
    pcVar14 = (code *)&UNK_ram_2f0e;
  case (code *)0xf65:
switchD_ram_0ec2_caseD_f65:
switchD_ram_0ec2_caseD_f67:
    *(undefined2 *)((short)register0x44 + -2) = 0xf68;
    bVar9 = copyRunUpTileColumn(UNRECOVERED_JUMPTABLE,pcVar14);
    return bVar9;
  case (code *)0xf5d:
    if ((bVar3 >> 2 & 1) != 0) goto caseD_f5f_1;
    cVar12 = '\x02';
    if (DAT_ram_8110 == 'P') goto LAB_ram_213e;
    if (DAT_ram_8110 != -0x80) {
      if (DAT_ram_8110 == -0x60) {
        do {
          *(undefined2 *)((short)register0x44 + -2) = 0x216d;
          FUN_ram_2178(&UNK_ram_2198);
          cVar12 = cVar12 + -1;
        } while (cVar12 != '\0');
        DAT_ram_8107 = 1;
        bVar9 = FUN_ram_2188();
        return bVar9;
      }
      if (DAT_ram_8110 != -0x50) {
        if (DAT_ram_8110 != -0x30) {
          bVar9 = FUN_ram_2188(UNRECOVERED_JUMPTABLE + -0x57f8);
          return bVar9;
        }
LAB_ram_213e:
        do {
          *(undefined2 *)((short)register0x44 + -2) = 0x2146;
          FUN_ram_2178(&UNK_ram_2190);
          cVar12 = cVar12 + -1;
        } while (cVar12 != '\0');
        bVar9 = FUN_ram_2188();
        return bVar9;
      }
    }
    do {
      *(undefined2 *)((short)register0x44 + -2) = 0x2154;
      FUN_ram_2178(&UNK_ram_2194);
      cVar12 = cVar12 + -1;
    } while (cVar12 != '\0');
    if (DAT_ram_8107 == '\0') {
      DAT_ram_811a = *(char *)(param_2 + 2) - 1;
      return DAT_ram_811a;
    }
    DAT_ram_8107 = 0;
    bVar9 = FUN_ram_2188();
    return bVar9;
  case (code *)0xf5f:
    goto switchD_ram_0ec2_caseD_f5f;
  case (code *)0xf61:
    goto switchD_ram_0ec2_caseD_f61;
  case (code *)0xf63:
    goto switchD_ram_0ec2_caseD_f65;
  case (code *)0xf67:
    goto switchD_ram_0ec2_caseD_f67;
  case (code *)0xf69:
    pcVar14 = DAT_ram_83ed;
  case (code *)0xf6d:
    UNRECOVERED_JUMPTABLE = DAT_ram_83eb;
code_r0x0f70:
    param_1 = (code *)((ushort)UNRECOVERED_JUMPTABLE & 0xff00);
switchD_ram_0ec2_caseD_f71:
    param_1 = (code *)CONCAT11((char)((ushort)param_1 >> 8),(char)UNRECOVERED_JUMPTABLE);
    puVar6 = (undefined *)0x0;
switchD_ram_0ec2_caseD_f73:
    uVar5 = (ushort)((byte)puVar6 & 1);
    bVar1 = UNRECOVERED_JUMPTABLE < pcVar14;
    uVar2 = (short)UNRECOVERED_JUMPTABLE - (short)pcVar14;
    UNRECOVERED_JUMPTABLE = (code *)(uVar2 - uVar5);
    puVar6 = (undefined *)(ushort)(bVar1 || uVar2 < uVar5);
switchD_ram_0ec2_caseD_f75:
    if ((bool)((byte)puVar6 & 1)) {
      puVar17 = (undefined2 *)((short)register0x44 + -2);
      register0x44 = (BADSPACEBASE *)((short)register0x44 + -2);
      *puVar17 = param_1;
    }
    else {
switchD_ram_0ec2_caseD_f77:
      *(code **)((short)register0x44 + -2) = pcVar14;
      puVar16 = (undefined2 *)((short)register0x44 + -4);
      register0x44 = (BADSPACEBASE *)((short)register0x44 + -4);
      *puVar16 = param_1;
switchD_ram_0ec2_caseD_f79:
      pcVar14 = *(code **)register0x44;
      register0x44 = (BADSPACEBASE *)((short)register0x44 + 2);
    }
switchD_ram_0ec2_caseD_f7d:
    *(undefined2 *)((short)register0x44 + -2) = 0xf80;
    bVar9 = insertHighScoreEntry(UNRECOVERED_JUMPTABLE,pcVar14);
    puVar6 = (undefined *)((ushort)bVar9 << 8);
code_r0x0f80:
    pcVar14 = *(code **)register0x44;
    puVar18 = (undefined2 *)((short)register0x44 + 2);
switchD_ram_0ec2_caseD_f81:
    register0x44 = (BADSPACEBASE *)((short)puVar18 + -2);
    *(undefined **)((short)puVar18 + -2) = puVar6;
    *(undefined2 *)((short)puVar18 + -4) = 0xf85;
    bVar9 = insertHighScoreEntry(pcVar14);
    puVar6 = (undefined *)((ushort)bVar9 << 8);
  case (code *)0xf85:
switchD_ram_0ec2_caseD_f85:
    UNRECOVERED_JUMPTABLE = (code *)((ushort)puVar6 & 0xff00);
    puVar6 = *(undefined **)register0x44;
switchD_ram_0ec2_caseD_f87:
    DAT_ram_83fb = CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),(char)((ushort)puVar6 >> 8));
switchD_ram_0ec2_caseD_f8b:
    return (byte)((ushort)puVar6 >> 8);
  case (code *)0xf6b:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case (code *)0xf6f:
    goto code_r0x0f70;
  case (code *)0xf71:
    goto switchD_ram_0ec2_caseD_f71;
  case (code *)0xf73:
    goto switchD_ram_0ec2_caseD_f73;
  case (code *)0xf75:
    goto switchD_ram_0ec2_caseD_f75;
  case (code *)0xf77:
    goto switchD_ram_0ec2_caseD_f77;
  case (code *)0xf79:
    goto switchD_ram_0ec2_caseD_f79;
  case (code *)0xf7b:
    param_1 = (code *)&UNK_ram_cdc5;
    puVar6 = (undefined *)
             (ushort)(byte)(bVar3 & 0x2a | (((bVar9 & 0xf) + (bVar7 & 0xf) & 0x10) != 0) << 4 |
                            SCARRY1(bVar9,bVar7) << 2 | CARRY1(bVar9,bVar7) |
                            ((byte)(bVar9 + bVar7) == '\0') << 6 |
                           ((char)(bVar9 + bVar7) < '\0') << 7);
  case (code *)0xf7f:
    puVar6 = (undefined *)CONCAT11(*param_1,(char)puVar6);
    goto code_r0x0f80;
  case (code *)0xf7d:
    goto switchD_ram_0ec2_caseD_f7d;
  case (code *)0xf81:
    goto switchD_ram_0ec2_caseD_f81;
  case (code *)0xf83:
    puVar6 = (undefined *)((ushort)(byte)*param_1 << 8);
    goto switchD_ram_0ec2_caseD_f85;
  case (code *)0xf87:
    goto switchD_ram_0ec2_caseD_f87;
  case (code *)0xf89:
    enableMaskableInterrupts();
    puVar6 = (undefined *)((ushort)(byte)(bVar9 + cVar13) << 8);
  case (code *)0xf8b:
    goto switchD_ram_0ec2_caseD_f8b;
  case (code *)0xf8d:
    goto code_r0x0f10;
  case (code *)0xf8f:
    if (bVar9 == 0) {
      return 0;
    }
  case (code *)0xf91:
    pcVar14 = (code *)&DAT_ram_a806;
code_r0x0f94:
    param_1 = (code *)CONCAT11(8,cVar12);
code_r0x0f96:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_1413;
    goto switchD_ram_0ec2_caseD_f99;
  case (code *)0xf93:
    goto code_r0x0f94;
  case (code *)0xf95:
    goto code_r0x0f96;
  case (code *)0xf97:
    pcVar14 = (code *)CONCAT11(1,(char)(pcVar14 + 1));
  case (code *)0xf99:
    goto switchD_ram_0ec2_caseD_f99;
  case (code *)0xf9b:
    goto switchD_ram_0ec2_caseD_f9b;
  case (code *)0xf9d:
    goto switchD_ram_0ec2_caseD_f9d;
  case (code *)0xf9f:
    goto switchD_ram_0ec2_caseD_f9f;
  case (code *)0xfa1:
    goto switchD_ram_0ec2_caseD_fa1;
  case (code *)0xfa4:
    goto switchD_ram_0ec2_caseD_fa4;
  case (code *)0xfa5:
    goto switchD_ram_0ec2_caseD_fa5;
  case (code *)0xfa7:
    goto switchD_ram_0ec2_caseD_fa7;
  case (code *)0xfa9:
    *(undefined2 *)((short)register0x44 + -2) = 0xfaa;
    copyRunUpTileColumn();
    goto code_r0x0faa;
  case (code *)0xfab:
    goto switchD_ram_0ec2_caseD_fab;
  case (code *)0xfad:
    puVar6 = (undefined *)((ushort)(byte)(bVar9 + cVar12) << 8);
    goto switchD_ram_0ec2_caseD_fab;
  case dispatchFrogAnimationArm:
    goto code_r0x0faf;
  case dispatchFrogAnimationArm:
code_r0x0faf:
    param_1 = (code *)&switchD_ram:0fbd::switchdataD_ram_0fbe;
    UNRECOVERED_JUMPTABLE = _caseD_36;
  case dispatchFrogAnimationArm:
switchD_ram_0ec2_caseD_fb5:
    UNRECOVERED_JUMPTABLE = (code *)((ushort)UNRECOVERED_JUMPTABLE & 0xff);
switchD_ram_0ec2_caseD_fb7:
    UNRECOVERED_JUMPTABLE = param_1 + (short)UNRECOVERED_JUMPTABLE * 2;
switchD_ram_0ec2_caseD_fb9:
    param_1 = (code *)(ushort)(byte)*UNRECOVERED_JUMPTABLE;
    UNRECOVERED_JUMPTABLE = UNRECOVERED_JUMPTABLE + 1;
switchD_ram_0ec2_caseD_fbb:
    UNRECOVERED_JUMPTABLE = (code *)CONCAT11(*UNRECOVERED_JUMPTABLE,(char)param_1);
switchD_ram_0ec2_caseD_fbd:
                    /* WARNING: Could not recover jumptable at 0x0fbd. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    bVar9 = (*UNRECOVERED_JUMPTABLE)();
    return bVar9;
  case dispatchFrogAnimationArm:
    goto switchD_ram_0ec2_caseD_fb5;
  case (code *)0xfb7:
    goto switchD_ram_0ec2_caseD_fb7;
  case (code *)0xfb9:
    goto switchD_ram_0ec2_caseD_fb9;
  case (code *)0xfbb:
    goto switchD_ram_0ec2_caseD_fbb;
  case (code *)0xfbd:
    goto switchD_ram_0ec2_caseD_fbd;
  case (code *)0xfbf:
    if (bVar4 != 1) {
      bVar9 = renderFrogAnimTileColumns
                        (CONCAT11(DAT_ram_8003,0x81),UNRECOVERED_JUMPTABLE,
                         switchD_ram:0fbd::caseD_40);
      return bVar9;
    }
    param_1 = (code *)CONCAT11(0xff,cVar12);
caseD_f5f_1:
    *UNRECOVERED_JUMPTABLE = SUB21((ushort)param_1 >> 8,0);
switchD_ram_0ec2_caseD_f61:
    goto code_r0x0f62;
  }
  bVar9 = (byte)((ushort)puVar6 >> 8);
  puVar6 = (undefined *)((ushort)bVar9 << 8);
  if (bVar9 == 0) goto switchD_ram_0e00_caseD_ea5;
switchD_ram_0ec2_caseD_f27:
  param_1 = (code *)CONCAT11(7,(char)param_1);
switchD_ram_0ec2_caseD_f29:
  pcVar14 = (code *)0x6;
switchD_ram_0ec2_caseD_f2c:
  UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8043;
  goto switchD_ram_0ec2_caseD_f2f;
switchD_ram_0e00_caseD_dff:
  bVar4 = (byte)puVar6;
  bVar7 = (byte)((ushort)puVar6 >> 8);
  bVar3 = bVar4 & 0xee |
          ((((ushort)UNRECOVERED_JUMPTABLE & 0xfff) + (ushort)bVar9 & 0x1000) != 0) << 4 |
          CARRY2((ushort)UNRECOVERED_JUMPTABLE,(ushort)pcVar14);
  puVar6 = (undefined *)CONCAT11(bVar7,bVar3);
  UNRECOVERED_JUMPTABLE = UNRECOVERED_JUMPTABLE + (short)pcVar14;
  bVar11 = (byte)((ushort)param_1 >> 8);
  switch(UNRECOVERED_JUMPTABLE) {
  case (code *)0xdff:
    goto switchD_ram_0e00_caseD_dff;
  case (code *)0xe01:
    bVar9 = dispatch_0e49();
    return bVar9;
  case (code *)0xe03:
    bVar9 = dispatch_0e3f();
    return bVar9;
  case (code *)0xe05:
    bVar9 = dispatch_0e35();
    return bVar9;
  case (code *)0xe07:
    bVar9 = dispatch_0e2b();
    return bVar9;
  case (code *)0xe09:
    bVar9 = dispatch_0e21();
    return bVar9;
  case (code *)0xe0b:
    bVar9 = dispatch_0e17();
    return bVar9;
  case dispatch_0e0d:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_ab06;
    goto code_r0x0e10;
  case dispatch_0e0d:
code_r0x0e10:
    pcVar14 = (code *)&DAT_ram_8040;
    goto switchD_ram_0e00_caseD_e13;
  case dispatch_0e0d:
  case (code *)0xe13:
    goto switchD_ram_0e00_caseD_e13;
  case (code *)0xe15:
    goto switchD_ram_0e00_caseD_e15;
  case dispatch_0e17:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_aaa6;
    goto code_r0x0e1a;
  case dispatch_0e17:
code_r0x0e1a:
    pcVar14 = (code *)&DAT_ram_8044;
  case dispatch_0e17:
switchD_ram_0e00_caseD_e1d:
    puVar6 = &UNK_ram_d800;
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
    pcVar14 = (code *)&DAT_ram_8048;
  case dispatch_0e21:
switchD_ram_0e00_caseD_e27:
    puVar6 = &UNK_ram_dc00;
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
    pcVar14 = (code *)&DAT_ram_804c;
  case dispatch_0e2b:
switchD_ram_0e00_caseD_e31:
    puVar6 = &UNK_ram_f400;
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
    pcVar14 = (code *)&DAT_ram_8050;
  case dispatch_0e35:
switchD_ram_0e00_caseD_e3b:
    puVar6 = &UNK_ram_f400;
switchD_ram_0e00_caseD_e3d:
    goto switchD_ram_0e00_caseD_e51;
  case dispatch_0e35:
    pcVar14 = (code *)CONCAT11(bVar11,bVar9);
    goto switchD_ram_0e00_caseD_e3b;
  case (code *)0xe3d:
    goto switchD_ram_0e00_caseD_e3d;
  case dispatch_0e3f:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_a926;
    goto code_r0x0e42;
  case dispatch_0e3f:
code_r0x0e42:
    pcVar14 = (code *)&UNK_ram_8054;
  case dispatch_0e3f:
switchD_ram_0e00_caseD_e45:
    puVar6 = (undefined *)0xf800;
switchD_ram_0e00_caseD_e47:
    goto switchD_ram_0e00_caseD_e51;
  case dispatch_0e3f:
    pcVar14 = (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),bVar9);
    goto switchD_ram_0e00_caseD_e45;
  case (code *)0xe47:
    goto switchD_ram_0e00_caseD_e47;
  case dispatch_0e49:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_a8c6;
    goto code_r0x0e4c;
  case (code *)0xe4b:
code_r0x0e4c:
    pcVar14 = (code *)&DAT_ram_8058;
  case (code *)0xe4f:
switchD_ram_0e00_caseD_e4f:
    puVar6 = &UNK_ram_d800;
    goto switchD_ram_0e00_caseD_e51;
  case (code *)0xe4d:
    pcVar14 = (code *)(ushort)bVar11;
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
    param_1 = (code *)CONCAT11(bVar11 + 1,(char)param_1);
    puVar6 = (undefined *)((ushort)bVar7 << 8);
    goto LAB_ram_0e62;
  case (code *)0xe63:
    goto switchD_ram_0e00_caseD_e63;
  case (code *)0xe65:
    if ((bool)(bVar4 >> 7)) {
      *(undefined2 *)((short)register0x44 + -2) = 0xe68;
      bVar3 = func_0xd721();
      bVar9 = (byte)pcVar14;
      puVar6 = (undefined *)((ushort)bVar3 << 8);
    }
    goto code_r0x0e68;
  case (code *)0xe67:
    *(undefined2 *)((short)register0x44 + -2) = 0xe68;
    bVar3 = func_0x0010();
    bVar9 = (byte)pcVar14;
    puVar6 = (undefined *)((ushort)bVar3 << 8);
code_r0x0e68:
    puVar6 = (undefined *)((ushort)(byte)((char)((ushort)puVar6 >> 8) + bVar9) << 8);
  case (code *)0xe69:
    goto switchD_ram_0e00_caseD_e69;
  case (code *)0xe6b:
    goto switchD_ram_0e00_caseD_e6b;
  case (code *)0xe6d:
    goto switchD_ram_0e00_caseD_e6d;
  case (code *)0xe6f:
    puVar6 = (undefined *)((ushort)(byte)(bVar7 + bVar9) << 8);
  case (code *)0xe71:
    goto switchD_ram_0e00_caseD_e71;
  case (code *)0xe73:
    goto setAttractIdleMode;
  case setAttractIdleMode:
    goto setAttractIdleMode;
  case setAttractIdleMode:
    puVar6 = (undefined *)((ushort)(byte)(bVar7 + 0x7d) << 8);
  case setAttractIdleMode:
    goto switchD_ram_0e00_caseD_e79;
  case (code *)0xe7b:
    register0x44 = (BADSPACEBASE *)((short)register0x44 + 2);
    puVar6 = (undefined *)(CONCAT11(bVar7 + bVar9,bVar3) & 0xff2a);
  case (code *)0xe7d:
    goto switchD_ram_0e00_caseD_e7d;
  case (code *)0xe7f:
    if (-1 < (char)bVar3) {
      *(undefined2 *)((short)register0x44 + -2) = 0xe82;
      func_0xbf21();
      puVar6 = (undefined *)0x0;
    }
    goto code_r0x0e82;
  case (code *)0xe81:
    puVar6 = (undefined *)(CONCAT11(bVar7,bVar3) & 0xff2a);
code_r0x0e82:
    puVar6 = (undefined *)(ushort)((byte)puVar6 & 0x28);
  case (code *)0xe83:
    goto switchD_ram_0e00_caseD_e83;
  case (code *)0xe85:
    goto switchD_ram_0e00_caseD_e85;
  case (code *)0xe87:
    goto switchD_ram_0e00_caseD_e87;
  case (code *)0xe89:
    goto switchD_ram_0e00_caseD_e87;
  case (code *)0xe8b:
    param_1 = (code *)0x2100;
  case (code *)0xe8f:
    puVar6 = (undefined *)((ushort)(byte)(bVar7 + (char)((ushort)param_1 >> 8)) << 8);
  case (code *)0xe8d:
    goto switchD_ram_0e00_caseD_e8d;
  case (code *)0xe91:
    param_1 = param_1 + 1;
    puVar6 = (undefined *)((ushort)(byte)(bVar7 << 1 | bVar7 >> 7) << 8);
  case (code *)0xe93:
    goto switchD_ram_0e00_caseD_e93;
  case (code *)0xe95:
    puVar6 = (undefined *)((ushort)(byte)(bVar7 + (char)param_1) << 8);
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
    puVar6 = (undefined *)((ushort)bVar7 << 8);
    goto switchD_ram_0e00_caseD_e9f;
  case (code *)0xea3:
    puVar6 = (undefined *)((ushort)(byte)(bVar7 + bVar9) << 8);
  case (code *)0xea5:
    goto switchD_ram_0e00_caseD_ea5;
  case (code *)0xea7:
    puVar6 = (undefined *)((ushort)(byte)(bVar7 + bVar9) << 8);
    goto switchD_ram_0e00_caseD_ea5;
  case (code *)0xea9:
    puVar6 = (undefined *)((ushort)(byte)(bVar7 << 1 | bVar7 >> 7) << 8);
    goto switchD_ram_0e00_caseD_ea5;
  case (code *)0xeab:
    puVar6 = (undefined *)((ushort)(byte)(bVar7 + bVar9) << 8);
  case (code *)0xead:
    goto switchD_ram_0e00_caseD_ead;
  case (code *)0xeaf:
    goto switchD_ram_0e00_caseD_eaf;
  case (code *)0xeb1:
    puVar6 = (undefined *)((ushort)(byte)(bVar7 + bVar9) << 8);
    goto switchD_ram_0e00_caseD_eaf;
  case (code *)0xeb3:
    goto switchD_ram_0e00_caseD_eb3;
  case (code *)0xeb5:
    goto switchD_ram_0e00_caseD_eb5;
  case (code *)0xeb7:
    goto switchD_ram_0e00_caseD_eb7;
  case (code *)0xeb9:
    puVar6 = (undefined *)(ushort)(bVar4 & 0x2a);
    goto switchD_ram_0e00_caseD_eb7;
  case (code *)0xebb:
    goto switchD_ram_0e00_caseD_ebb;
  case (code *)0xebd:
    goto switchD_ram_0e00_caseD_ebd;
  case (code *)0xebf:
    goto switchD_ram_0e00_caseD_ebf;
  case (code *)0xec1:
    goto switchD_ram_0ec2_caseD_ec1;
  case dispatch_0ec3:
code_r0x0ec3:
  case (code *)0xef9:
switchD_ram_0ec2_caseD_ef9:
    cVar12 = (char)((ushort)puVar6 >> 8);
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8058;
code_r0x0efc:
    param_1 = (code *)&UNK_ram_c100;
    break;
  case dispatch_0ec5:
code_r0x0ec5:
    goto code_r0x0ef2;
  case dispatch_0ec7:
code_r0x0ec7:
  case caseD_eeb:
code_r0x0eeb:
    cVar12 = (char)((ushort)puVar6 >> 8);
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8050;
code_r0x0eee:
    param_1 = (code *)&UNK_ram_9100;
    break;
  case dispatch_0ec9:
code_r0x0ec9:
    goto code_r0x0ee4;
  case dispatch_0ecb:
code_r0x0ecb:
  case caseD_edd:
code_r0x0edd:
    cVar12 = (char)((ushort)puVar6 >> 8);
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8048;
code_r0x0ee0:
    param_1 = (code *)&UNK_ram_6100;
    break;
  case dispatch_0ecd:
code_r0x0ecd:
    goto code_r0x0ed6;
  case dispatch_0ecf:
code_r0x0ecf:
    cVar12 = (char)((ushort)puVar6 >> 8);
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8040;
    goto code_r0x0ed2;
  case (code *)0xed1:
switchD_ram_0ec2_caseD_ed1:
    cVar12 = (char)((ushort)puVar6 >> 8) + (char)((ushort)param_1 >> 8);
code_r0x0ed2:
    param_1 = (code *)&UNK_ram_3100;
    break;
  case (code *)0xed3:
switchD_ram_0ec2_caseD_ed3:
    register0x44 = (BADSPACEBASE *)0x2818;
code_r0x0ed6:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_8044;
  case (code *)0xed9:
switchD_ram_0ec2_caseD_ed9:
    param_1 = (code *)&UNK_ram_4900;
switchD_ram_0ec2_caseD_edb:
    cVar12 = (char)((ushort)puVar6 >> 8);
    break;
  case (code *)0xed5:
switchD_ram_0ec2_caseD_ed5:
    if (!(bool)((byte)puVar6 >> 6 & 1)) goto switchD_ram_0ec2_caseD_ed7;
    puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
    goto switchD_ram_0ec2_caseD_ef9;
  case (code *)0xed7:
switchD_ram_0ec2_caseD_ed7:
    puVar6 = (undefined *)
             ((ushort)(byte)((char)((ushort)puVar6 >> 8) +
                            (char)((ushort)UNRECOVERED_JUMPTABLE >> 8)) << 8);
    goto switchD_ram_0ec2_caseD_ed9;
  case (code *)0xedb:
    goto switchD_ram_0ec2_caseD_edb;
  case (code *)0xedf:
switchD_ram_0ec2_caseD_edf:
    cVar12 = (char)((ushort)puVar6 >> 8) + (char)((ushort)param_1 >> 8);
    goto code_r0x0ee0;
  case (code *)0xee1:
switchD_ram_0ec2_caseD_ee1:
    cVar12 = (char)((ushort)puVar6 >> 8);
    UNRECOVERED_JUMPTABLE = (code *)CONCAT11((char)param_1,(char)UNRECOVERED_JUMPTABLE);
    break;
  case (code *)0xee3:
switchD_ram_0ec2_caseD_ee3:
    puVar6 = (undefined *)((ushort)(byte)*pcVar14 << 8);
code_r0x0ee4:
    UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_804c;
  case (code *)0xee7:
switchD_ram_0ec2_caseD_ee7:
    param_1 = (code *)&UNK_ram_7900;
switchD_ram_0ec2_caseD_ee9:
    cVar12 = (char)((ushort)puVar6 >> 8);
    break;
  case (code *)0xee5:
switchD_ram_0ec2_caseD_ee5:
    puVar6 = (undefined *)
             ((ushort)(byte)((char)((ushort)puVar6 >> 8) + (char)((ushort)param_1 >> 8)) << 8);
    goto switchD_ram_0ec2_caseD_ee7;
  case (code *)0xee9:
    goto switchD_ram_0ec2_caseD_ee9;
  case (code *)0xeed:
switchD_ram_0ec2_caseD_eed:
    cVar12 = (char)((ushort)puVar6 >> 8) + (char)((ushort)param_1 >> 8);
    goto code_r0x0eee;
  case (code *)0xeef:
switchD_ram_0ec2_caseD_eef:
    cVar12 = (char)((ushort)puVar6 >> 8) - (char)param_1;
    break;
  case (code *)0xef1:
switchD_ram_0ec2_caseD_ef1:
    puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
code_r0x0ef2:
    UNRECOVERED_JUMPTABLE = (code *)&UNK_ram_8054;
  case (code *)0xef5:
switchD_ram_0ec2_caseD_ef5:
    param_1 = (code *)&UNK_ram_a900;
switchD_ram_0ec2_caseD_ef7:
    cVar12 = (char)((ushort)puVar6 >> 8);
    break;
  case (code *)0xef3:
switchD_ram_0ec2_caseD_ef3:
    puVar6 = (undefined *)
             ((ushort)(byte)((char)((ushort)puVar6 >> 8) + (char)((ushort)param_1 >> 8)) << 8);
    goto switchD_ram_0ec2_caseD_ef5;
  case (code *)0xef7:
    goto switchD_ram_0ec2_caseD_ef7;
  case (code *)0xefb:
switchD_ram_0ec2_caseD_efb:
    cVar12 = (char)((ushort)puVar6 >> 8) + (char)((ushort)param_1 >> 8);
    goto code_r0x0efc;
  case (code *)0xefd:
switchD_ram_0ec2_caseD_efd:
    cVar12 = (char)((ushort)puVar6 >> 8);
    param_1 = *(code **)register0x44;
    register0x44 = (BADSPACEBASE *)((short)register0x44 + 2);
  }
  *(undefined2 *)((short)register0x44 + -2) = 0xf01;
  bVar9 = FUN_ram_0f3e(cVar12);
  puVar6 = (undefined *)((ushort)bVar9 << 8);
switchD_ram_0ec2_caseD_f01:
  param_1 = (code *)CONCAT11((char)((ushort)param_1 >> 8),(char)((ushort)puVar6 >> 8));
  *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE + -1);
switchD_ram_0ec2_caseD_f03:
  *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE + -1);
  *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE + -1);
switchD_ram_0ec2_caseD_f05:
  *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE + -1);
  puVar6 = (undefined *)((ushort)(byte)*UNRECOVERED_JUMPTABLE << 8);
switchD_ram_0ec2_caseD_f07:
  UNRECOVERED_JUMPTABLE =
       (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                        (char)UNRECOVERED_JUMPTABLE + '\x01');
  puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
  *UNRECOVERED_JUMPTABLE = SUB21(param_1,0);
switchD_ram_0ec2_caseD_f09:
  bVar9 = (byte)((ushort)puVar6 >> 8);
  bVar1 = bVar9 < (byte)((ushort)param_1 >> 8);
  puVar6 = (undefined *)CONCAT11(bVar9,bVar1);
  if (!bVar1) {
    return bVar9;
  }
switchD_ram_0ec2_caseD_f0b:
  *UNRECOVERED_JUMPTABLE = (code)0x1e;
switchD_ram_0ec2_caseD_f0d:
  UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_83d7;
code_r0x0f10:
  cVar8 = *UNRECOVERED_JUMPTABLE;
  *UNRECOVERED_JUMPTABLE = (code)((char)cVar8 + -1);
  puVar6 = (undefined *)
           CONCAT11((char)((ushort)puVar6 >> 8),((code)((char)cVar8 + -1) == (code)0x0) << 6);
switchD_ram_0ec2_caseD_f11:
  if (((byte)puVar6 >> 6 & 1) == 0) {
    return (byte)((ushort)puVar6 >> 8);
  }
code_r0x0f12:
  *UNRECOVERED_JUMPTABLE = (code)0x14;
  goto switchD_ram_0e00_caseD_eaf;
switchD_ram_0e00_caseD_ebf:
  uVar10 = *(undefined2 *)register0x44;
  register0x44 = (BADSPACEBASE *)((short)register0x44 + 2);
  param_1 = (code *)CONCAT11((char)((ushort)uVar10 >> 8),0x19);
  goto switchD_ram_0ec2_switchD;
switchD_ram_0ec2_caseD_f2f:
  do {
    *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE + -1);
    *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE + -1);
    puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
switchD_ram_0ec2_caseD_f31:
    *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE + -1);
    *UNRECOVERED_JUMPTABLE = (code)((char)*UNRECOVERED_JUMPTABLE + -1);
    puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
switchD_ram_0ec2_caseD_f33:
    UNRECOVERED_JUMPTABLE =
         (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                          (char)UNRECOVERED_JUMPTABLE + -2);
    puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
switchD_ram_0ec2_caseD_f35:
    *UNRECOVERED_JUMPTABLE = SUB21(param_1,0);
    puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
    UNRECOVERED_JUMPTABLE = UNRECOVERED_JUMPTABLE + (short)pcVar14;
switchD_ram_0ec2_caseD_f37:
    cVar12 = (char)((ushort)param_1 >> 8) + -1;
    param_1 = (code *)CONCAT11(cVar12,(char)param_1);
  } while (cVar12 != '\0');
switchD_ram_0ec2_caseD_f39:
  DAT_ram_83d7 = (char)((ushort)puVar6 >> 8) - 1;
  puVar6 = (undefined *)((ushort)DAT_ram_83d7 << 8);
switchD_ram_0ec2_caseD_f3d:
  return (byte)((ushort)puVar6 >> 8);
switchD_ram_0ec2_caseD_f99:
  do {
    *pcVar14 = *UNRECOVERED_JUMPTABLE;
switchD_ram_0ec2_caseD_f9b:
    UNRECOVERED_JUMPTABLE = UNRECOVERED_JUMPTABLE + 1;
    pcVar14 = pcVar14 + 1;
switchD_ram_0ec2_caseD_f9d:
    *pcVar14 = *UNRECOVERED_JUMPTABLE;
switchD_ram_0ec2_caseD_f9f:
    UNRECOVERED_JUMPTABLE = UNRECOVERED_JUMPTABLE + 1;
    puVar19 = (undefined2 *)((short)register0x44 + -2);
    register0x44 = (BADSPACEBASE *)((short)register0x44 + -2);
    *puVar19 = param_1;
switchD_ram_0ec2_caseD_fa1:
    param_1 = (code *)0x1f;
switchD_ram_0ec2_caseD_fa4:
    pcVar15 = UNRECOVERED_JUMPTABLE;
    UNRECOVERED_JUMPTABLE = pcVar14;
switchD_ram_0ec2_caseD_fa5:
    pcVar14 = UNRECOVERED_JUMPTABLE + (short)param_1;
    UNRECOVERED_JUMPTABLE = pcVar15;
switchD_ram_0ec2_caseD_fa7:
    uVar10 = *(undefined2 *)register0x44;
    register0x44 = (BADSPACEBASE *)((short)register0x44 + 2);
    cVar12 = (char)((ushort)uVar10 >> 8) + -1;
    param_1 = (code *)CONCAT11(cVar12,(char)uVar10);
  } while (cVar12 != '\0');
code_r0x0faa:
  puVar6 = (undefined *)0x0;
switchD_ram_0ec2_caseD_fab:
  DAT_ram_8118 = (byte)((ushort)puVar6 >> 8);
  return DAT_ram_8118;
switchD_ram_0e00_caseD_e13:
  puVar6 = &UNK_ram_d400;
switchD_ram_0e00_caseD_e15:
switchD_ram_0e00_caseD_e51:
  param_1 = (code *)0x1f;
switchD_ram_0e00_caseD_e54:
  *UNRECOVERED_JUMPTABLE = SUB21((ushort)puVar6 >> 8,0);
switchD_ram_0e00_caseD_e55:
  UNRECOVERED_JUMPTABLE =
       (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                        (char)UNRECOVERED_JUMPTABLE + '\x01');
  puVar6 = (undefined *)((ushort)(byte)((char)((ushort)puVar6 >> 8) + 1) << 8);
switchD_ram_0e00_caseD_e57:
  cVar8 = SUB21((ushort)puVar6 >> 8,0);
  *UNRECOVERED_JUMPTABLE = cVar8;
  puVar6 = (undefined *)((ushort)(byte)((char)cVar8 + 1) << 8);
switchD_ram_0e00_caseD_e59:
  uVar5 = (ushort)puVar6 >> 8;
  puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
  UNRECOVERED_JUMPTABLE = UNRECOVERED_JUMPTABLE + (short)param_1;
  *UNRECOVERED_JUMPTABLE = SUB21(uVar5,0);
switchD_ram_0e00_caseD_e5b:
  UNRECOVERED_JUMPTABLE =
       (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                        (char)UNRECOVERED_JUMPTABLE + '\x01');
  puVar6 = (undefined *)((ushort)(byte)((char)((ushort)puVar6 >> 8) + 1) << 8);
switchD_ram_0e00_caseD_e5d:
  *UNRECOVERED_JUMPTABLE = SUB21((ushort)puVar6 >> 8,0);
  UNRECOVERED_JUMPTABLE = pcVar14;
switchD_ram_0e00_caseD_e5f:
  param_1 = (code *)0x400;
LAB_ram_0e62:
  do {
    *UNRECOVERED_JUMPTABLE = SUB21(param_1,0);
switchD_ram_0e00_caseD_e63:
    UNRECOVERED_JUMPTABLE =
         (code *)CONCAT11((char)((ushort)UNRECOVERED_JUMPTABLE >> 8),
                          (char)UNRECOVERED_JUMPTABLE + '\x01');
    puVar6 = (undefined *)((ushort)puVar6 & 0xff00);
    cVar12 = (char)((ushort)param_1 >> 8) + -1;
    param_1 = (code *)CONCAT11(cVar12,(char)param_1);
  } while (cVar12 != '\0');
  UNRECOVERED_JUMPTABLE = (code *)&DAT_ram_83d7;
switchD_ram_0e00_caseD_e69:
  cVar8 = *UNRECOVERED_JUMPTABLE;
  *UNRECOVERED_JUMPTABLE = (code)((char)cVar8 + -1);
  if ((code)((char)cVar8 + -1) != (code)0x0) {
    return (byte)((ushort)puVar6 >> 8);
  }
switchD_ram_0e00_caseD_e6b:
  *UNRECOVERED_JUMPTABLE = (code)0x7;
switchD_ram_0e00_caseD_e6d:
  puVar6 = (undefined *)0x0;
  DAT_ram_83bf = '\0';
switchD_ram_0e00_caseD_e71:
  DAT_ram_83bb = (undefined1)((ushort)puVar6 >> 8);
setAttractIdleMode:
  puVar6 = (undefined *)0x500;
  DAT_ram_83d6 = 5;
switchD_ram_0e00_caseD_e79:
  return (byte)((ushort)puVar6 >> 8);
}

