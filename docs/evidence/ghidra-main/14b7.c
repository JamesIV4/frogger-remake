
/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Instruction at (ram,0x21df) overlaps instruction at (ram,0x21de)
    */
/* WARNING: Removing unreachable block (ram,0x1b13) */
/* WARNING: Removing unreachable block (ram,0x1b8b) */
/* WARNING: Removing unreachable block (ram,0x1b90) */
/* WARNING: Removing unreachable block (ram,0x1b91) */
/* WARNING: Removing unreachable block (ram,0x1b97) */
/* WARNING: Removing unreachable block (ram,0x1ba2) */
/* WARNING: Removing unreachable block (ram,0x1ba7) */
/* WARNING: Removing unreachable block (ram,0x1baf) */
/* WARNING: Removing unreachable block (ram,0x1bb0) */
/* WARNING: Removing unreachable block (ram,0x1bb4) */
/* WARNING: Removing unreachable block (ram,0x1bbe) */
/* WARNING: Removing unreachable block (ram,0x1bbf) */
/* WARNING: Removing unreachable block (ram,0x1bcd) */
/* WARNING: Removing unreachable block (ram,0x1bd8) */
/* WARNING: Removing unreachable block (ram,0x1b18) */
/* WARNING: Removing unreachable block (ram,0x1b26) */
/* WARNING: Removing unreachable block (ram,0x1b30) */
/* WARNING: Removing unreachable block (ram,0x1b37) */
/* WARNING: Removing unreachable block (ram,0x1b3e) */
/* WARNING: Removing unreachable block (ram,0x1b83) */
/* WARNING: Removing unreachable block (ram,0x1b43) */
/* WARNING: Removing unreachable block (ram,0x1be4) */
/* WARNING: Removing unreachable block (ram,0x1bea) */
/* WARNING: Removing unreachable block (ram,0x1bf5) */
/* WARNING: Removing unreachable block (ram,0x1bfa) */
/* WARNING: Removing unreachable block (ram,0x1c02) */
/* WARNING: Removing unreachable block (ram,0x1c03) */
/* WARNING: Removing unreachable block (ram,0x1c07) */
/* WARNING: Removing unreachable block (ram,0x1c0d) */
/* WARNING: Removing unreachable block (ram,0x1c14) */
/* WARNING: Removing unreachable block (ram,0x1c15) */
/* WARNING: Removing unreachable block (ram,0x1c23) */
/* WARNING: Removing unreachable block (ram,0x1c33) */
/* WARNING: Removing unreachable block (ram,0x1b46) */
/* WARNING: Removing unreachable block (ram,0x1b4d) */
/* WARNING: Removing unreachable block (ram,0x1b54) */
/* WARNING: Removing unreachable block (ram,0x1b59) */
/* WARNING: Removing unreachable block (ram,0x1b67) */
/* WARNING: Removing unreachable block (ram,0x1b6c) */
/* WARNING: Removing unreachable block (ram,0x1ca0) */
/* WARNING: Removing unreachable block (ram,0x1ca5) */
/* WARNING: Removing unreachable block (ram,0x1ca6) */
/* WARNING: Removing unreachable block (ram,0x1cab) */
/* WARNING: Removing unreachable block (ram,0x1cac) */
/* WARNING: Removing unreachable block (ram,0x1cb2) */
/* WARNING: Removing unreachable block (ram,0x1cbd) */
/* WARNING: Removing unreachable block (ram,0x1cc2) */
/* WARNING: Removing unreachable block (ram,0x1cca) */
/* WARNING: Removing unreachable block (ram,0x1ccb) */
/* WARNING: Removing unreachable block (ram,0x1ccf) */
/* WARNING: Removing unreachable block (ram,0x1cd5) */
/* WARNING: Removing unreachable block (ram,0x1cd9) */
/* WARNING: Removing unreachable block (ram,0x1cda) */
/* WARNING: Removing unreachable block (ram,0x1ce8) */
/* WARNING: Removing unreachable block (ram,0x1cf3) */
/* WARNING: Removing unreachable block (ram,0x1c41) */
/* WARNING: Removing unreachable block (ram,0x1c46) */
/* WARNING: Removing unreachable block (ram,0x1c47) */
/* WARNING: Removing unreachable block (ram,0x1c4c) */
/* WARNING: Removing unreachable block (ram,0x1c4d) */
/* WARNING: Removing unreachable block (ram,0x1c53) */
/* WARNING: Removing unreachable block (ram,0x1c5e) */
/* WARNING: Removing unreachable block (ram,0x1c63) */
/* WARNING: Removing unreachable block (ram,0x1c6b) */
/* WARNING: Removing unreachable block (ram,0x1c6c) */
/* WARNING: Removing unreachable block (ram,0x1c70) */
/* WARNING: Removing unreachable block (ram,0x1c76) */
/* WARNING: Removing unreachable block (ram,0x1c7a) */
/* WARNING: Removing unreachable block (ram,0x1c7b) */
/* WARNING: Removing unreachable block (ram,0x1c89) */
/* WARNING: Removing unreachable block (ram,0x1c94) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

byte moveLaneObjectsAndCarryFrog(byte *param_1,byte *param_2,char *param_3,undefined2 param_4)

{
  byte bVar1;
  byte bVar2;
  char cVar6;
  short sVar3;
  undefined2 uVar4;
  char cVar7;
  ushort uVar5;
  byte bVar8;
  undefined1 uVar14;
  undefined1 *puVar9;
  byte *pbVar10;
  undefined2 uVar11;
  byte bVar15;
  byte *pbVar12;
  undefined *puVar13;
  undefined1 uVar19;
  undefined1 *puVar16;
  byte *pbVar17;
  byte *pbVar18;
  byte bVar20;
  undefined2 unaff_AF_;
  char *pcVar21;
  
code_r0x14b7:
  bVar1 = DAT_ram_80ff * '\x02';
  puVar13 = (undefined *)(ushort)bVar1;
  bVar2 = (&switchD_ram:14c6::switchdataD_ram_14c7)[(short)puVar13];
  pbVar12 = (byte *)CONCAT11(0x14,bVar2);
  bVar20 = (&BYTE_ram_14c8)[(short)puVar13];
  pbVar18 = *(byte **)(&switchD_ram:14c6::switchdataD_ram_14c7 + (short)puVar13);
  bVar8 = (byte)param_1;
  bVar15 = (byte)((ushort)param_1 >> 8);
  switch(bVar1) {
  case 0:
    pbVar18 = &DAT_ram_819b;
    param_1 = &switchD_ram:14c6::caseD_1a;
    param_3 = &switchD_ram:14c6::caseD_1e;
    param_2 = &switchD_ram:14c6::caseD_22;
    break;
  case 2:
    pbVar18 = &switchD_ram:14c6::caseD_28;
    param_1 = (byte *)&DAT_ram_8109;
    param_3 = &UNK_ram_8010;
    param_2 = &UNK_ram_81a7;
  case 0x36:
switchD_ram_14c6_caseD_36:
    bVar1 = *param_2;
    if ((bVar1 == 0) && (bVar1 = *pbVar18 & 0xf, (*pbVar18 & 0x10) == 0)) {
LAB_ram_1651:
      bVar2 = *param_1;
      do {
        param_1 = param_1 + 1;
        *param_1 = *param_1 - bVar1;
        bVar2 = bVar2 - 1;
      } while (bVar2 != 0);
      cVar7 = *param_3;
      *param_3 = cVar7 - bVar1;
      param_3[2] = cVar7 - bVar1;
      if (DAT_ram_8047 < 0x73) {
        if ((DAT_ram_8047 & 0xf) < 3) {
          if ((DAT_ram_80ff == (byte)((DAT_ram_8047 & 0xf0) - 0x30) >> 4) &&
             ((DAT_ram_8044 = DAT_ram_8044 - bVar1, DAT_ram_8044 < 8 || (0xe6 < DAT_ram_8044)))) {
            DAT_ram_8004 = 1;
          }
        }
        else if ((0xb < (DAT_ram_8047 & 0xf)) &&
                (DAT_ram_80ff == (byte)((DAT_ram_8047 & 0xf0) - 0x20) >> 4)) {
          DAT_ram_8044 = DAT_ram_8044 - bVar1;
        }
      }
      *param_2 = 0;
    }
    else {
      if (bVar1 == 1) {
        bVar1 = 1;
        goto LAB_ram_1651;
      }
      *param_2 = bVar1 - 1;
    }
    bVar1 = DAT_ram_80ff + 1;
    DAT_ram_80ff = bVar1;
    if (10 < bVar1) {
      DAT_ram_80ff = 0;
      return bVar1;
    }
    goto code_r0x14b7;
  case 4:
    pbVar18 = &UNK_ram_819d;
    param_1 = &switchD_ram:14c6::caseD_3c;
    param_3 = &switchD_ram:14c6::caseD_40;
    param_2 = &switchD_ram:14c6::caseD_44;
    break;
  case 6:
    pbVar18 = &switchD_ram:14c6::caseD_4a;
    param_1 = &UNK_ram_811b;
    param_3 = &UNK_ram_8018;
    param_2 = &UNK_ram_81a9;
    break;
  case 8:
    pbVar18 = &UNK_ram_819f;
    param_1 = (byte *)&switchD_ram:14c6::caseD_5e;
    param_3 = &switchD_ram:14c6::caseD_62;
    param_2 = &switchD_ram:14c6::caseD_66;
    goto switchD_ram_14c6_caseD_36;
  case 10:
    goto code_r0x15de;
  case 0xc:
    pbVar18 = &UNK_ram_81a1;
    param_1 = &switchD_ram:14c6::caseD_80;
    param_3 = &switchD_ram:14c6::caseD_84;
    param_2 = &switchD_ram:14c6::caseD_88;
    goto switchD_ram_14c6_caseD_36;
  case 0xe:
    pbVar18 = &switchD_ram:14c6::caseD_8e;
    param_1 = &switchD_ram:0fbd::caseD_b5;
    param_3 = &UNK_ram_8028;
    param_2 = &UNK_ram_81ad;
    break;
  case 0x10:
    pbVar18 = &UNK_ram_81a3;
    param_1 = &switchD_ram:14c6::caseD_a2;
    param_3 = &switchD_ram:14c6::caseD_a6;
    param_2 = &switchD_ram:14c6::caseD_aa;
    goto switchD_ram_14c6_caseD_36;
  case 0x12:
    pbVar18 = &switchD_ram:14c6::caseD_b0;
    param_1 = &switchD_ram:0fbd::caseD_d5;
    param_3 = &UNK_ram_8030;
    param_2 = &UNK_ram_81af;
    break;
  case 0x14:
    pbVar18 = &UNK_ram_81a5;
    param_1 = &switchD_ram:14c6::caseD_c4;
    param_3 = &switchD_ram:14c6::caseD_c8;
    param_2 = &switchD_ram:14c6::caseD_cc;
    goto switchD_ram_14c6_caseD_36;
  case 0x16:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  default:
    uVar4 = 0x1114;
    switchD_ram:0fbd::caseD_5a = bVar1 | bVar8;
    uVar11 = CONCAT11(bVar15 + 1,bVar8);
code_r0x1186:
    _caseD_36 = CONCAT21(uVar11,switchD_ram:0fbd::caseD_36);
    bVar1 = renderFrogAnimTileColumns
                      (uVar4,pbVar18,&switchD_ram:14c6::caseD_c4,&switchD_ram:14c6::caseD_c4);
    return bVar1;
  case 0x1a:
  case 0xee:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x1c:
  case 0x3e:
  case 0x60:
  case 0x82:
  case 0xa4:
  case 0xc6:
    goto switchD_ram_14c6_caseD_1c;
  case 0x1e:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x20:
  case 0x42:
  case 100:
  case 0x86:
  case 0xa8:
  case 0xca:
    _UNK_ram_32af = (short)pbVar18 * 2;
    bVar1 = FUN_ram_2229((char)((ushort)unaff_AF_ >> 8) + bVar2);
    return bVar1;
  case 0x22:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x24:
  case 0x46:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x26:
  case 0x48:
    pbVar12 = (byte *)0x1300;
    goto switchD_ram_14c6_caseD_8c;
  case 0x28:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x2a:
    if ((bVar1 == 0) && ((DAT_ram_2e08 == param_1 || (DAT_ram_2e08 < param_1)))) {
      DAT_ram_83cf = bVar1;
      *pbVar12 = 1;
      pcVar21 = (char *)CONCAT11(0x14,bVar2 - 2);
      cVar7 = *pcVar21 + '\x01';
      *pcVar21 = cVar7;
      puVar13 = &UNK_ram_abde;
      do {
        puVar13 = puVar13 + -0x20;
        cVar7 = cVar7 + -1;
      } while (cVar7 != '\0');
      *puVar13 = 0x4d;
      bVar1 = enqueueSoundCommand(7);
    }
    if (param_1 <= DAT_ram_83ef) {
      return bVar1;
    }
    DAT_ram_83ef = param_1;
    return bVar1;
  case 0x2c:
  case 0x4e:
  case 0x70:
  case 0x92:
  case 0xb4:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x2e:
    goto switchD_ram_14c6_caseD_2e;
  case 0x30:
  case 0x52:
  case 0x74:
  case 0x96:
  case 0xb8:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x32:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x34:
  case 0x56:
  case 0x78:
  case 0x9a:
  case 0xbc:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x38:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x3c:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x40:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x44:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x4a:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x4c:
    return bVar1;
  case 0x50:
    DAT_ram_81b4 = 0x15;
    bVar1 = DAT_ram_81b3 - 9;
    if (bVar1 == 0) {
      DAT_ram_81b3 = bVar1;
      return 0;
    }
    pbVar12 = &DAT_ram_819b;
    sVar3 = 0xb;
    DAT_ram_81b3 = DAT_ram_81b3 + '\x01';
    do {
      *pbVar12 = *pbVar18;
      pbVar12 = pbVar12 + 1;
      pbVar18 = pbVar18 + 1;
      sVar3 = sVar3 + -1;
    } while (sVar3 != 0);
    return bVar1;
  case 0x54:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x58:
  case 0x7a:
  case 0x9c:
  case 0xbe:
    break;
  case 0x5a:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x5e:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x62:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x66:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x68:
  case 0x8a:
  case 0x90:
  case 0xac:
  case 0xb6:
  case 0xce:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x6a:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x6c:
    goto code_r0x15de;
  case 0x6e:
    bVar2 = *pbVar18;
    *pbVar18 = bVar1;
    if (bVar1 == 0) {
      DAT_ram_b818 = 1;
      DAT_ram_837e = '\x04';
    }
                    /* WARNING: Could not recover jumptable at 0x2d22. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    bVar1 = (*(dispatch_2d23 + CONCAT11(bVar2,bVar8)))();
    return bVar1;
  case 0x72:
    *pbVar12 = bVar1;
    DAT_ram_8111 = DAT_ram_8111 + 2;
    DAT_ram_8119 = bVar1;
    if (DAT_ram_8111 < 0xa0) {
      blitScrollBand();
    }
    DAT_ram_826e = DAT_ram_826e + 1;
    if (DAT_ram_826e == 0x10) {
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
      blitScrollTileGrid(CONCAT11(DAT_ram_8274,DAT_ram_811a),&UNK_ram_1423);
      puVar13 = &UNK_ram_145f;
    }
    else if (DAT_ram_826e == 0x20) {
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
      blitScrollTileGrid(CONCAT11(DAT_ram_8274,DAT_ram_811a),&UNK_ram_142b);
      puVar13 = &UNK_ram_1473;
    }
    else {
      if (DAT_ram_826e != 0x30) {
        return DAT_ram_826e;
      }
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
      DAT_ram_826e = 0;
      blitScrollTileGrid(CONCAT11(DAT_ram_8274,DAT_ram_811a),&UNK_ram_1433);
      puVar13 = &UNK_ram_1487;
    }
    puVar9 = switchD_ram:0fbd::caseD_83;
    bVar1 = DAT_ram_8119;
    bVar2 = DAT_ram_827d;
    DAT_ram_8003 = DAT_ram_827d;
    switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_7f;
    switchD_ram:0fbd::caseD_40 = puVar13;
    do {
      do {
        *puVar9 = *puVar13;
        puVar9[1] = puVar13[1];
        puVar9 = puVar9 + 0x20;
        bVar2 = bVar2 - 1;
        puVar13 = puVar13 + 2;
      } while (bVar2 != 0);
      puVar9 = puVar9 + switchD_ram:0fbd::caseD_5a;
      bVar1 = bVar1 - 1;
      puVar13 = switchD_ram:0fbd::caseD_40;
      bVar2 = DAT_ram_8003;
    } while (bVar1 != 0);
    return DAT_ram_8003;
  case 0x76:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x7c:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x80:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x84:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x88:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x8c:
  case 0xae:
    goto switchD_ram_14c6_caseD_8c;
  case 0x8e:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x94:
    DAT_ram_814e = bVar1 + 2;
    uVar5 = (ushort)DAT_ram_8145;
    DAT_ram_8145 = DAT_ram_8145 + 0x20;
    param_1[uVar5] = pbVar18[(short)pbVar12];
    (param_1 + uVar5)[1] = (pbVar18 + (short)pbVar12)[1];
    if (DAT_ram_814e < 0x10) {
      return DAT_ram_814e;
    }
    DAT_ram_814f = 0;
    DAT_ram_814e = 0;
    DAT_ram_8145 = 0;
    DAT_ram_8146 = 0;
    DAT_ram_8147 = 0;
    return 0;
  case 0x98:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0x9e:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xa2:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xa6:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xaa:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xb0:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xb2:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xba:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xc0:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xc4:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 200:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xcc:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xd0:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xd2:
    *param_1 = *pbVar18;
    pbVar12 = (byte *)CONCAT11(bVar20,bVar2 + 1);
    pbVar18 = (byte *)CONCAT11(bVar15,bVar8 + 1);
    cVar7 = '\x1c';
    do {
      bVar1 = *pbVar12 >> 1;
      bVar2 = (byte)(bVar1 | *pbVar12 << 7) >> 1;
      bVar1 = (byte)(bVar2 | bVar1 << 7) >> 1;
      *pbVar18 = (byte)(bVar1 | bVar2 << 7) >> 1 | bVar1 << 7;
      uVar19 = (undefined1)((ushort)pbVar12 >> 8);
      uVar14 = (undefined1)((ushort)pbVar18 >> 8);
      *(undefined1 *)CONCAT11(uVar14,(char)pbVar18 + '\x01') =
           *(undefined1 *)CONCAT11(uVar19,(char)pbVar12 + '\x01');
      pbVar12 = (byte *)CONCAT11(uVar19,(char)pbVar12 + '\x02');
      pbVar18 = (byte *)CONCAT11(uVar14,(char)pbVar18 + '\x02');
      cVar7 = cVar7 + -1;
    } while (cVar7 != '\0');
    cVar7 = '\b';
    if (DAT_ram_842f != '\0') {
      cVar7 = '\x06';
      pbVar18 = (byte *)CONCAT11(uVar14,0x48);
      pbVar12 = (byte *)CONCAT11(uVar19,0x48);
    }
    do {
      bVar1 = *pbVar12 >> 1;
      bVar2 = (byte)(bVar1 | *pbVar12 << 7) >> 1;
      bVar1 = (byte)(bVar2 | bVar1 << 7) >> 1;
      *pbVar18 = (byte)(bVar1 | bVar2 << 7) >> 1 | bVar1 << 7;
      pbVar12 = (byte *)CONCAT11((char)((ushort)pbVar12 >> 8),(char)pbVar12 + '\x01');
      pbVar18 = (byte *)CONCAT11((char)((ushort)pbVar18 >> 8),(char)pbVar18 + '\x01');
      cVar6 = '\x03';
      do {
        *pbVar18 = *pbVar12;
        pbVar12 = (byte *)CONCAT11((char)((ushort)pbVar12 >> 8),(char)pbVar12 + '\x01');
        pbVar18 = (byte *)CONCAT11((char)((ushort)pbVar18 >> 8),(char)pbVar18 + '\x01');
        cVar6 = cVar6 + -1;
      } while (cVar6 != '\0');
      cVar7 = cVar7 + -1;
    } while (cVar7 != '\0');
    if ((DAT_ram_837f != '\0') && (DAT_ram_837f = DAT_ram_837f + -1, DAT_ram_837f == '\0')) {
      DAT_ram_b81c = 0;
    }
    if ((DAT_ram_837e != '\0') && (DAT_ram_837e = DAT_ram_837e + -1, DAT_ram_837e == '\0')) {
      DAT_ram_b818 = 0;
    }
    if (((((DAT_ram_e004 & 8) != 0) && (DAT_ram_83fe != '\0')) && (DAT_ram_83fd != '\0')) &&
       (DAT_ram_83fd != '\x01')) {
      DAT_ram_b043 = DAT_ram_8043 + '\x02';
      DAT_ram_b047 = DAT_ram_8047 + 2;
    }
    if (DAT_ram_83fe == '\0') {
      if (DAT_ram_83d6 < 2) {
        if (DAT_ram_83d6 == 0) {
          driveAttractDemoSequencer();
        }
        driveInPlayFrameUpdate();
        DAT_ram_83cd = 0;
        DAT_ram_83cf = 0;
        DAT_ram_83b5 = '\0';
        _DAT_ram_8293 = 0;
        puVar16 = &DAT_ram_825c;
        puVar9 = &DAT_ram_825d;
        sVar3 = 0xb;
        DAT_ram_825c = '\0';
        do {
          *puVar9 = *puVar16;
          puVar9 = puVar9 + 1;
          puVar16 = puVar16 + 1;
          sVar3 = sVar3 + -1;
        } while (sVar3 != 0);
        UNK_ram_83af = 0x80;
        UNK_ram_83b0 = 0;
        UNK_ram_83b1 = 0;
      }
      else if (((DAT_ram_83d8 != '\0') && (DAT_ram_83d8 = DAT_ram_83d8 + -1, DAT_ram_83d8 == '\0'))
              && (DAT_ram_83d7 == '\0')) {
        DAT_ram_83d6 = DAT_ram_83d6 - 1;
      }
    }
    else {
      dequeueSoundCommand();
      if (DAT_ram_83ea != '\0') {
        if ((char)((ushort)DAT_ram_83d2 >> 8) == '\0' && (char)DAT_ram_83d2 == '\0') {
          if (((char)((ushort)DAT_ram_8382 >> 8) != '\0' || (char)DAT_ram_8382 != '\0') &&
             (DAT_ram_8382 = DAT_ram_8382 + -1, DAT_ram_8382 == 0)) {
            enqueueSoundCommand(0xf);
            enqueueSoundCommand(0xb0);
            DAT_ram_8371 = 0;
          }
          bVar1 = DAT_ram_825d;
          if (DAT_ram_83fd == '\x01') {
            if (DAT_ram_825c == '\x05') {
              puVar16 = &DAT_ram_825e;
              puVar9 = &DAT_ram_825f;
              sVar3 = 4;
              DAT_ram_825e = 0;
              do {
                *puVar9 = *puVar16;
                puVar9 = puVar9 + 1;
                puVar16 = puVar16 + 1;
                sVar3 = sVar3 + -1;
              } while (sVar3 != 0);
              DAT_ram_825c = '\0';
              armBoardCompleteReveal();
              goto LAB_ram_0245;
            }
          }
          else {
switchD_ram_14c6_caseD_f4:
            if (bVar1 == 5) {
              puVar16 = &DAT_ram_8263;
              puVar9 = &DAT_ram_8264;
              sVar3 = 4;
              DAT_ram_8263 = 0;
              do {
                *puVar9 = *puVar16;
                puVar9 = puVar9 + 1;
                puVar16 = puVar16 + 1;
                sVar3 = sVar3 + -1;
              } while (sVar3 != 0);
              DAT_ram_825d = 0;
              armBoardCompleteReveal();
              goto LAB_ram_0245;
            }
          }
          if (DAT_ram_8298 == '\0') {
            if (DAT_ram_8297 == '\0') {
              if ((char)((ushort)DAT_ram_829d >> 8) == '\0' && (char)DAT_ram_829d == '\0') {
                driveScoreDisplayCountdown();
                orchestrateCollisionsAndFrogInput();
                if (DAT_ram_83b5 == '\0') {
                  DAT_ram_83b5 = '\x01';
                  DAT_ram_8384 = -1;
                  if (DAT_ram_8380 != '\0') {
                    DAT_ram_8380 = '\0';
                    DAT_ram_8382 = 0x40;
                    copyRunUpTileColumn(&UNK_ram_aa51,&UNK_ram_2f7b);
                  }
                }
              }
            }
            else {
              DAT_ram_8297 = DAT_ram_8297 + -1;
            }
          }
          else {
            DAT_ram_8298 = DAT_ram_8298 + -1;
          }
          if ((DAT_ram_8384 != '\0') && (DAT_ram_8384 = DAT_ram_8384 + -1, DAT_ram_8384 == '\0')) {
            blitFourTileGroupColumn(&UNK_ram_a850);
          }
          advanceScrollLaneObjects();
          advanceAnimationFrameBuffer();
          if (DAT_ram_8107 != '\0') {
            DAT_ram_8109 = DAT_ram_8109 + -1;
          }
          if (DAT_ram_8108 != '\0') {
            switchD_ram:14c6::caseD_5e = switchD_ram:14c6::caseD_5e + -1;
          }
          dispatchFrogMoveAgainstLanes();
          if (DAT_ram_8107 != '\0') {
            DAT_ram_8109 = DAT_ram_8109 + '\x01';
          }
          if (DAT_ram_8108 != '\0') {
            switchD_ram:14c6::caseD_5e = switchD_ram:14c6::caseD_5e + '\x01';
          }
          driveFrogDeathAnimation();
          moveLaneObjectsAndCarryFrog();
          driveSpriteObjectCluster();
          tickGatedCountdown();
          tickFrogRespawnDelay();
          if (DAT_ram_8297 != '\0') {
            stampHomeBayFrogByColumn();
          }
        }
        else {
          DAT_ram_83d2 = DAT_ram_83d2 + -1;
          moveLaneObjectsAndCarryFrog();
          advanceAnimationFrameBuffer();
        }
      }
    }
LAB_ram_0245:
    DAT_ram_b808 = 1;
    return (byte)((ushort)param_4 >> 8);
  case 0xd4:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xd6:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xd8:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xda:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xdc:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xde:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xe0:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xe2:
    goto switchD_ram_14c6_caseD_e2;
  case 0xe4:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xe6:
    DAT_ram_8035 = DAT_ram_80ff << 2 | (DAT_ram_80ff & 0x7f) >> 6;
    DAT_ram_8021 = 6;
    DAT_ram_8023 = 6;
    DAT_ram_8039 = 6;
    DAT_ram_803b = 6;
    cVar7 = '\n';
    puVar9 = &DAT_ram_800d;
    DAT_ram_8037 = DAT_ram_8035;
    do {
      *puVar9 = 5;
      puVar9 = puVar9 + 2;
      cVar7 = cVar7 + -1;
    } while (cVar7 != '\0');
    DAT_ram_8029 = 5;
    DAT_ram_802b = 5;
    DAT_ram_8031 = 5;
    DAT_ram_8033 = 5;
    DAT_ram_800d = 2;
    DAT_ram_800f = 2;
    DAT_ram_8015 = 2;
    DAT_ram_8017 = 2;
    DAT_ram_8019 = 2;
    DAT_ram_801b = 2;
    return 2;
  case 0xe8:
    if (puVar13 <= &UNK_ram_eb38) goto LAB_ram_1284;
    goto LAB_ram_12a1;
  case 0xea:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xec:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xf0:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xf2:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xf4:
    goto switchD_ram_14c6_caseD_f4;
  case 0xf6:
  case 0xfe:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xf8:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xfa:
                    /* WARNING: Bad instruction - Truncating control flow here */
    halt_baddata();
  case 0xfc:
    goto switchD_ram_14c6_caseD_fc;
  }
  pbVar12 = (byte *)CONCAT11(0x14,*param_2);
  if (*param_2 == 0) {
    bVar1 = *pbVar18;
    pbVar12 = (byte *)(CONCAT11(bVar1,bVar1) & 0xff0f);
    cVar7 = (char)pbVar12;
    if ((bVar1 & 0x10) != 0) goto switchD_ram_14c6_caseD_e2;
  }
  else {
switchD_ram_14c6_caseD_e2:
    if ((char)pbVar12 != '\x01') {
      *param_2 = (char)pbVar12 - 1;
      bVar1 = switchD_ram:14c6::caseD_6c();
      return bVar1;
    }
    cVar7 = '\x01';
  }
  bVar1 = *param_1;
  do {
    param_1 = param_1 + 1;
    *param_1 = *param_1 + cVar7;
    bVar1 = bVar1 - 1;
  } while (bVar1 != 0);
  cVar6 = *param_3;
  *param_3 = cVar6 + cVar7;
  param_3[2] = cVar6 + cVar7;
  if ((0x2f < DAT_ram_8047) && (DAT_ram_8047 < 0x73)) {
    if ((DAT_ram_8047 & 0xf) < 3) {
      if (((DAT_ram_80ff == (byte)((DAT_ram_8047 & 0xf0) - 0x30) >> 4) && (0x2f < DAT_ram_8047)) &&
         ((DAT_ram_8044 = DAT_ram_8044 + cVar7, DAT_ram_8044 < 8 || (0xe6 < DAT_ram_8044)))) {
        DAT_ram_8004 = 1;
      }
    }
    else if ((0xb < (DAT_ram_8047 & 0xf)) &&
            (DAT_ram_80ff == (byte)((DAT_ram_8047 & 0xf0) - 0x20) >> 4)) {
      DAT_ram_8044 = DAT_ram_8044 + cVar7;
    }
  }
