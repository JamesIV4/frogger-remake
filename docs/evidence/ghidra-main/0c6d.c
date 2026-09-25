
/* WARNING: Instruction at (ram,0x0d79) overlaps instruction at (ram,0x0d78)
    */
/* WARNING: Removing unreachable block (ram,0x0ca1) */
/* WARNING: Removing unreachable block (ram,0x06cd) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

byte renderMode4PointTablePhase(byte *param_1)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  byte bVar4;
  byte bVar5;
  undefined1 uVar6;
  char cVar8;
  undefined2 uVar7;
  char cVar9;
  undefined *puVar10;
  char cVar11;
  undefined1 *puVar12;
  undefined2 *puVar13;
  byte *pbVar14;
  char cVar15;
  short unaff_AF_;
  
code_r0x0c6d:
  if (DAT_ram_83d7 == '\0') {
    DAT_ram_83d7 = '\x05';
  }
  DAT_ram_83d7 = DAT_ram_83d7 + -1;
  bVar5 = DAT_ram_83d7 * '\x02';
  bVar1 = bVar5 == 0;
  puVar10 = (undefined *)(ushort)bVar5;
  bVar2 = ((ushort)(puVar10 + 0xc82) & 0x1000) != 0;
  bVar3 = &UNK_ram_f37d < puVar10;
  pbVar14 = puVar10 + 0xc82;
  cVar8 = (char)param_1;
  cVar15 = (char)((ushort)param_1 >> 8);
  bVar4 = bVar5;
  switch(bVar5) {
  case 0:
    bVar5 = dispatch_0cc0();
    return bVar5;
  case 2:
    bVar5 = dispatch_0c51();
    return bVar5;
  case 4:
    bVar5 = dispatch_0ceb();
    return bVar5;
  case 6:
    bVar5 = dispatch_0cc6();
    return bVar5;
  case 8:
    bVar5 = 6;
  case 10:
    DAT_ram_801d = bVar5;
    DAT_ram_8023 = bVar5;
    break;
  case 0xc:
    bVar5 = bVar5 + cVar15;
    DAT_ram_8023 = bVar5;
    break;
  case 0xe:
    bVar5 = bVar5 + cVar15;
    break;
  case 0x12:
    DAT_ram_802f = bVar5 + cVar15;
    goto switchD_ram_0c81_caseD_16;
  case 0x14:
  case 0x16:
    goto switchD_ram_0c81_caseD_16;
  case 0x18:
    goto switchD_ram_0c81_caseD_18;
  case 0x1a:
    bVar5 = bVar5 + cVar15;
    DAT_ram_8021 = bVar5;
    goto switchD_ram_0c81_caseD_1e;
  case 0x1c:
    bVar5 = BCDadjust(bVar5,bVar3,bVar2);
    BCDadjustCarry(bVar5,bVar3,bVar2);
    hasEvenParity(bVar5);
  case 0x20:
    DAT_ram_802d = bVar5 + cVar15;
    goto switchD_ram_0c81_caseD_24;
  case 0x1e:
    goto switchD_ram_0c81_caseD_1e;
  case 0x22:
  case 0x24:
    goto switchD_ram_0c81_caseD_24;
  case 0x26:
    goto switchD_ram_0c81_caseD_26;
  case 0x28:
    bVar5 = 0;
    goto code_r0x0cab;
  case 0x2a:
  case 0x2c:
    goto switchD_ram_0c81_caseD_2c;
  case 0x2e:
    goto code_r0x0cb3;
  case 0x30:
    goto code_r0x0cb3;
  case 0x32:
    goto switchD_ram_0c81_caseD_32;
  case 0x34:
  case 0x36:
    DAT_ram_83d8 = 0x80;
    return bVar5;
  case 0x38:
    goto switchD_ram_0c81_caseD_38;
  case 0x3a:
    DAT_ram_83d8 = 0x80;
    return DAT_ram_83d7 * '\x04';
  case 0x3c:
    return bVar5 + cVar15;
  case 0x3e:
    pbVar14 = &DAT_ram_83d8;
    goto code_r0x0cc3;
  case 0x40:
    bVar5 = DAT_ram_83d7 * '\x04';
code_r0x0cc3:
    *pbVar14 = 0xc0;
    return bVar5;
  case 0x42:
    if (!bVar1) {
      return bVar5;
    }
    return 0;
  case 0x44:
    pbVar14 = &UNK_ram_ab70;
    goto code_r0x0cc9;
  case 0x46:
code_r0x0cc9:
    bVar5 = 0x50;
code_r0x0ccb:
    writePackedBcdByte(bVar5,pbVar14);
  case 0x4c:
switchD_ram_0c81_caseD_4c:
    puVar10 = &UNK_ram_2fba;
code_r0x0cd3:
    bVar5 = copyRunUpTileColumn(puVar10);
switchD_ram_0c81_caseD_52:
    puVar10 = &UNK_ram_2f43;
code_r0x0cd9:
    copyRunUpTileColumn(bVar5,puVar10);
switchD_ram_0c81_caseD_58:
    puVar10 = &UNK_ram_2fae;
code_r0x0cdf:
    copyRunUpTileColumn(puVar10);
switchD_ram_0c81_caseD_5e:
    pbVar14 = &UNK_ram_ab71;
code_r0x0ce3:
    puVar10 = &UNK_ram_2f17;
switchD_ram_0c81_caseD_64:
switchD_ram_0c81_caseD_66:
    bVar5 = copyRunUpTileColumn(pbVar14,puVar10);
    DAT_ram_83d8 = 0x80;
    return bVar5;
  case 0x48:
    goto code_r0x0ccb;
  case 0x4a:
    goto switchD_ram_0c81_caseD_4c;
  case 0x4e:
    goto code_r0x0cd3;
  case 0x50:
    goto code_r0x0cd3;
  case 0x52:
    goto switchD_ram_0c81_caseD_52;
  case 0x54:
    bVar5 = ~bVar5;
    goto code_r0x0cd9;
  case 0x56:
    bVar5 = *param_1;
    goto code_r0x0cd9;
  case 0x58:
    goto switchD_ram_0c81_caseD_58;
  case 0x5a:
    goto code_r0x0cdf;
  case 0x5c:
    goto code_r0x0cdf;
  case 0x5e:
    goto switchD_ram_0c81_caseD_5e;
  case 0x60:
    goto code_r0x0ce3;
  case 0x62:
  case 100:
    goto switchD_ram_0c81_caseD_64;
  case 0x66:
    goto switchD_ram_0c81_caseD_66;
  case 0x68:
    RST1();
    pbVar14 = &UNK_ram_ab73;
  case 0x6c:
switchD_ram_0c81_caseD_6c:
    writePackedBcdWord(pbVar14,0x1000);
switchD_ram_0c81_caseD_72:
    puVar10 = &UNK_ram_2fba;
code_r0x0cf9:
    bVar5 = copyRunUpTileColumn(puVar10);
switchD_ram_0c81_caseD_78:
    puVar10 = &UNK_ram_2f39;
code_r0x0cff:
    copyRunUpTileColumn(bVar5,puVar10);
switchD_ram_0c81_caseD_7e:
    puVar10 = &UNK_ram_2fae;
code_r0x0d03:
    copyRunUpTileColumn(puVar10);
switchD_ram_0c81_caseD_84:
    pbVar14 = &UNK_ram_ab74;
code_r0x0d09:
    puVar10 = &UNK_ram_2f2a;
switchD_ram_0c81_caseD_8a:
switchD_ram_0c81_caseD_8c:
    bVar5 = copyRunUpTileColumn(pbVar14,puVar10);
    DAT_ram_83d8 = 0x80;
    return bVar5;
  case 0x6a:
    *pbVar14 = bVar5;
    goto switchD_ram_0c81_caseD_6c;
  case 0x6e:
    if (cVar15 != '\x01') {
      return bVar5;
    }
  case 0x70:
  case 0x72:
    goto switchD_ram_0c81_caseD_72;
  case 0x74:
    goto code_r0x0cf9;
  case 0x76:
    goto code_r0x0cf9;
  case 0x78:
    goto switchD_ram_0c81_caseD_78;
  case 0x7a:
    bVar5 = ~bVar5;
    goto code_r0x0cff;
  case 0x7c:
    bVar5 = *param_1;
    goto code_r0x0cff;
  case 0x7e:
    goto switchD_ram_0c81_caseD_7e;
  case 0x80:
    goto code_r0x0d03;
  case 0x82:
  case 0x84:
    goto switchD_ram_0c81_caseD_84;
  case 0x86:
    goto code_r0x0d09;
  case 0x88:
    pbVar14 = clearAndSeedScoreField;
    goto switchD_ram_0c81_caseD_8c;
  case 0x8a:
    goto switchD_ram_0c81_caseD_8a;
  case 0x8c:
    goto switchD_ram_0c81_caseD_8c;
  case 0x8e:
    bVar5 = DAT_ram_83d8;
  case 0x92:
switchD_ram_0c81_caseD_92:
    if (bVar5 != 0) {
      return bVar5;
    }
switchD_ram_0c81_caseD_94:
    bVar5 = DAT_ram_83d6;
switchD_ram_0fbd_caseD_33:
    bVar1 = bVar5 == 3;
code_r0x0d1b:
    if (bVar1) {
      DAT_ram_83d8 = DAT_ram_83d8 - 1;
      DAT_ram_83d7 = 0;
      DAT_ram_83b3 = 0;
      fillTilemapBlock22x32();
      DAT_ram_8019 = 3;
      puVar12 = &DAT_ram_801f;
      uVar6 = SUB21(param_1,0);
      cVar8 = '\x05';
      do {
        *puVar12 = 0;
        puVar12 = (undefined1 *)CONCAT11((char)((ushort)puVar12 >> 8),(char)puVar12 + '\x04');
        cVar8 = cVar8 + -1;
      } while (cVar8 != '\0');
      placeScoreRankMarkers();
      uVar7 = CONCAT11(0xd,uVar6);
      copyRunUpTileColumn(&UNK_ram_aaac,&UNK_ram_2ee5);
      cVar8 = '\x01';
      do {
        uVar6 = (undefined1)uVar7;
        writeScoreDigitStepUp(CONCAT11(0xaa,cVar8 * '\x02' + -0x33));
        uVar7 = CONCAT11(3,uVar6);
        cVar15 = cVar8;
        bVar5 = copyRunUpTileColumn((char)((ushort)unaff_AF_ >> 8),uVar7);
        unaff_AF_ = (ushort)bVar5 << 8;
        puVar13 = &DAT_ram_83ef;
        cVar9 = cVar8;
        do {
          cVar11 = (char)puVar13;
          uVar6 = (undefined1)((ushort)puVar13 >> 8);
          puVar13 = (undefined2 *)CONCAT11(uVar6,cVar11 + '\x02');
          cVar9 = cVar9 + -1;
        } while (cVar9 != '\0');
        writeScoreField(CONCAT11(0xa9,cVar8 * '\x02' + -0x13),
                        CONCAT11(*(undefined1 *)CONCAT11(uVar6,cVar11 + '\x03'),
                                 *(undefined1 *)puVar13));
        copyRunUpTileColumn(&UNK_ram_2fba);
        cVar8 = cVar15 + '\x01';
      } while (cVar8 != '\x06');
      DAT_ram_8039 = 0;
      bVar5 = copyRunUpTileColumn(CONCAT11(0xf,(char)uVar7),&UNK_ram_aafc,&UNK_ram_2f4d);
      return bVar5;
    }
switchD_ram_0c81_caseD_9c:
    cVar8 = DAT_ram_83e1;
code_r0x0d21:
    bVar1 = cVar8 == '\0';
switchD_ram_0c81_caseD_a0:
    bVar5 = DAT_ram_83d6;
    if (!bVar1) {
initInPlayBoardOnce:
      clearActivePlayerWorkRam(param_1);
      bVar4 = DAT_ram_83ba;
      goto switchD_ram_0c81_caseD_d0;
    }
  case 0xa6:
switchD_ram_0c81_caseD_a6:
    bVar1 = bVar5 == 4;
switchD_ram_0c81_caseD_a8:
    if (!bVar1) {
code_r0x0d2d:
      bVar1 = bVar5 == 2;
code_r0x0d2f:
      if (bVar1) {
        DAT_ram_83d8 = 0xff;
        fillTilemapBlock28x32(param_1);
        DAT_ram_829b = 0;
        DAT_ram_8021 = 0;
        DAT_ram_801b = 5;
        DAT_ram_802b = 3;
        copyRunUpTileColumn(&UNK_ram_aa8d,&UNK_ram_2f5c);
        if (DAT_ram_83e4 < 10) {
          writeScoreDigitStepUp(&UNK_ram_ab15);
          copyRunUpTileColumn(&UNK_ram_2fae);
          copyRunUpTileColumn(&UNK_ram_2f73);
          bVar5 = copyRunUpTileColumn(&UNK_ram_2f92);
          return bVar5;
        }
        return DAT_ram_83e4;
      }
switchD_ram_0c81_caseD_b0:
      bVar1 = bVar5 == 5;
switchD_ram_0c81_caseD_b2:
      if (!bVar1) {
        return bVar5;
      }
      pbVar14 = &DAT_ram_83d8;
switchD_ram_0c81_caseD_b6:
      *pbVar14 = 0x30;
switchD_ram_0c81_caseD_b8:
      pbVar14 = (byte *)CONCAT11((char)((ushort)pbVar14 >> 8),(char)pbVar14 + -1);
      bVar5 = 0;
switchD_ram_0c81_caseD_ba:
      *pbVar14 = bVar5;
      DAT_ram_8015 = bVar5;
switchD_ram_0c81_caseD_be:
      puVar10 = &UNK_ram_2f01;
code_r0x0d43:
      pbVar14 = &UNK_ram_aaca;
switchD_ram_0c81_caseD_c4:
      param_1 = (byte *)CONCAT11(0xd,(char)param_1);
switchD_ram_0c81_caseD_c6:
      copyRunUpTileColumn(param_1,pbVar14,puVar10);
      bVar5 = blitMode3FinalStrip();
      return bVar5;
    }
    goto code_r0x0c6d;
  case 0x90:
    if (bVar3) {
      return bVar5;
    }
    bVar5 = DAT_ram_83d7 * '\x04';
    goto switchD_ram_0c81_caseD_92;
  case 0x94:
    goto switchD_ram_0c81_caseD_94;
  case 0x96:
    bVar5 = DAT_ram_83d7 * '\x04';
    goto switchD_ram_0fbd_caseD_33;
  case 0x98:
    param_1 = param_1 + 1;
    goto code_r0x0d1b;
  case 0x9a:
    param_1 = param_1 + -1;
  case 0x9c:
    goto switchD_ram_0c81_caseD_9c;
  case 0x9e:
    cVar8 = DAT_ram_83d7 * '\x04';
    goto code_r0x0d21;
  case 0xa0:
    goto switchD_ram_0c81_caseD_a0;
  case 0xa2:
    param_1 = (byte *)CONCAT11(cVar15,cVar8 + -1);
    bVar5 = DAT_ram_83d6;
    goto switchD_ram_0c81_caseD_a6;
  case 0xa4:
    bVar5 = bVar5 + 0x7d;
    goto switchD_ram_0c81_caseD_a6;
  case 0xa8:
    goto switchD_ram_0c81_caseD_a8;
  case 0xaa:
    param_1 = (byte *)CONCAT11(cVar15,cVar8 + '\x01');
    goto code_r0x0d2d;
  case 0xac:
    *param_1 = bVar5;
    goto code_r0x0d2f;
  case 0xae:
    bVar5 = bVar5 + cVar15 + bVar3;
  case 0xb0:
    goto switchD_ram_0c81_caseD_b0;
  case 0xb2:
    goto switchD_ram_0c81_caseD_b2;
  case 0xb4:
    if (bVar3) {
      return bVar5;
    }
  case 0xb6:
    goto switchD_ram_0c81_caseD_b6;
  case 0xb8:
    goto switchD_ram_0c81_caseD_b8;
  case 0xba:
    goto switchD_ram_0c81_caseD_ba;
  case 0xbc:
  case 0xbe:
    goto switchD_ram_0c81_caseD_be;
  case 0xc0:
    goto code_r0x0d43;
  case 0xc2:
    if (bVar1) {
      bVar5 = seatStackAndEnterColdBoot();
      *pbVar14 = 0xfe;
      if (bVar5 == 0x50) {
        puVar12 = &DAT_ram_a924;
      }
      else {
        if (bVar5 != 0x30) {
          if (bVar5 == 0x10) {
            fillTwoByTwoTileBlock(&DAT_ram_ab64);
            fillTwoByTwoTileBlock(&DAT_ram_aaa4);
            fillTwoByTwoTileBlock(&DAT_ram_a9e4);
            fillTwoByTwoTileBlock(&DAT_ram_a924);
            fillTwoByTwoTileBlock(&DAT_ram_a864);
            DAT_ram_842f = 0;
            bVar5 = awardExtraLife();
            return bVar5;
          }
          return bVar5;
        }
        puVar12 = &DAT_ram_a864;
      }
      *puVar12 = 0xfc;
      puVar12[1] = 0xfd;
      puVar12[0x20] = 0xfe;
      puVar12[0x21] = 0xff;
      return bVar5;
    }
    param_1 = (byte *)CONCAT11(cVar15,cVar8 + -1);
    goto switchD_ram_0c81_caseD_c6;
  case 0xc4:
    goto switchD_ram_0c81_caseD_c4;
  case 0xc6:
    goto switchD_ram_0c81_caseD_c6;
  case 200:
    param_1 = (byte *)CONCAT11(cVar15,cVar8 + '\x01');
  case 0xca:
    goto initInPlayBoardOnce;
  case 0xcc:
    bVar4 = DAT_ram_83ba;
  case 0xd0:
switchD_ram_0c81_caseD_d0:
    bVar5 = 0;
    if (bVar4 != 0) {
      return bVar4;
    }
switchD_ram_0c81_caseD_d2:
    pbVar14 = (byte *)CONCAT11(bVar5,bVar5);
switchD_ram_0c81_caseD_d4:
    _DAT_ram_81b3 = pbVar14;
    _DAT_ram_8293 = pbVar14;
  case 0xda:
switchD_ram_0c81_caseD_da:
    switchD_ram:0fbd::caseD_1d = bVar5;
    DAT_ram_829a = bVar5;
switchD_ram_0c81_caseD_e0:
    bVar5 = bVar5 + 1;
    DAT_ram_83ba = bVar5;
switchD_ram_0c81_caseD_e4:
    loadActivePlayerLaneParams(bVar5);
    activateFrogObject();
switchD_ram_0c81_caseD_ea:
    fillTilemapBlock28x32();
code_r0x0d6f:
    clearObjectBlocksAndMirrorToObjRam();
switchD_ram_0c81_caseD_f0:
    bVar5 = 4;
switchD_ram_0c81_caseD_f2:
    DAT_ram_801b = bVar5;
code_r0x0d77:
    bVar5 = 6;
    DAT_ram_8029 = 6;
switchD_ram_0c81_caseD_fa:
    pbVar14 = &UNK_ram_aa28;
code_r0x0d7f:
    puVar10 = &UNK_ram_2f77;
code_r0x0d82:
    copyRunUpTileColumn(bVar5,pbVar14,puVar10);
    copyRunUpTileColumn(&UNK_ram_aaad,(char)puVar10 + '\x01');
    blitPlayerSelectPrompt();
    copyRunUpTileColumn(&UNK_ram_ab74,&UNK_ram_2f88);
    copyRunUpTileColumn(&UNK_ram_2fa8);
    puVar10 = &UNK_ram_2fae;
    copyRunUpTileColumn(&UNK_ram_2fae);
    copyRunUpTileColumn(puVar10 + 1);
    writeScoreField(&UNK_ram_a994,DAT_ram_2e08);
    bVar5 = copyRunUpTileColumn(&UNK_ram_2fba);
    return bVar5;
  case 0xce:
    bVar4 = DAT_ram_83d7 * '\x04';
    goto switchD_ram_0c81_caseD_d0;
  case 0xd2:
    goto switchD_ram_0c81_caseD_d2;
  case 0xd4:
    goto switchD_ram_0c81_caseD_d4;
  case 0xd6:
    _DAT_ram_81b3 = pbVar14;
    goto switchD_ram_0c81_caseD_da;
  case 0xd8:
    bVar5 = bVar5 + cVar8;
    goto switchD_ram_0c81_caseD_da;
  case 0xdc:
    DAT_ram_829a = bVar5;
    goto switchD_ram_0c81_caseD_e0;
  case 0xde:
    bVar5 = bVar5 - bVar3;
  case 0xe0:
    goto switchD_ram_0c81_caseD_e0;
  case 0xe2:
    bVar5 = DAT_ram_83d7 * '\x04';
  case 0xe4:
    goto switchD_ram_0c81_caseD_e4;
  case 0xe6:
    pbRam04cd = pbVar14;
    goto switchD_ram_0c81_caseD_ea;
  case 0xe8:
  case 0xea:
    goto switchD_ram_0c81_caseD_ea;
  case 0xec:
    goto code_r0x0d6f;
  case 0xee:
    goto switchD_ram_0c81_caseD_f2;
  case 0xf0:
    goto switchD_ram_0c81_caseD_f0;
  case 0xf2:
    goto switchD_ram_0c81_caseD_f2;
  case 0xf4:
    goto code_r0x0d77;
  case 0xf6:
    param_1 = (byte *)CONCAT11(0x32,cVar8);
  case 0xf8:
    bVar5 = bVar5 + (char)((ushort)param_1 >> 8);
  case 0xfa:
    goto switchD_ram_0c81_caseD_fa;
  case 0xfc:
    goto code_r0x0d7f;
  case 0xfe:
    *pbVar14 = bVar5;
    bVar5 = ~bVar5;
    goto code_r0x0d82;
  }
  DAT_ram_8029 = bVar5;
  DAT_ram_802f = bVar5;
switchD_ram_0c81_caseD_16:
  bVar5 = 3;
switchD_ram_0c81_caseD_18:
  DAT_ram_801b = bVar5;
  DAT_ram_8021 = bVar5;
switchD_ram_0c81_caseD_1e:
  DAT_ram_8027 = bVar5;
  DAT_ram_802d = bVar5;
switchD_ram_0c81_caseD_24:
  bVar5 = 0x10;
switchD_ram_0c81_caseD_26:
  pbVar14 = &UNK_ram_ab6d;
code_r0x0cab:
  writePackedBcdByte(bVar5,pbVar14);
switchD_ram_0c81_caseD_2c:
  puVar10 = &UNK_ram_2fba;
code_r0x0cb3:
  copyRunUpTileColumn(puVar10);
switchD_ram_0c81_caseD_32:
  bVar5 = copyRunUpTileColumn(&UNK_ram_2ed1);
switchD_ram_0c81_caseD_38:
  DAT_ram_83d8 = 0x80;
  return bVar5;
}