switchD_ram_14c6_caseD_fc:
  *param_2 = 0;
code_r0x15de:
  bVar1 = DAT_ram_80ff + 1;
  DAT_ram_80ff = bVar1;
  if (10 < bVar1) {
    DAT_ram_80ff = 0;
    return bVar1;
  }
  goto code_r0x14b7;
  while (bVar1 = (char)((ushort)pbVar12 >> 8) - 1, pbVar12 = (byte *)((ushort)bVar1 << 8),
        bVar1 != 0) {
LAB_ram_12a1:
    pbVar18 = pbVar18 + 1;
    if ((bVar15 <= *pbVar18) || (*pbVar18 < bVar8)) {
      if (0x7f < DAT_ram_8047) {
        bVar1 = dispatchFrogMoveAgainstLanes();
        return bVar1;
      }
      goto code_r0x12e4;
    }
  }
  if (0x7f < DAT_ram_8047) {
    bVar1 = resolveFrogMoveAgainstLanes();
    return bVar1;
  }
  goto code_r0x12d0;
switchD_ram_14c6_caseD_2e:
  uVar14 = DAT_ram_8271;
  pbVar17 = DAT_ram_13ed;
  cVar7 = (char)pbVar12 + -1;
  if (cVar7 != '\0') {
    bVar1 = renderFrogAnimTileColumns
                      (CONCAT11(DAT_ram_8003,cVar7),pbVar18 + bVar1,switchD_ram:0fbd::caseD_40);
    return bVar1;
  }
  do {
    cVar7 = switchD_ram:0fbd::caseD_36 + '\x01';
    uVar5 = (ushort)switchD_ram:0fbd::caseD_40;
    _caseD_36 = CONCAT21(switchD_ram:0fbd::caseD_40,cVar7);
    switch(_caseD_36 & 0xff) {
    case 0:
      goto renderFrogAnimArm0;
    case 1:
      blitFrogAnimColumnOnTrigger();
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
      _caseD_36 = CONCAT21(&UNK_ram_1423,switchD_ram:0fbd::caseD_36);
      bVar1 = renderFrogAnimTileColumns
                        (CONCAT11(DAT_ram_8274,DAT_ram_8275),switchD_ram:0fbd::caseD_53,
                         &DAT_ram_8109,&DAT_ram_8109);
      return bVar1;
    case 2:
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_5f;
      _caseD_36 = CONCAT21(&UNK_ram_143b,cVar7);
      bVar1 = renderFrogAnimTileColumns
                        (CONCAT11(DAT_ram_8277,DAT_ram_8278),uRam13f1,&switchD_ram:14c6::caseD_3c,
                         &switchD_ram:14c6::caseD_3c);
      return bVar1;
    case 3:
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_6f;
      _caseD_36 = CONCAT21(&UNK_ram_1453,cVar7);
      bVar1 = renderFrogAnimTileColumns
                        (CONCAT11(DAT_ram_827a,DAT_ram_827b),uRam13f3,&UNK_ram_811b,&UNK_ram_811b);
      return bVar1;
    case 4:
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_7f;
      _caseD_36 = CONCAT21(&UNK_ram_145f,cVar7);
      bVar1 = renderFrogAnimTileColumns
                        (CONCAT11(DAT_ram_827d,DAT_ram_827e),switchD_ram:0fbd::caseD_83,
                         &switchD_ram:14c6::caseD_5e,&switchD_ram:14c6::caseD_5e);
      return bVar1;
    case 5:
      break;
    case 6:
      switchD_ram:0fbd::caseD_5a = DAT_ram_8282;
      _caseD_36 = CONCAT21(0x149f,cVar7);
      bVar1 = renderFrogAnimTileColumns
                        (CONCAT11(DAT_ram_8283,DAT_ram_8284),DAT_ram_13f9,
                         &switchD_ram:14c6::caseD_80,&switchD_ram:14c6::caseD_80);
      return bVar1;
    case 7:
      switchD_ram:0fbd::caseD_5a = DAT_ram_8285;
      _caseD_36 = CONCAT21(0x14a7,cVar7);
      bVar1 = renderFrogAnimTileColumns
                        (CONCAT11(DAT_ram_8286,DAT_ram_8287),DAT_ram_13fb,
                         &switchD_ram:0fbd::caseD_b5,&switchD_ram:0fbd::caseD_b5);
      return bVar1;
    case 8:
      switchD_ram:0fbd::caseD_5a = DAT_ram_8288;
      _caseD_36 = CONCAT21(0x14ab,cVar7);
      bVar1 = renderFrogAnimTileColumns
                        (CONCAT11(DAT_ram_8289,DAT_ram_828a),DAT_ram_13fd,
                         &switchD_ram:14c6::caseD_a2,&switchD_ram:14c6::caseD_a2);
      return bVar1;
    case 9:
      switchD_ram:0fbd::caseD_5a = DAT_ram_828b;
      _caseD_36 = CONCAT21(0x14af,cVar7);
      bVar1 = renderFrogAnimTileColumns
                        (CONCAT11(DAT_ram_828c,DAT_ram_828d),DAT_ram_13ff,
                         &switchD_ram:0fbd::caseD_d5,&switchD_ram:0fbd::caseD_d5);
      return bVar1;
    case 10:
      uVar4 = CONCAT11(DAT_ram_828f,DAT_ram_8290);
      uVar11 = 0x14b3;
      pbVar18 = _UNK_ram_1401;
      switchD_ram:0fbd::caseD_5a = DAT_ram_828e;
      goto code_r0x1186;
    default:
      _caseD_36 = (uint3)uVar5 << 8;
      return 0;
    }
  } while( true );
renderFrogAnimArm0:
  pbVar12 = (byte *)CONCAT11(DAT_ram_8271,DAT_ram_8272);
  puVar9 = &switchD_ram:14c6::caseD_1a;
  pcVar21 = &switchD_ram:14c6::caseD_1a;
  switchD_ram:0fbd::caseD_5a = DAT_ram_8270;
  _caseD_36 = CONCAT21(&switchD_ram:0fbd::caseD_11,cVar7);
  pbVar18 = pbVar12;
  computeVramColumnIndex();
  if (switchD_ram:0fbd::caseD_1d == '\0') {
    puVar9[1] = ~(byte)pbVar18 + 1;
    *pcVar21 = *pcVar21 + '\x01';
  }
  pbVar10 = &switchD_ram:0fbd::caseD_11;
  DAT_ram_8003 = uVar14;
  while( true ) {
    *pbVar17 = *pbVar10;
    pbVar17[1] = pbVar10[1];
    pbVar17 = pbVar17 + 0x20;
    cVar7 = (char)((ushort)pbVar12 >> 8) + -1;
    pbVar12 = (byte *)CONCAT11(cVar7,(char)pbVar12);
    pbVar18 = pbVar17;
    bVar1 = switchD_ram:0fbd::caseD_5a;
    if (cVar7 == '\0') break;
    pbVar10 = pbVar10 + 2;
  }
  goto switchD_ram_14c6_caseD_2e;
switchD_ram_14c6_caseD_1c:
  while( true ) {
    cVar7 = (char)pbVar12;
    pbVar18 = (byte *)((short)pbVar18 * 2);
    pbRam0206 = pbVar18;
    FUN_ram_2219(&UNK_ram_2231);
    if ((char)(cVar7 + -1) == '\0') break;
    pbVar12 = (byte *)CONCAT11(2,cVar7 + -1);
  }
  bVar1 = FUN_ram_2229();
  return bVar1;
switchD_ram_14c6_caseD_8c:
  do {
    pbVar18 = pbVar18 + (short)param_1;
    bVar1 = (char)((ushort)pbVar12 >> 8) - 1;
    pbVar12 = (byte *)((ushort)bVar1 << 8);
  } while (bVar1 != 0);
  cVar7 = '\x02';
  if (DAT_ram_8110 == 'P') {
LAB_ram_213e:
    do {
      FUN_ram_2178(&UNK_ram_2190);
      cVar7 = cVar7 + -1;
    } while (cVar7 != '\0');
    bVar1 = FUN_ram_2188();
    return bVar1;
  }
  if (DAT_ram_8110 != -0x80) {
    if (DAT_ram_8110 == -0x60) {
      do {
        FUN_ram_2178(&UNK_ram_2198);
        cVar7 = cVar7 + -1;
      } while (cVar7 != '\0');
      DAT_ram_8107 = 1;
      bVar1 = FUN_ram_2188();
      return bVar1;
    }
    if (DAT_ram_8110 != -0x50) {
      if (DAT_ram_8110 != -0x30) {
        bVar1 = FUN_ram_2188(pbVar18 + -0x57f8);
        return bVar1;
      }
      goto LAB_ram_213e;
    }
  }
  do {
    FUN_ram_2178(&UNK_ram_2194);
    cVar7 = cVar7 + -1;
  } while (cVar7 != '\0');
  if (DAT_ram_8107 != '\0') {
    DAT_ram_8107 = 0;
    bVar1 = FUN_ram_2188();
    return bVar1;
  }
  DAT_ram_811a = param_3[2] - 1U;
  return param_3[2] - 1U;
  while (bVar1 = (char)((ushort)pbVar12 >> 8) - 1, pbVar12 = (byte *)((ushort)bVar1 << 8),
        bVar1 != 0) {
LAB_ram_1284:
    pbVar18 = pbVar18 + 1;
    if ((bVar15 <= *pbVar18) && (*pbVar18 < bVar8)) {
      if (0x7f < DAT_ram_8047) {
        bVar1 = dispatchFrogMoveAgainstLanes();
        return bVar1;
      }
code_r0x12e4:
      if (DAT_ram_8004 != 0) {
        return DAT_ram_8004;
      }
      bVar1 = DAT_ram_8047 + 0xf & 0xf;
      if (4 < bVar1) {
                    /* WARNING: Could not recover jumptable at 0x130a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        bVar1 = (**(code **)(&DAT_ram_130b + (ushort)((byte)(DAT_ram_8047 + 0xf) >> 4) * 2))();
        return bVar1;
      }
      return bVar1;
    }
  }
  if (0x7f < DAT_ram_8047) {
    bVar1 = resolveFrogMoveAgainstLanes();
    return bVar1;
  }
code_r0x12d0:
  DAT_ram_8004 = 1;
  if (0x7f < DAT_ram_8047) {
    return DAT_ram_8047;
  }
  if (DAT_ram_8047 < 0x30) {
    return DAT_ram_8047;
  }
  DAT_ram_829c = 1;
  return 1;
}

