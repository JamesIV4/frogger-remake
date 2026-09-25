
/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Instruction at (ram,0x14e4) overlaps instruction at (ram,0x14e3)
    */
/* WARNING: This function may have set the stack pointer */
/* WARNING: Removing unreachable block (ram,0x0ca1) */
/* WARNING: Removing unreachable block (ram,0x1415) */
/* WARNING: Removing unreachable block (ram,0x14ae) */
/* WARNING: Removing unreachable block (ram,0x0393) */
/* WARNING: Removing unreachable block (ram,0x1417) */
/* WARNING: Removing unreachable block (ram,0x1418) */
/* WARNING: Removing unreachable block (ram,0x141c) */
/* WARNING: Removing unreachable block (ram,0x141f) */
/* WARNING: Removing unreachable block (ram,0x1420) */
/* WARNING: Removing unreachable block (ram,0xccd3) */
/* WARNING: Removing unreachable block (ram,0x06cd) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

code dispatchFrogAnimationArm(code *param_1,byte *param_2,char *param_3,undefined2 param_4)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  short sVar4;
  undefined2 in_AF;
  code cVar5;
  undefined1 uVar6;
  byte bVar7;
  char cVar12;
  short sVar8;
  char *pcVar9;
  code *pcVar10;
  char cVar13;
  code cVar14;
  ushort uVar11;
  byte *pbVar15;
  undefined1 *puVar16;
  undefined2 *puVar17;
  code *pcVar18;
  code cVar19;
  byte *pbVar20;
  undefined1 uVar25;
  undefined1 *puVar21;
  undefined *puVar22;
  undefined *puVar23;
  byte bVar26;
  code *pcVar24;
  code in_I;
  short unaff_AF_;
  undefined2 unaff_BC_;
  char unaff_E_;
  short in_stack_00000000;
  
  do {
    uVar11 = _caseD_36 & 0xff;
    puVar23 = (undefined *)(uVar11 * 2);
    cVar5 = SUB21((ushort)in_AF >> 8,0);
    bVar2 = &UNK_ram_f041 < puVar23;
    bVar7 = (&switchD_ram:0fbd::switchdataD_ram_0fbe)[(short)puVar23];
    pcVar18 = (code *)CONCAT11(0xf,bVar7);
    pcVar24 = *(code **)(&switchD_ram:0fbd::switchdataD_ram_0fbe + (short)puVar23);
    cVar12 = (char)param_1;
    cVar13 = (char)((ushort)param_1 >> 8);
    cVar14 = DAT_ram_a433;
    switch(uVar11) {
    case 0:
      pcVar18 = (code *)CONCAT11(DAT_ram_8271,DAT_ram_8272);
      param_1 = (code *)&switchD_ram:0fbd::caseD_11;
      param_3 = &switchD_ram:14c6::caseD_1a;
      param_2 = &switchD_ram:14c6::caseD_1a;
      switchD_ram:0fbd::caseD_5a = DAT_ram_8270;
      _caseD_36 = CONCAT21(&switchD_ram:0fbd::caseD_11,switchD_ram:0fbd::caseD_36);
      pcVar24 = DAT_ram_13ed;
    case 0x9c:
    case 0xac:
    case 0xbc:
    case 0xcc:
    case 0xdc:
    case 0xec:
      pcVar10 = pcVar18;
      computeVramColumnIndex();
      if (switchD_ram:0fbd::caseD_1d == (code)0x0) {
        param_3[1] = ~(byte)pcVar10 + 1;
        param_3 = param_3 + 1;
        *param_2 = *param_2 + 1;
      }
      DAT_ram_8003 = SUB21((ushort)pcVar18 >> 8,0);
switchD_ram_0fbd_caseD_3e:
      while( true ) {
        *pcVar24 = *param_1;
        pcVar24[1] = param_1[1];
        pcVar24 = pcVar24 + 0x20;
        cVar13 = (char)((ushort)pcVar18 >> 8) + -1;
        pcVar18 = (code *)CONCAT11(cVar13,(char)pcVar18);
        cVar5 = switchD_ram:0fbd::caseD_5a;
        if (cVar13 == '\0') break;
        param_1 = param_1 + 2;
      }
switchD_ram_14c6_caseD_2e:
      param_1 = (code *)(ushort)(byte)cVar5;
      cVar13 = (char)pcVar18 + -1;
      if (cVar13 != '\0') {
        cVar5 = (code)renderFrogAnimTileColumns
                                (CONCAT11(DAT_ram_8003,cVar13),pcVar24 + (short)param_1,
                                 switchD_ram:0fbd::caseD_40);
        return cVar5;
      }
      break;
    case 1:
      blitFrogAnimColumnOnTrigger();
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
      _caseD_36 = CONCAT21(&UNK_ram_1423,switchD_ram:0fbd::caseD_36);
      cVar5 = (code)renderFrogAnimTileColumns
                              (CONCAT11(DAT_ram_8274,DAT_ram_8275),switchD_ram:0fbd::caseD_53,
                               &DAT_ram_8109,&DAT_ram_8109);
      return cVar5;
    case 2:
      switchD_ram:0fbd::caseD_5a = (code)switchD_ram:0fbd::caseD_5f;
      _caseD_36 = CONCAT21(&UNK_ram_143b,switchD_ram:0fbd::caseD_36);
      cVar5 = (code)renderFrogAnimTileColumns
                              (CONCAT11(DAT_ram_8277,DAT_ram_8278),uRam13f1,
                               &switchD_ram:14c6::caseD_3c,&switchD_ram:14c6::caseD_3c);
      return cVar5;
    case 3:
      switchD_ram:0fbd::caseD_5a = (code)switchD_ram:0fbd::caseD_6f;
      _caseD_36 = CONCAT21(&UNK_ram_1453,switchD_ram:0fbd::caseD_36);
      cVar5 = (code)renderFrogAnimTileColumns
                              (CONCAT11(DAT_ram_827a,DAT_ram_827b),uRam13f3,&UNK_ram_811b,
                               &UNK_ram_811b);
      return cVar5;
    case 4:
      switchD_ram:0fbd::caseD_5a = (code)switchD_ram:0fbd::caseD_7f;
      _caseD_36 = CONCAT21(&UNK_ram_145f,switchD_ram:0fbd::caseD_36);
      cVar5 = (code)renderFrogAnimTileColumns
                              (CONCAT11(DAT_ram_827d,DAT_ram_827e),switchD_ram:0fbd::caseD_83,
                               &switchD_ram:14c6::caseD_5e,&switchD_ram:14c6::caseD_5e);
      return cVar5;
    case 5:
      break;
    case 6:
      pcVar18 = (code *)CONCAT11(DAT_ram_8283,DAT_ram_8284);
      switchD_ram:0fbd::caseD_5a = DAT_ram_8282;
      _caseD_36 = CONCAT21(0x149f,switchD_ram:0fbd::caseD_36);
      pcVar24 = DAT_ram_13f9;
      cVar5 = DAT_ram_8282;
      goto code_r0x1115;
    case 7:
      goto code_r0x1118;
    case 8:
      switchD_ram:0fbd::caseD_5a = (code)DAT_ram_8288;
      _caseD_36 = CONCAT21(0x14ab,switchD_ram:0fbd::caseD_36);
      cVar5 = (code)renderFrogAnimTileColumns
                              (CONCAT11(DAT_ram_8289,DAT_ram_828a),DAT_ram_13fd,
                               &switchD_ram:14c6::caseD_a2,&switchD_ram:14c6::caseD_a2);
      return cVar5;
    case 9:
      switchD_ram:0fbd::caseD_5a = (code)DAT_ram_828b;
      _caseD_36 = CONCAT21(0x14af,switchD_ram:0fbd::caseD_36);
      cVar5 = (code)renderFrogAnimTileColumns
                              (CONCAT11(DAT_ram_828c,DAT_ram_828d),DAT_ram_13ff,
                               &switchD_ram:0fbd::caseD_d5,&switchD_ram:0fbd::caseD_d5);
      return cVar5;
    case 10:
renderFrogAnimArm10:
      switchD_ram:0fbd::caseD_5a = (code)DAT_ram_828e;
      _caseD_36 = CONCAT21(0x14b3,switchD_ram:0fbd::caseD_36);
      cVar5 = (code)renderFrogAnimTileColumns
                              (CONCAT11(DAT_ram_828f,DAT_ram_8290),_UNK_ram_1401,
                               &switchD_ram:14c6::caseD_c4,&switchD_ram:14c6::caseD_c4);
      return cVar5;
    case 0xb:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xc:
    case 0x9e:
    case 0xae:
    case 0xbe:
    case 0xce:
    case 0xde:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xd:
    case 0x9f:
    case 0xaf:
    case 0xbf:
    case 0xcf:
    case 0xdf:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xe:
    case 0xa0:
    case 0xb0:
    case 0xc0:
    case 0xd0:
    case 0xe0:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xf:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x10:
    case 0xa2:
    case 0xb2:
    case 0xc2:
    case 0xd2:
code_r0x1118:
      switchD_ram:0fbd::caseD_5a = (code)DAT_ram_8285;
      _caseD_36 = CONCAT21(0x14a7,switchD_ram:0fbd::caseD_36);
      cVar5 = (code)renderFrogAnimTileColumns
                              (CONCAT11(DAT_ram_8286,DAT_ram_8287),DAT_ram_13fb,
                               &switchD_ram:0fbd::caseD_b5,&switchD_ram:0fbd::caseD_b5);
      return cVar5;
    case 0x11:
      *pcVar24 = (code)0x37;
      *pcVar24 = (code)((char)*pcVar24 + 1);
      *pcVar24 = (code)((char)*pcVar24 - 1);
      *pcVar24 = (code)0x37;
      pcVar24 = pcVar24 + (short)register0x44;
      param_1 = (code *)&UNK_ram_3f3f;
      *pcVar24 = (code)((char)*pcVar24 + 1);
      *pcVar24 = (code)((char)*pcVar24 - 1);
      *pcVar24 = (code)0x37;
      *pcVar24 = (code)((char)*pcVar24 + 1);
      *pcVar24 = (code)((char)*pcVar24 - 1);
      *pcVar24 = (code)0x37;
      *pcVar24 = (code)((char)*pcVar24 + 1);
      *pcVar24 = (code)((char)*pcVar24 - 1);
      *pcVar24 = (code)0x37;
      *pcVar24 = (code)((char)*pcVar24 + 1);
      *pcVar24 = (code)((char)*pcVar24 - 1);
      *pcVar24 = (code)0x37;
      *pcVar24 = (code)((char)*pcVar24 + 1);
      *pcVar24 = (code)((char)*pcVar24 - 1);
      *pcVar24 = (code)0x37;
      pcVar18 = (code *)CONCAT11(0x3f,bVar7);
    case 0xa3:
      cVar5 = (code)((byte)((ushort)pcVar18 >> 8) ^ (byte)pcVar18 ^ (byte)((ushort)param_1 >> 8) ^
                    (byte)param_1);
switchD_ram_0fbd_caseD_b3:
      cVar5 = (code)((byte)cVar5 & (byte)((ushort)pcVar18 >> 8) & (byte)pcVar18 &
                     (byte)((ushort)param_1 >> 8) & (byte)param_1);
      bVar2 = false;
switchD_ram_0fbd_caseD_c3:
      cVar14 = cVar5;
      if (!bVar2) goto dispatch_14dd;
switchD_ram_0fbd_caseD_e3:
      DAT_ram_a433 = cVar14;
      param_1 = (code *)CONCAT11((char)param_1,(char)param_1);
code_r0x14b7:
      cVar5 = (code)((char)DAT_ram_80ff * '\x02');
      puVar23 = (undefined *)(ushort)(byte)cVar5;
      bVar7 = (&switchD_ram:14c6::switchdataD_ram_14c7)[(short)puVar23];
      pcVar18 = (code *)CONCAT11(0x14,bVar7);
      bVar26 = (&BYTE_ram_14c8)[(short)puVar23];
      pcVar24 = *(code **)(&switchD_ram:14c6::switchdataD_ram_14c7 + (short)puVar23);
      cVar14 = SUB21(param_1,0);
      cVar19 = SUB21((ushort)param_1 >> 8,0);
      switch(cVar5) {
      case (code)0x0:
dispatch_14dd:
        pcVar24 = (code *)&DAT_ram_819b;
        param_1 = (code *)&switchD_ram:14c6::caseD_1a;
        param_3 = &switchD_ram:14c6::caseD_1e;
        param_2 = &switchD_ram:14c6::caseD_22;
        break;
      case (code)0x2:
        pcVar24 = (code *)&switchD_ram:14c6::caseD_28;
        param_1 = (code *)&DAT_ram_8109;
        param_3 = &UNK_ram_8010;
        param_2 = &UNK_ram_81a7;
      case (code)0x36:
switchD_ram_14c6_caseD_36:
        bVar7 = *param_2;
        if ((bVar7 == 0) && (bVar7 = (byte)*pcVar24 & 0xf, ((byte)*pcVar24 & 0x10) == 0)) {
LAB_ram_1651:
          cVar5 = *param_1;
          do {
            param_1 = param_1 + 1;
            *param_1 = (code)((char)*param_1 - bVar7);
            cVar5 = (code)((char)cVar5 - 1);
          } while (cVar5 != (code)0x0);
          cVar13 = *param_3;
          *param_3 = cVar13 - bVar7;
          param_3[2] = cVar13 - bVar7;
          if ((byte)DAT_ram_8047 < 0x73) {
            if (((byte)DAT_ram_8047 & 0xf) < 3) {
              if ((DAT_ram_80ff == (code)((byte)(((byte)DAT_ram_8047 & 0xf0) - 0x30) >> 4)) &&
                 ((DAT_ram_8044 = (code)((char)DAT_ram_8044 - bVar7), (byte)DAT_ram_8044 < 8 ||
                  (0xe6 < (byte)DAT_ram_8044)))) {
                DAT_ram_8004 = (code)0x1;
              }
            }
            else if ((0xb < ((byte)DAT_ram_8047 & 0xf)) &&
                    (DAT_ram_80ff == (code)((byte)(((byte)DAT_ram_8047 & 0xf0) - 0x20) >> 4))) {
              DAT_ram_8044 = (code)((char)DAT_ram_8044 - bVar7);
            }
          }
          *param_2 = 0;
        }
        else {
          if (bVar7 == 1) {
            bVar7 = 1;
            goto LAB_ram_1651;
          }
          *param_2 = bVar7 - 1;
        }
        cVar5 = (code)((char)DAT_ram_80ff + 1);
        DAT_ram_80ff = cVar5;
        if (10 < (byte)cVar5) {
          DAT_ram_80ff = (code)0x0;
          return cVar5;
        }
        goto code_r0x14b7;
      case (code)0x4:
        pcVar24 = (code *)&UNK_ram_819d;
        param_1 = (code *)&switchD_ram:14c6::caseD_3c;
        param_3 = &switchD_ram:14c6::caseD_40;
        param_2 = &switchD_ram:14c6::caseD_44;
        break;
      case (code)0x6:
        pcVar24 = (code *)&switchD_ram:14c6::caseD_4a;
        param_1 = (code *)&UNK_ram_811b;
        param_3 = &UNK_ram_8018;
        param_2 = &UNK_ram_81a9;
        break;
      case (code)0x8:
        pcVar24 = (code *)&UNK_ram_819f;
        param_1 = (code *)&switchD_ram:14c6::caseD_5e;
        param_3 = &switchD_ram:14c6::caseD_62;
        param_2 = &switchD_ram:14c6::caseD_66;
        goto switchD_ram_14c6_caseD_36;
      case (code)0xa:
        goto code_r0x15de;
      case (code)0xc:
        pcVar24 = (code *)&UNK_ram_81a1;
        param_1 = (code *)&switchD_ram:14c6::caseD_80;
        param_3 = &switchD_ram:14c6::caseD_84;
        param_2 = &switchD_ram:14c6::caseD_88;
        goto switchD_ram_14c6_caseD_36;
      case (code)0xe:
        pcVar24 = (code *)&switchD_ram:14c6::caseD_8e;
        param_1 = (code *)&switchD_ram:0fbd::caseD_b5;
        param_3 = &UNK_ram_8028;
        param_2 = &UNK_ram_81ad;
        break;
      case (code)0x10:
        pcVar24 = (code *)&UNK_ram_81a3;
        param_1 = (code *)&switchD_ram:14c6::caseD_a2;
        param_3 = &switchD_ram:14c6::caseD_a6;
        param_2 = &switchD_ram:14c6::caseD_aa;
        goto switchD_ram_14c6_caseD_36;
      case (code)0x12:
        pcVar24 = (code *)&switchD_ram:14c6::caseD_b0;
        param_1 = (code *)&switchD_ram:0fbd::caseD_d5;
        param_3 = &UNK_ram_8030;
        param_2 = &UNK_ram_81af;
        break;
      case (code)0x14:
        pcVar24 = (code *)&UNK_ram_81a5;
        param_1 = (code *)&switchD_ram:14c6::caseD_c4;
        param_3 = &switchD_ram:14c6::caseD_c8;
        param_2 = &switchD_ram:14c6::caseD_cc;
        goto switchD_ram_14c6_caseD_36;
      case (code)0x16:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      default:
        goto renderFrogAnimArm10;
      case (code)0x1a:
      case (code)0xee:
        goto code_r0x8100;
      case (code)0x1c:
      case (code)0x3e:
      case (code)0x60:
      case (code)0x82:
      case (code)0xa4:
      case (code)0xc6:
        goto switchD_ram_14c6_caseD_1c;
      case (code)0x1e:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x20:
      case (code)0x42:
      case (code)0x64:
      case (code)0x86:
      case (code)0xa8:
      case (code)0xca:
        goto switchD_ram_14c6_caseD_20;
      case (code)0x22:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x24:
      case (code)0x46:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x26:
      case (code)0x48:
        goto switchD_ram_14c6_caseD_26;
      case (code)0x28:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x2a:
        if ((cVar5 != (code)0x0) || ((DAT_ram_2e08 != param_1 && (param_1 <= DAT_ram_2e08))))
        goto LAB_ram_0936;
        cVar5 = (code)0x1;
        DAT_ram_83cf = '\0';
        goto switchD_ram_0fbd_caseD_56;
      case (code)0x2c:
      case (code)0x4e:
      case (code)0x70:
      case (code)0x92:
      case (code)0xb4:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x2e:
        goto switchD_ram_14c6_caseD_2e;
      case (code)0x30:
      case (code)0x52:
      case (code)0x74:
      case (code)0x96:
      case (code)0xb8:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x32:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x34:
      case (code)0x56:
      case (code)0x78:
      case (code)0x9a:
      case (code)0xbc:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x38:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x3c:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x40:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x44:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x4a:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x4c:
        return cVar5;
      case (code)0x50:
        cVar13 = DAT_ram_81b3;
        _DAT_ram_81b3 = (code *)CONCAT11(0x15,DAT_ram_81b3 + '\x01');
        if ((code)(cVar13 + -9) == (code)0x0) {
          _DAT_ram_81b3 = (code *)0x1500;
          return (code)0x0;
        }
        pcVar18 = (code *)&DAT_ram_819b;
        sVar8 = 0xb;
        do {
          *pcVar18 = *pcVar24;
          pcVar18 = pcVar18 + 1;
          pcVar24 = pcVar24 + 1;
          sVar8 = sVar8 + -1;
        } while (sVar8 != 0);
        return (code)(cVar13 + -9);
      case (code)0x54:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x58:
      case (code)0x7a:
      case (code)0x9c:
      case (code)0xbe:
        break;
      case (code)0x5a:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x5e:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x62:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x66:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x68:
      case (code)0x8a:
      case (code)0x90:
      case (code)0xac:
      case (code)0xb6:
      case (code)0xce:
        goto DAT_ram_4000;
      case (code)0x6a:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x6c:
        goto code_r0x15de;
      case (code)0x6e:
        cVar19 = *pcVar24;
        *pcVar24 = cVar5;
        if (cVar5 == (code)0x0) {
          DAT_ram_b818 = 1;
          DAT_ram_837e = '\x04';
        }
                    /* WARNING: Could not recover jumptable at 0x2d22. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        cVar5 = (code)(*(dispatch_2d23 + CONCAT11(cVar19,cVar14)))();
        return cVar5;
      case (code)0x72:
        *pcVar18 = cVar5;
        DAT_ram_8111 = DAT_ram_8111 + 2;
        DAT_ram_8119 = cVar5;
        if (DAT_ram_8111 < 0xa0) {
          blitScrollBand();
        }
        DAT_ram_826e = (code)((char)DAT_ram_826e + '\x01');
        if (DAT_ram_826e == (code)0x10) {
          switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
          blitScrollTileGrid(CONCAT11(DAT_ram_8274,DAT_ram_811a),&UNK_ram_1423);
          puVar23 = &UNK_ram_145f;
          goto LAB_ram_20bf;
        }
        if (DAT_ram_826e == (code)0x20) {
          switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
          blitScrollTileGrid(CONCAT11(DAT_ram_8274,DAT_ram_811a),&UNK_ram_142b);
          puVar23 = &UNK_ram_1473;
          goto LAB_ram_20bf;
        }
        if (DAT_ram_826e != (code)0x30) {
          return DAT_ram_826e;
        }
        puVar16 = DAT_ram_811a;
        pcVar18 = (code *)CONCAT11(DAT_ram_8274,DAT_ram_811a);
        param_1 = (code *)&UNK_ram_1433;
        switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
        cVar13 = '\0';
        DAT_ram_826e = (code)0x0;
        goto code_r0x20a9;
      case (code)0x76:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x7c:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x80:
        goto code_r0x8136;
      case (code)0x84:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x88:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x8c:
      case (code)0xae:
        goto switchD_ram_14c6_caseD_8c;
      case (code)0x8e:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x94:
        DAT_ram_814e = (code)((char)cVar5 + 2);
        uVar11 = (ushort)DAT_ram_8145;
        DAT_ram_8145 = DAT_ram_8145 + 0x20;
        param_1[uVar11] = pcVar24[(short)pcVar18];
        (param_1 + uVar11)[1] = (pcVar24 + (short)pcVar18)[1];
        if ((byte)DAT_ram_814e < 0x10) {
          return DAT_ram_814e;
        }
        DAT_ram_814f = 0;
        DAT_ram_814e = (code)0x0;
        DAT_ram_8145 = 0;
        DAT_ram_8146 = 0;
        DAT_ram_8147 = 0;
        return (code)0x0;
      case (code)0x98:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0x9e:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xa2:
        goto code_r0x8148;
      case (code)0xa6:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xaa:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xb0:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xb2:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xba:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xc0:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xc4:
        goto code_r0x815a;
      case (code)0xc8:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xcc:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xd0:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xd2:
        *param_1 = *pcVar24;
        pbVar20 = (byte *)CONCAT11(bVar26,bVar7 + 1);
        pbVar15 = (byte *)CONCAT11(cVar19,(char)cVar14 + '\x01');
        cVar13 = '\x1c';
        do {
          bVar7 = *pbVar20 >> 1;
          bVar26 = (byte)(bVar7 | *pbVar20 << 7) >> 1;
          bVar7 = (byte)(bVar26 | bVar7 << 7) >> 1;
          *pbVar15 = (byte)(bVar7 | bVar26 << 7) >> 1 | bVar7 << 7;
          uVar25 = (undefined1)((ushort)pbVar20 >> 8);
          uVar6 = (undefined1)((ushort)pbVar15 >> 8);
          *(undefined1 *)CONCAT11(uVar6,(char)pbVar15 + '\x01') =
               *(undefined1 *)CONCAT11(uVar25,(char)pbVar20 + '\x01');
          pbVar20 = (byte *)CONCAT11(uVar25,(char)pbVar20 + '\x02');
          pbVar15 = (byte *)CONCAT11(uVar6,(char)pbVar15 + '\x02');
          cVar13 = cVar13 + -1;
        } while (cVar13 != '\0');
        cVar13 = '\b';
        if (DAT_ram_842f != '\0') {
          cVar13 = '\x06';
          pbVar15 = (byte *)CONCAT11(uVar6,0x48);
          pbVar20 = (byte *)CONCAT11(uVar25,0x48);
        }
        do {
          bVar7 = *pbVar20 >> 1;
          bVar26 = (byte)(bVar7 | *pbVar20 << 7) >> 1;
          bVar7 = (byte)(bVar26 | bVar7 << 7) >> 1;
          *pbVar15 = (byte)(bVar7 | bVar26 << 7) >> 1 | bVar7 << 7;
          pbVar20 = (byte *)CONCAT11((char)((ushort)pbVar20 >> 8),(char)pbVar20 + '\x01');
          pbVar15 = (byte *)CONCAT11((char)((ushort)pbVar15 >> 8),(char)pbVar15 + '\x01');
          cVar12 = '\x03';
          do {
            *pbVar15 = *pbVar20;
            pbVar20 = (byte *)CONCAT11((char)((ushort)pbVar20 >> 8),(char)pbVar20 + '\x01');
            pbVar15 = (byte *)CONCAT11((char)((ushort)pbVar15 >> 8),(char)pbVar15 + '\x01');
            cVar12 = cVar12 + -1;
          } while (cVar12 != '\0');
          cVar13 = cVar13 + -1;
        } while (cVar13 != '\0');
        if ((DAT_ram_837f != '\0') && (DAT_ram_837f = DAT_ram_837f + -1, DAT_ram_837f == '\0')) {
          DAT_ram_b81c = 0;
        }
        if ((DAT_ram_837e != '\0') && (DAT_ram_837e = DAT_ram_837e + -1, DAT_ram_837e == '\0')) {
          DAT_ram_b818 = 0;
        }
        if (((((DAT_ram_e004 & 8) != 0) && (DAT_ram_83fe != 0)) && (DAT_ram_83fd != '\0')) &&
           (DAT_ram_83fd != '\x01')) {
          DAT_ram_b043 = DAT_ram_8043 + '\x02';
          DAT_ram_b047 = (char)DAT_ram_8047 + 2;
        }
        if (DAT_ram_83fe == 0) {
          cVar5 = DAT_ram_83d6;
          if (1 < (byte)DAT_ram_83d6) {
            if (((DAT_ram_83d8 != (code)0x0) &&
                (DAT_ram_83d8 = (code)((char)DAT_ram_83d8 - 1), DAT_ram_83d8 == (code)0x0)) &&
               (DAT_ram_83d7 == '\0')) {
              DAT_ram_83d6 = (code)((char)DAT_ram_83d6 - 1);
            }
            goto LAB_ram_0245;
          }
          goto switchD_ram_0fbd_caseD_e1;
        }
        dequeueSoundCommand();
        if (DAT_ram_83ea == '\0') goto LAB_ram_0245;
        if ((char)((ushort)DAT_ram_83d2 >> 8) != '\0' || (char)DAT_ram_83d2 != '\0') {
          DAT_ram_83d2 = DAT_ram_83d2 + -1;
          moveLaneObjectsAndCarryFrog();
          goto dispatch_011c;
        }
        if (((char)((ushort)DAT_ram_8382 >> 8) != '\0' || (char)DAT_ram_8382 != '\0') &&
           (DAT_ram_8382 = DAT_ram_8382 + -1, DAT_ram_8382 == 0)) {
          enqueueSoundCommand(0xf);
          enqueueSoundCommand(0xb0);
          DAT_ram_8371 = 0;
        }
        cVar5 = DAT_ram_825d;
        if (DAT_ram_83fd == '\x01') {
          if (DAT_ram_825c == '\x05') {
            puVar21 = DAT_ram_825e;
            puVar16 = DAT_ram_825f;
            sVar8 = 4;
            DAT_ram_825e = (code)0x0;
            do {
              *puVar16 = *puVar21;
              puVar16 = puVar16 + 1;
              puVar21 = puVar21 + 1;
              sVar8 = sVar8 + -1;
            } while (sVar8 != 0);
            DAT_ram_825c = '\0';
            armBoardCompleteReveal();
            goto LAB_ram_0245;
          }
        }
        else {
switchD_ram_14c6_caseD_f4:
          if (cVar5 == (code)0x5) {
            puVar21 = DAT_ram_8263;
            puVar16 = DAT_ram_8264;
            sVar8 = 4;
            DAT_ram_8263 = (code)0x0;
            do {
              *puVar16 = *puVar21;
              puVar16 = puVar16 + 1;
              puVar21 = puVar21 + 1;
              sVar8 = sVar8 + -1;
            } while (sVar8 != 0);
            DAT_ram_825d = (code)0x0;
            armBoardCompleteReveal();
            goto LAB_ram_0245;
          }
        }
        if (DAT_ram_8298 == '\0') {
          if (DAT_ram_8297 != '\0') {
            DAT_ram_8297 = DAT_ram_8297 + -1;
            goto LAB_ram_01e2;
          }
          if ((char)((ushort)DAT_ram_829d >> 8) != '\0' || (char)DAT_ram_829d != '\0')
          goto LAB_ram_01e2;
          driveScoreDisplayCountdown();
          orchestrateCollisionsAndFrogInput();
          if (DAT_ram_83b5 != '\0') goto LAB_ram_01e2;
          DAT_ram_83b5 = '\x01';
          DAT_ram_8384 = -1;
          cVar13 = DAT_ram_8380;
          goto code_r0x01cc;
        }
        DAT_ram_8298 = DAT_ram_8298 + -1;
        goto LAB_ram_01e2;
      case (code)0xd4:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xd6:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xd8:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xda:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xdc:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xde:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xe0:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xe2:
        goto switchD_ram_14c6_caseD_e2;
      case (code)0xe4:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xe6:
        goto switchD_ram_14c6_caseD_e6;
      case (code)0xe8:
        if (puVar23 < &UNK_ram_eb39) goto LAB_ram_1284;
        goto LAB_ram_12a1;
      case (code)0xea:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xec:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xf0:
        goto switchD_ram_14c6_caseD_f0;
      case (code)0xf2:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xf4:
        goto switchD_ram_14c6_caseD_f4;
      case (code)0xf6:
      case (code)0xfe:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xf8:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xfa:
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      case (code)0xfc:
        goto switchD_ram_14c6_caseD_fc;
      }
      pcVar18 = (code *)(ushort)*param_2;
      if (*param_2 == 0) {
        cVar5 = *pcVar24;
        pcVar18 = (code *)(CONCAT11(cVar5,cVar5) & 0xff0f);
        cVar13 = (char)pcVar18;
        if (((byte)cVar5 & 0x10) != 0) goto switchD_ram_14c6_caseD_e2;
      }
      else {
switchD_ram_14c6_caseD_e2:
        if ((char)pcVar18 != '\x01') {
          *param_2 = (char)pcVar18 - 1;
          cVar5 = (code)switchD_ram:14c6::caseD_6c();
          return cVar5;
        }
        cVar13 = '\x01';
      }
      cVar5 = *param_1;
      do {
        param_1 = param_1 + 1;
        *param_1 = (code)((char)*param_1 + cVar13);
        cVar5 = (code)((char)cVar5 - 1);
      } while (cVar5 != (code)0x0);
      cVar12 = *param_3;
      *param_3 = cVar12 + cVar13;
      param_3[2] = cVar12 + cVar13;
      if ((0x2f < (byte)DAT_ram_8047) && ((byte)DAT_ram_8047 < 0x73)) {
        if (((byte)DAT_ram_8047 & 0xf) < 3) {
          if (((DAT_ram_80ff == (code)((byte)(((byte)DAT_ram_8047 & 0xf0) - 0x30) >> 4)) &&
              (0x2f < (byte)DAT_ram_8047)) &&
             ((DAT_ram_8044 = (code)((char)DAT_ram_8044 + cVar13), (byte)DAT_ram_8044 < 8 ||
              (0xe6 < (byte)DAT_ram_8044)))) {
            DAT_ram_8004 = (code)0x1;
          }
        }
        else if ((0xb < ((byte)DAT_ram_8047 & 0xf)) &&
                (DAT_ram_80ff == (code)((byte)(((byte)DAT_ram_8047 & 0xf0) - 0x20) >> 4))) {
          DAT_ram_8044 = (code)((char)DAT_ram_8044 + cVar13);
        }
      }
switchD_ram_14c6_caseD_fc:
      *param_2 = 0;
code_r0x15de:
      cVar5 = (code)((char)DAT_ram_80ff + 1);
      DAT_ram_80ff = cVar5;
      if (10 < (byte)cVar5) {
        DAT_ram_80ff = (code)0x0;
        return cVar5;
      }
      goto code_r0x14b7;
    case 0x12:
    case 0x93:
    case 0xa4:
    case 0xb4:
    case 0xc4:
    case 0xd4:
    case 0xe4:
      goto switchD_ram_14c6_caseD_1c;
    case 0x13:
    case 0x15:
code_r0x8100:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x14:
    case 0x95:
    case 0xa6:
    case 0xb6:
    case 0xc6:
    case 0xd6:
    case 0xe6:
switchD_ram_14c6_caseD_20:
      _UNK_ram_32af = (short)pcVar24 * 2;
      cVar5 = (code)FUN_ram_2229((char)((ushort)unaff_AF_ >> 8) + (char)pcVar18);
      return cVar5;
    case 0x16:
    case 0x98:
    case 0xa8:
    case 0xb8:
    case 200:
    case 0xd8:
    case 0xe8:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x17:
    case 0x99:
    case 0xa9:
    case 0xb9:
    case 0xc9:
    case 0xd9:
    case 0xe9:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x18:
    case 0x9a:
    case 0xaa:
    case 0xba:
    case 0xca:
    case 0xda:
    case 0xea:
      goto switchD_ram_0fbd_caseD_18;
    case 0x19:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x1a:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x1b:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    default:
DAT_ram_4000:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x1d:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x1e:
    case 0x49:
    case 0x4b:
      puVar16 = (undefined1 *)CONCAT11(puVar23[0xfbf],*pcVar24);
      cVar13 = (char)cVar5 + cVar13;
code_r0x20a9:
      blitScrollTileGrid(cVar13,pcVar18,puVar16,param_1);
      puVar23 = &UNK_ram_1487;
LAB_ram_20bf:
      puVar16 = switchD_ram:0fbd::caseD_83;
      cVar5 = DAT_ram_8119;
      cVar14 = DAT_ram_827d;
      DAT_ram_8003 = DAT_ram_827d;
      switchD_ram:0fbd::caseD_5a = (code)switchD_ram:0fbd::caseD_7f;
      switchD_ram:0fbd::caseD_40 = puVar23;
      do {
        do {
          *puVar16 = *puVar23;
          puVar16[1] = puVar23[1];
          puVar16 = puVar16 + 0x20;
          cVar14 = (code)((char)cVar14 + -1);
          puVar23 = puVar23 + 2;
        } while (cVar14 != (code)0x0);
        puVar16 = puVar16 + (byte)switchD_ram:0fbd::caseD_5a;
        cVar5 = (code)((char)cVar5 - 1);
        puVar23 = switchD_ram:0fbd::caseD_40;
        cVar14 = DAT_ram_8003;
      } while (cVar5 != (code)0x0);
      return DAT_ram_8003;
    case 0x1f:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x21:
switchD_ram_14c6_caseD_f0:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x22:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x23:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x24:
      return cVar5;
    case 0x25:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x26:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x27:
      DAT_ram_83d9 = (char)cVar5 + unaff_E_ & 0xef;
      DAT_ram_d002 = DAT_ram_83d9;
      goto code_r0x033c;
    case 0x28:
      DAT_ram_8122 = cVar5;
      if (cVar5 == (code)0x0) {
        stampHomeBayFly();
      }
      if (DAT_ram_8122 == (code)0x70) {
        stampHomeBaySlot();
      }
      if (0x30 < (byte)DAT_ram_8047) {
        if (DAT_ram_83fe == 0) {
          return (code)0x0;
        }
        cVar5 = (code)scanFrogInputAndDispatchHop();
        return cVar5;
      }
      if ((byte)DAT_ram_8044 < 0x15) {
holdFrogMissedHomeBay:
        if ((byte)DAT_ram_8047 < 0x2a) {
          DAT_ram_8004 = (code)0x1;
          cVar5 = (code)scanFrogInputAndDispatchHop();
          return cVar5;
        }
      }
      else if ((DAT_ram_8044 == (code)0x1c) || ((byte)DAT_ram_8044 < 0x1c)) {
        cVar5 = DAT_ram_8263;
        if (DAT_ram_83fd == '\x01') {
          cVar5 = DAT_ram_825e;
        }
        if (cVar5 != (code)0x0) {
          return cVar5;
        }
        if ((byte)DAT_ram_8047 < 0x2a) {
          if (DAT_ram_8121 == '\x01') {
            FUN_ram_2673();
          }
          stampHomeGoalAndResetFrog(&DAT_ram_ab64);
          if (DAT_ram_8134 != '\0') {
            armHomeGoalSprite();
            DAT_ram_8134 = '\0';
          }
          if (DAT_ram_83fd == '\x01') {
            DAT_ram_825e = (code)0x1;
            DAT_ram_825c = DAT_ram_825c + '\x01';
            return (code)0x1;
          }
          DAT_ram_8263 = (code)0x1;
          DAT_ram_825d = (code)((char)DAT_ram_825d + 1);
          return (code)0x1;
        }
      }
      else {
        if (((((byte)DAT_ram_8044 < 0x2e) || (DAT_ram_8044 == (code)0x35)) ||
            ((byte)DAT_ram_8044 < 0x35)) || ((byte)DAT_ram_8044 < 0x45)) goto holdFrogMissedHomeBay;
        if ((DAT_ram_8044 == (code)0x4c) || ((byte)DAT_ram_8044 < 0x4c)) {
          cVar5 = DAT_ram_8264;
          if (DAT_ram_83fd == '\x01') {
            cVar5 = DAT_ram_825f;
          }
          if (cVar5 != (code)0x0) {
            return cVar5;
          }
          if ((byte)DAT_ram_8047 < 0x2a) {
            if (DAT_ram_8121 == '\x02') {
              FUN_ram_2673();
            }
            stampHomeGoalAndResetFrog(&DAT_ram_aaa4);
            if (DAT_ram_8134 != '\0') {
              armHomeGoalSprite();
              DAT_ram_8134 = '\0';
            }
            if (DAT_ram_83fd == '\x01') {
              DAT_ram_825f = (code)0x1;
              DAT_ram_825c = DAT_ram_825c + '\x01';
              return (code)0x1;
            }
            DAT_ram_8264 = (code)0x1;
            DAT_ram_825d = (code)((char)DAT_ram_825d + 1);
            return (code)0x1;
          }
        }
        else {
          if ((((byte)DAT_ram_8044 < 0x5e) || (DAT_ram_8044 == (code)0x65)) ||
             (((byte)DAT_ram_8044 < 0x65 || ((byte)DAT_ram_8044 < 0x75))))
          goto holdFrogMissedHomeBay;
          if ((DAT_ram_8044 == (code)0x7c) || ((byte)DAT_ram_8044 < 0x7c)) {
            cVar5 = DAT_ram_8265;
            if (DAT_ram_83fd == '\x01') {
              cVar5 = DAT_ram_8260;
            }
            if (cVar5 != (code)0x0) {
              return cVar5;
            }
            if ((byte)DAT_ram_8047 < 0x2a) {
              if (DAT_ram_8121 == '\x03') {
                FUN_ram_2673();
              }
              stampHomeGoalAndResetFrog(&DAT_ram_a9e4);
              if (DAT_ram_8134 != '\0') {
                armHomeGoalSprite();
                DAT_ram_8134 = '\0';
              }
              if (DAT_ram_83fd == '\x01') {
                DAT_ram_8260 = (code)0x1;
                DAT_ram_825c = DAT_ram_825c + '\x01';
                return (code)0x1;
              }
              DAT_ram_8265 = (code)0x1;
              DAT_ram_825d = (code)((char)DAT_ram_825d + 1);
              return (code)0x1;
            }
          }
          else {
            if (((((byte)DAT_ram_8044 < 0x8e) || (DAT_ram_8044 == (code)0x95)) ||
                ((byte)DAT_ram_8044 < 0x95)) || ((byte)DAT_ram_8044 < 0xa5))
            goto holdFrogMissedHomeBay;
            if ((DAT_ram_8044 == (code)0xac) || ((byte)DAT_ram_8044 < 0xac)) {
              cVar5 = DAT_ram_8266;
              if (DAT_ram_83fd == '\x01') {
                cVar5 = DAT_ram_8261;
              }
              if (cVar5 != (code)0x0) {
                return cVar5;
              }
              if ((byte)DAT_ram_8047 < 0x2a) {
                if (DAT_ram_8121 == '\x04') {
                  FUN_ram_2673();
                }
                stampHomeGoalAndResetFrog(&DAT_ram_a924);
                if (DAT_ram_8134 != '\0') {
                  armHomeGoalSprite();
                  DAT_ram_8134 = '\0';
                }
                if (DAT_ram_83fd == '\x01') {
                  DAT_ram_8261 = (code)0x1;
                  DAT_ram_825c = DAT_ram_825c + '\x01';
                  return (code)0x1;
                }
                DAT_ram_8266 = (code)0x1;
                DAT_ram_825d = (code)((char)DAT_ram_825d + 1);
                return (code)0x1;
              }
            }
            else {
              if (((((byte)DAT_ram_8044 < 0xbe) || (DAT_ram_8044 == (code)0xc5)) ||
                  (((byte)DAT_ram_8044 < 0xc5 || ((byte)DAT_ram_8044 < 0xd5)))) ||
                 ((DAT_ram_8044 != (code)0xdc && (0xdb < (byte)DAT_ram_8044))))
              goto holdFrogMissedHomeBay;
              cVar5 = DAT_ram_8267;
              if (DAT_ram_83fd == '\x01') {
                cVar5 = DAT_ram_8262;
              }
              if (cVar5 != (code)0x0) {
                return cVar5;
              }
              if ((byte)DAT_ram_8047 < 0x2a) {
                if (DAT_ram_8121 == '\x05') {
                  FUN_ram_2673();
                }
                stampHomeGoalAndResetFrog(&DAT_ram_a864);
                if (DAT_ram_8134 != '\0') {
                  armHomeGoalSprite();
                  DAT_ram_8134 = '\0';
                }
                if (DAT_ram_83fd == '\x01') {
                  DAT_ram_8262 = (code)0x1;
                  DAT_ram_825c = DAT_ram_825c + '\x01';
                  return (code)0x1;
                }
                DAT_ram_8267 = (code)0x1;
                DAT_ram_825d = (code)((char)DAT_ram_825d + 1);
                return (code)0x1;
              }
            }
          }
        }
      }
      if (DAT_ram_826c != (code)0x0) {
        return DAT_ram_826c;
      }
      if (DAT_ram_8268 != '\0') {
        DAT_ram_8268 = DAT_ram_8268 + -1;
        cVar5 = (code)advanceHomeBaySlotCursor();
        return cVar5;
      }
      if (DAT_ram_8004 != (code)0x0) {
        return DAT_ram_8004;
      }
      pcVar24 = DAT_ram_8044;
      param_1 = DAT_ram_8047;
      bVar7 = DAT_ram_e000;
      if (((DAT_ram_e004 & 8) != 0) && (DAT_ram_83fd != '\x01')) {
        bVar7 = DAT_ram_e002;
      }
      pcVar18 = (code *)(ushort)bVar7;
      if (DAT_ram_8248 != '\0') goto animateFrogHop;
      if (((DAT_ram_e004 & 8) == 0) || (DAT_ram_83fd == '\x01')) {
        bVar7 = DAT_ram_e004 & 0x40;
      }
      else {
        bVar7 = DAT_ram_e004 & 1;
      }
      pcVar24 = DAT_ram_8044;
      param_1 = DAT_ram_8047;
      if (bVar7 != 0) {
        DAT_ram_824c = (code)0x0;
        cVar13 = DAT_ram_8249;
        DAT_ram_8250 = DAT_ram_824c;
        goto code_r0x1b22;
      }
      if (0xef < (byte)DAT_ram_8047) {
        return DAT_ram_8047;
      }
      if (DAT_ram_8250 == (code)0x0) {
        enqueueSoundCommand(4);
        if (pcVar24[1] != (code)0xde) {
          DAT_ram_8045 = 0xde;
          goto LAB_ram_1ba7;
        }
      }
      else {
LAB_ram_1ba7:
        DAT_ram_8250 = (code)((char)DAT_ram_8250 + '\x01');
        if (DAT_ram_8250 == (code)0x0) {
          return (code)0x0;
        }
      }
      DAT_ram_8250 = DAT_ram_8256;
animateFrogHop:
      if (DAT_ram_824c != (code)0x0) {
        return DAT_ram_824c;
      }
      DAT_ram_8248 = 1;
      cVar13 = (char)DAT_ram_8250 + -1;
      if (cVar13 == '\0') {
        cVar5 = DAT_ram_8250;
        DAT_ram_824c = DAT_ram_8250;
        DAT_ram_8248 = cVar13;
        DAT_ram_8250 = (code)cVar13;
        pcVar24[1] = (code)0xde;
        return cVar5;
      }
      DAT_ram_8250 = (code)cVar13;
      *param_1 = (code)(DAT_ram_8254 + (char)*param_1);
      pcVar24[1] = (code)0xdc;
      return (code)0xdc;
    case 0x29:
      if (DAT_ram_8299 != (code)0x0) {
        DAT_ram_8299 = (code)((char)DAT_ram_8299 + -1);
        return DAT_ram_8299;
      }
    case 0x50:
    case 0x60:
    case 0x70:
    case 0x80:
      DAT_ram_8299 = (code)0x30;
      DAT_ram_829a = (code)((char)DAT_ram_829a + 1);
      if ((&DAT_ram_2e68)[(byte)DAT_ram_829a] != -1) {
        return (code)0x30;
      }
      DAT_ram_829a = (code)0x0;
      DAT_ram_8299 = (code)0x0;
      switchD_ram:0fbd::caseD_1d = (code)0x0;
      return (code)0x0;
    case 0x2a:
switchD_ram_14c6_caseD_e6:
    case 0x2f:
      DAT_ram_8035 = 7;
      DAT_ram_8037 = 7;
      DAT_ram_8021 = (code)0x6;
      DAT_ram_8023 = (code)0x6;
      DAT_ram_8039 = 6;
      DAT_ram_803b = 6;
      cVar13 = '\n';
      puVar16 = &DAT_ram_800d;
      do {
        *puVar16 = 5;
        puVar16 = puVar16 + 2;
        cVar13 = cVar13 + -1;
      } while (cVar13 != '\0');
      DAT_ram_8029 = (code)0x5;
      DAT_ram_802b = 5;
      DAT_ram_8031 = 5;
      DAT_ram_8033 = 5;
      DAT_ram_800d = 2;
      DAT_ram_800f = 2;
      DAT_ram_8015 = (code)0x2;
      DAT_ram_8017 = 2;
      DAT_ram_8019 = 2;
      DAT_ram_801b = (code)0x2;
      return (code)0x2;
    case 0x2b:
      if (0xf < (byte)cVar5) {
        return cVar5;
      }
      DAT_ram_8004 = (code)0x1;
      DAT_ram_842c = 1;
      return (code)0x1;
    case 0x2c:
      pcVar18 = (code *)0x1100;
      switch(bVar7 >> 4) {
      case 0:
      case 5:
      case 0x12:
      case 0x21:
      case 0x25:
      case 0x2c:
      case 0x39:
      case 0x93:
      case 0xa4:
      case 0xb4:
      case 0xc4:
      case 0xd4:
      case 0xe4:
        cVar5 = (code)FUN_ram_1270(0x1112,&switchD_ram:14c6::caseD_c4);
        return cVar5;
      case 1:
      case 9:
      case 0x18:
      case 0x1d:
      case 0x9a:
      case 0xaa:
      case 0xba:
      case 0xca:
      case 0xd5:
      case 0xd7:
      case 0xda:
      case 0xe5:
      case 0xe7:
      case 0xea:
      case 0xef:
        goto dispatch_1222;
      case 2:
      case 10:
      case 0x29:
      case 0x2b:
      case 0x3b:
      case 0x4f:
      case 0x50:
      case 0x5f:
      case 0x60:
      case 0x6f:
      case 0x70:
      case 0x7f:
      case 0x80:
      case 0xf3:
        cVar5 = (code)FUN_ram_1270(0x112f,&switchD_ram:14c6::caseD_5e);
        return cVar5;
      case 3:
      case 0xa3:
        cVar5 = (code)FUN_ram_1270(0x1122,&switchD_ram:14c6::caseD_80);
        return cVar5;
      case 4:
      case 0x5a:
      case 0x6a:
      case 0x7a:
      case 0x8a:
      case 0xe3:
      case 0xf5:
      case 0xf8:
        cVar5 = (code)FUN_ram_1270(0x1112,&switchD_ram:14c6::caseD_a2);
        return cVar5;
      case 6:
      case 0x14:
      case 0x38:
      case 0x4a:
      case 0x4c:
      case 99:
      case 0x73:
      case 0x83:
      case 0x95:
      case 0x9c:
      case 0xa6:
      case 0xac:
      case 0xb6:
      case 0xbc:
      case 0xc6:
      case 0xcc:
      case 0xd6:
      case 0xdc:
      case 0xe6:
      case 0xec:
        cVar5 = (code)resolveFrogMoveAgainstLanes();
        return cVar5;
      case 7:
      case 0x10:
      case 0x1c:
      case 0x2a:
      case 0x2e:
      case 0x2f:
      case 0x32:
      case 0x33:
      case 0x35:
      case 0x3d:
      case 0x54:
      case 0x55:
      case 100:
      case 0x65:
      case 0x74:
      case 0x75:
      case 0x84:
      case 0x85:
      case 0x90:
      case 0x91:
      case 0xa2:
      case 0xb2:
      case 0xc2:
      case 0xd2:
      case 0xe2:
      case 0xed:
      case 0xfc:
        cVar5 = (code)resolveFrogMoveAgainstLanes();
        return cVar5;
      case 8:
      case 0x16:
      case 0x24:
      case 0x27:
      case 0x30:
      case 0x37:
      case 0x41:
      case 0x97:
      case 0x98:
      case 0xa5:
      case 0xa7:
      case 0xa8:
      case 0xb5:
      case 0xb7:
      case 0xb8:
      case 200:
      case 0xd8:
      case 0xe8:
        cVar5 = (code)FUN_ram_1270(0x113c,&switchD_ram:14c6::caseD_1a);
        return cVar5;
      default:
        cVar5 = (code)resolveFrogMoveAgainstLanes();
        return cVar5;
      case 0xc:
      case 0x17:
      case 0x19:
      case 0x28:
      case 0x31:
      case 0x42:
      case 0x57:
      case 0x59:
      case 0x67:
      case 0x69:
      case 0x77:
      case 0x79:
      case 0x87:
      case 0x89:
      case 0x99:
      case 0x9b:
      case 0x9e:
      case 0xa9:
      case 0xab:
      case 0xae:
      case 0xb9:
      case 0xbb:
      case 0xbe:
      case 0xc9:
      case 0xcb:
      case 0xce:
      case 0xd9:
      case 0xdb:
      case 0xde:
      case 0xe9:
      case 0xeb:
        cVar5 = (code)resolveFrogMoveAgainstLanes();
        return cVar5;
      case 0x11:
      case 0x13:
      case 0x15:
      case 0x1f:
      case 0x22:
      case 0x36:
      case 0x3a:
      case 0x3e:
      case 0x40:
      case 0x44:
      case 0x45:
      case 0x47:
      case 0x48:
      case 0x4e:
      case 0x5c:
      case 0x5e:
      case 0x6c:
      case 0x6e:
      case 0x7c:
      case 0x7e:
      case 0x8c:
      case 0x8e:
      case 0x92:
      case 0xf0:
      case 0xf1:
      case 0xf4:
      case 0xf7:
        cVar5 = (code)resolveFrogMoveAgainstLanes();
        return cVar5;
      case 0x1a:
      case 0x1b:
      case 0x34:
      case 0x43:
      case 0x4d:
      case 0x5d:
      case 0x6d:
      case 0x7d:
      case 0x8d:
      case 0xf6:
      case 0xf9:
      case 0xfa:
      case 0xfb:
      case 0xfd:
      case 0xfe:
      case 0xff:
        cVar5 = (code)FUN_ram_1270(0x1112,&switchD_ram:0fbd::caseD_d5);
        return cVar5;
      case 0x1e:
      case 0x49:
      case 0x4b:
      case 0xb3:
      case 0xc3:
      case 0xd3:
      case 0xee:
        cVar5 = (code)FUN_ram_1270(0x1112,&switchD_ram:0fbd::caseD_b5);
        return cVar5;
      case 0x26:
      case 0x3f:
      case 0x46:
      case 0x53:
      case 0x5b:
      case 0x6b:
      case 0x7b:
      case 0x8b:
      case 0xf2:
        cVar5 = (code)resolveFrogMoveAgainstLanes();
        return cVar5;
      case 0x3c:
      case 0x51:
      case 0x52:
      case 0x61:
      case 0x62:
      case 0x71:
      case 0x72:
      case 0x81:
      case 0x82:
      case 0xc5:
      case 199:
        cVar5 = (code)FUN_ram_1270(0x111f,&DAT_ram_8109);
        return cVar5;
      }
    case 0x2d:
      goto code_r0x001e;
    case 0x2e:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x30:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x31:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x32:
      goto switchD_ram_0fbd_caseD_32;
    case 0x33:
      goto switchD_ram_0fbd_caseD_33;
    case 0x35:
      goto switchD_ram_0fbd_caseD_35;
    case 0x36:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x37:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x38:
      goto switchD_ram_0fbd_caseD_38;
    case 0x39:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x3a:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x3b:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x3c:
      goto switchD_ram_0fbd_caseD_3c;
    case 0x3d:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x3e:
      goto switchD_ram_0fbd_caseD_3e;
    case 0x3f:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x40:
    case 0x5c:
    case 0x6c:
    case 0x7c:
    case 0x8c:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x41:
      *pcVar18 = cVar5;
      if (!bVar2) {
        return cVar5;
      }
code_r0x033c:
      issueSoundCommand(0xff);
drainForegroundThenYieldEachVblank:
      if (1 < (byte)DAT_ram_83d6) {
        dispatchGameModeFrame();
      }
      renderScoreHeader();
      if (DAT_ram_83d6 != (code)0x1) {
        renderCreditLine();
      }
      setUpPlayStartOnce();
      DAT_ram_8254 = '\x02';
      DAT_ram_8255 = '\x02';
      DAT_ram_8256 = (code)0x9;
      DAT_ram_8257 = (code)0x9;
      DAT_ram_8258 = (code)0x9;
      DAT_ram_8259 = (code)0x9;
      sVar8 = DAT_ram_83c7;
LAB_ram_036b:
      do {
        uVar11 = (ushort)sVar8 >> 8;
        cVar13 = (char)sVar8;
        sVar8 = sVar8 + -1;
      } while ((char)uVar11 != '\0' || cVar13 != '\0');
      if (DAT_ram_83fe == 0) {
        if (DAT_ram_83b3 != '\0') goto drainForegroundThenYieldEachVblank;
        if ((char)DAT_ram_e002 < '\0') {
          if ((bool)((DAT_ram_e002 & 0x7f) >> 6)) goto drainForegroundThenYieldEachVblank;
          bVar7 = 2;
        }
        else {
          bVar7 = 1;
        }
        if (DAT_ram_83e1 < bVar7) goto drainForegroundThenYieldEachVblank;
        bVar2 = ((DAT_ram_83e1 & 0xf) - bVar7 & 0x10) != 0;
        bVar3 = DAT_ram_83e1 < bVar7;
        DAT_ram_83e1 = BCDadjust(DAT_ram_83e1 - bVar7,bVar3,bVar2);
        BCDadjustCarry(DAT_ram_83e1,bVar3,bVar2);
        hasEvenParity(DAT_ram_83e1);
        puVar21 = &DAT_ram_8500;
        puVar16 = &DAT_ram_8501;
        sVar8 = 0x1ff;
        DAT_ram_8500 = 0;
        DAT_ram_8370 = bVar7;
        do {
          *puVar16 = *puVar21;
          puVar16 = puVar16 + 1;
          puVar21 = puVar21 + 1;
          sVar8 = sVar8 + -1;
        } while (sVar8 != 0);
        DAT_ram_83fd = '\x01';
        DAT_ram_83b3 = '\x01';
        DAT_ram_83b7 = 1;
        _DAT_ram_83b8 = 0x101;
        DAT_ram_83fe = bVar7;
        initNewGameScoreAndTimers();
        DAT_ram_803d = 3;
        clearSoundQueue();
        DAT_ram_8071 = 0;
        enqueueSoundCommand();
        enqueueSoundCommand(9);
        enqueueSoundCommand(10);
        enqueueSoundCommand(0xb);
        DAT_ram_829d = 0x20;
        DAT_ram_8382 = 0x1a0;
        DAT_ram_83d2 = 0;
        clearActivePlayerWorkRam();
        clearTilemapToTile16();
        loadActivePlayerLaneParams();
        DAT_ram_842f = '\0';
        DAT_ram_842d = 0;
        _DAT_ram_8293 = seatStackAndEnterColdBoot;
        puVar22 = &UNK_ram_8440;
        puVar23 = &UNK_ram_8441;
        sVar8 = 0x4f;
        UNK_ram_8440 = 0;
        do {
          *puVar23 = *puVar22;
          puVar23 = puVar23 + 1;
          puVar22 = puVar22 + 1;
          sVar8 = sVar8 + -1;
        } while (sVar8 != 0);
        DAT_ram_8004 = (code)0x0;
        DAT_ram_825a = 1;
      }
      if (DAT_ram_83ea == '\0') {
        if (DAT_ram_83cd == '\0') {
          if (DAT_ram_83fe != 1) {
            clearTilemapToTile16();
            swapInActivePlayerPages();
          }
          renderScoreHeader();
        }
        if (DAT_ram_826d != '\0') {
          advanceBoardForeground();
        }
        DAT_ram_83ea = renderFrogSceneAndTickTimer();
        renderTimeBar();
        DAT_ram_839e = 0x20;
        DAT_ram_839d = 0x10;
        DAT_ram_839c = 0x20;
        if (DAT_ram_83fe != 1) {
          raiseActivePlayerStartFlag();
        }
        DAT_ram_826d = 0;
        DAT_ram_83b6 = DAT_ram_83cd;
        renderLivesRow();
        cVar5 = (code)endForegroundPassAtPaceTail();
        return cVar5;
      }
      renderScoreHeader();
      sVar8 = DAT_ram_83c7;
      if (DAT_ram_83ce != '\0') {
        activateFrogObject();
        clearActivePlayerWorkRam();
        DAT_ram_839a = 0;
        DAT_ram_839b = 0;
        DAT_ram_83cc = 0;
        DAT_ram_83ea = '\0';
        puVar21 = &DAT_ram_83a0;
        puVar16 = &DAT_ram_83a1;
        sVar8 = 0xd;
        DAT_ram_83a0 = 0;
        do {
          *puVar16 = *puVar21;
          puVar16 = puVar16 + 1;
          puVar21 = puVar21 + 1;
          sVar8 = sVar8 + -1;
        } while (sVar8 != 0);
        enqueueSoundCommand(0x80);
        if (DAT_ram_83cf == '\0') {
          handOffToOtherPlayer();
          cVar5 = (code)endForegroundPassAtPaceTail();
          return cVar5;
        }
        blitGameOverLine();
        enqueueSoundCommand(0xc);
        enqueueSoundCommand(0xd);
        do {
          DAT_ram_83c5 = DAT_ram_83c5 + -1;
        } while (DAT_ram_83c5 != 0);
        if (DAT_ram_83fe == 1) {
          DAT_ram_825c = '\0';
          puVar21 = DAT_ram_825e;
          puVar16 = DAT_ram_825f;
          sVar8 = 4;
          DAT_ram_825e = (code)0x0;
          DAT_ram_83c5 = 0;
          do {
            *puVar16 = *puVar21;
            puVar16 = puVar16 + 1;
            puVar21 = puVar21 + 1;
            sVar8 = sVar8 + -1;
            sVar4 = DAT_ram_83c5;
          } while (sVar8 != 0);
        }
        else {
          if (DAT_ram_83fd == '\x01') {
            DAT_ram_83c9 = 1;
            if (DAT_ram_83ca == '\0') {
              clearTilemapToTile16();
              handOffToOtherPlayer();
              DAT_ram_83fe = 1;
              DAT_ram_825c = 1;
              puVar21 = DAT_ram_825e;
              puVar16 = DAT_ram_825f;
              sVar8 = 4;
              DAT_ram_825e = (code)0x0;
              do {
                *puVar16 = *puVar21;
                puVar16 = puVar16 + 1;
                puVar21 = puVar21 + 1;
                sVar8 = sVar8 + -1;
              } while (sVar8 != 0);
              puVar23 = &UNK_ram_8600;
              puVar16 = DAT_ram_80ff;
              sVar8 = 0xb7;
              do {
                *puVar16 = *puVar23;
                puVar16 = puVar16 + 1;
                puVar23 = puVar23 + 1;
                sVar8 = sVar8 + -1;
              } while (sVar8 != 0);
              puVar21 = &DAT_ram_85c0;
              puVar16 = &switchD_ram:14c6::caseD_1e;
              sVar8 = 0x2b;
              do {
                *puVar16 = *puVar21;
                puVar16 = puVar16 + 1;
                puVar21 = puVar21 + 1;
                sVar8 = sVar8 + -1;
              } while (sVar8 != 0);
              DAT_ram_803f = 1;
              cVar5 = (code)endForegroundPassAtPaceTail();
              return cVar5;
            }
            DAT_ram_825c = 0;
            puVar21 = DAT_ram_825e;
            puVar16 = DAT_ram_825f;
            sVar8 = 4;
            DAT_ram_825e = (code)0x0;
            DAT_ram_83c5 = 0;
            do {
              *puVar16 = *puVar21;
              puVar16 = puVar16 + 1;
              puVar21 = puVar21 + 1;
              sVar8 = sVar8 + -1;
            } while (sVar8 != 0);
            cVar5 = (code)coldStartClearPlayRamAndSetMode();
            return cVar5;
          }
          DAT_ram_83ca = '\x01';
          sVar4 = 0;
          if (DAT_ram_83c9 == '\0') {
            clearTilemapToTile16();
            handOffToOtherPlayer();
            DAT_ram_83fe = 1;
            DAT_ram_825d = (code)0x1;
            puVar21 = DAT_ram_8263;
            puVar16 = DAT_ram_8264;
            sVar8 = 4;
            DAT_ram_8263 = (code)0x0;
            do {
              *puVar16 = *puVar21;
              puVar16 = puVar16 + 1;
              puVar21 = puVar21 + 1;
              sVar8 = sVar8 + -1;
            } while (sVar8 != 0);
            puVar23 = &UNK_ram_86c0;
            puVar16 = &switchD_ram:14c6::caseD_1e;
            sVar8 = 0x2b;
            do {
              *puVar16 = *puVar23;
              puVar16 = puVar16 + 1;
              puVar23 = puVar23 + 1;
              sVar8 = sVar8 + -1;
            } while (sVar8 != 0);
            DAT_ram_803f = 1;
            puVar21 = &DAT_ram_8500;
            puVar16 = DAT_ram_80ff;
            sVar8 = 0xb7;
            do {
              *puVar16 = *puVar21;
              puVar16 = puVar16 + 1;
              puVar21 = puVar21 + 1;
              sVar8 = sVar8 + -1;
            } while (sVar8 != 0);
            cVar5 = (code)endForegroundPassAtPaceTail();
            return cVar5;
          }
        }
        DAT_ram_83c5 = sVar4;
        DAT_ram_825d = (code)0x0;
        puVar21 = DAT_ram_8263;
        puVar16 = DAT_ram_8264;
        sVar8 = 4;
        DAT_ram_8263 = (code)0x0;
        do {
          *puVar16 = *puVar21;
          puVar16 = puVar16 + 1;
          puVar21 = puVar21 + 1;
          sVar8 = sVar8 + -1;
        } while (sVar8 != 0);
        clearTilemapToTile16();
        clearActivePlayerWorkRam();
        renderCreditLine();
        packScoreRankPair();
        renderScoreHeader();
        puVar21 = &switchD_ram:14c6::caseD_1a;
        puVar16 = &DAT_ram_8101;
        sVar8 = 0x15f;
        switchD_ram:14c6::caseD_1a = 0;
        do {
          *puVar16 = *puVar21;
          puVar16 = puVar16 + 1;
          puVar21 = puVar21 + 1;
          sVar8 = sVar8 + -1;
        } while (sVar8 != 0);
        puVar16 = &switchD_ram:0fbd::caseD_36;
        puVar17 = &switchD_ram:0fbd::caseD_40;
        sVar8 = 4;
        _caseD_36 = _caseD_36 & 0xffff00;
        do {
          *(undefined1 *)puVar17 = *puVar16;
          puVar17 = (undefined2 *)((short)puVar17 + 1);
          puVar16 = puVar16 + 1;
          sVar8 = sVar8 + -1;
        } while (sVar8 != 0);
        puVar21 = &switchD_ram:14c6::caseD_1e;
        puVar16 = &DAT_ram_800d;
        sVar8 = 0x2e;
        switchD_ram:14c6::caseD_1e = 0;
        do {
          *puVar16 = *puVar21;
          puVar16 = puVar16 + 1;
          puVar21 = puVar21 + 1;
          sVar8 = sVar8 + -1;
        } while (sVar8 != 0);
        DAT_ram_83c3 = 0;
        DAT_ram_83fe = 0;
        DAT_ram_83bf = 0;
        DAT_ram_83c9 = '\0';
        DAT_ram_83ca = '\0';
        DAT_ram_b810 = 0;
        DAT_ram_b80c = 0;
        _DAT_ram_8293 = seatStackAndEnterColdBoot;
        DAT_ram_83bb = 0;
        DAT_ram_83cb = 0;
        DAT_ram_83d8 = (code)0x0;
        DAT_ram_83c4 = 0;
        DAT_ram_83ba = (code)0x0;
        DAT_ram_8295 = 0;
        switchD_ram:0fbd::caseD_1d = (code)0x0;
        cVar13 = '\x03';
        DAT_ram_83d6 = (code)0x3;
        goto code_r0x05cd;
      }
      goto LAB_ram_036b;
    case 0x42:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x43:
    case 0x5d:
    case 0x6d:
    case 0x7d:
    case 0x8d:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x44:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x45:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x47:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x48:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x4a:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x4c:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x4d:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x4e:
    case 0x5e:
    case 0x6e:
    case 0x7e:
      cVar5 = (code)0x0;
switchD_ram_0fbd_caseD_35:
      pcVar24 = (code *)CONCAT11(cVar5,cVar5);
      pcVar18 = (code *)((ushort)(byte)param_3[2] << 8);
switchD_ram_14c6_caseD_26:
      pcVar18 = (code *)((ushort)(byte)((char)((ushort)pcVar18 >> 8) - 1) << 8);
switchD_ram_14c6_caseD_8c:
      do {
        pcVar24 = pcVar24 + (short)param_1;
        bVar7 = (char)((ushort)pcVar18 >> 8) - 1;
        pcVar18 = (code *)((ushort)bVar7 << 8);
      } while (bVar7 != 0);
      cVar13 = '\x02';
      if (DAT_ram_8110 != 'P') {
        if (DAT_ram_8110 != -0x80) {
          if (DAT_ram_8110 == -0x60) {
            do {
              FUN_ram_2178(&UNK_ram_2198);
              cVar13 = cVar13 + -1;
            } while (cVar13 != '\0');
            DAT_ram_8107 = 1;
            cVar5 = (code)FUN_ram_2188();
            return cVar5;
          }
          if (DAT_ram_8110 != -0x50) {
            if (DAT_ram_8110 != -0x30) {
              cVar5 = (code)FUN_ram_2188(pcVar24 + -0x57f8);
              return cVar5;
            }
            goto LAB_ram_213e;
          }
        }
        do {
          FUN_ram_2178(&UNK_ram_2194);
          cVar13 = cVar13 + -1;
        } while (cVar13 != '\0');
        if (DAT_ram_8107 != '\0') {
          DAT_ram_8107 = 0;
          cVar5 = (code)FUN_ram_2188();
          return cVar5;
        }
        DAT_ram_811a = (code)(param_3[2] + -1);
        return (code)(param_3[2] + -1);
      }
LAB_ram_213e:
      do {
        FUN_ram_2178(&UNK_ram_2190);
        cVar13 = cVar13 + -1;
      } while (cVar13 != '\0');
      cVar5 = (code)FUN_ram_2188();
      return cVar5;
    case 0x4f:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x51:
    case 0x61:
    case 0x71:
    case 0x81:
      if (DAT_ram_829b == (code)0x0) {
        return (code)0x0;
      }
      driveAttractDemoFrogHop();
      orchestrateCollisionsAndFrogInput();
      advanceAttractDemoFrogHop();
      renderFrogSceneAndTickTimer();
      driveScoreDisplayCountdown();
      advanceScrollLaneObjects();
      advanceAnimationFrameBuffer();
      dispatchFrogMoveAgainstLanes();
      driveFrogDeathAnimation();
      tickGatedCountdown();
      cVar5 = (code)moveLaneObjectsAndCarryFrog();
      return cVar5;
    case 0x52:
    case 0x62:
    case 0x72:
    case 0x82:
      param_3[3] = (char)cVar5;
      return cVar5;
    case 0x53:
    case 99:
      cVar5 = (code)((byte)*pcVar18 ^ 0xf);
code_r0x13f3:
      pcVar18 = (code *)0xf00;
      cVar5 = (code)((byte)cVar5 ^ 0xf);
code_r0x13f5:
      if ((char)((ushort)pcVar18 >> 8) != '\x01') {
        param_1 = param_1 + 1;
        goto LAB_ram_13a3;
      }
      *param_1 = cVar5;
      param_1 = (code *)CONCAT11(0xa8,cVar12);
      in_stack_00000000 = 0;
      do {
        if ((byte)cVar5 < (byte)SUB21(param_1,0)) {
          if ((byte)DAT_ram_8047 < 0x80) {
            return DAT_ram_8047;
          }
          DAT_ram_8004 = (code)0x1;
          return (code)0x1;
        }
        do {
          bVar7 = (char)((ushort)in_stack_00000000 >> 8) - 1;
          in_stack_00000000 = (ushort)bVar7 << 8;
          if (bVar7 == 0) {
            if (0x7f < (byte)DAT_ram_8047) {
              return DAT_ram_8047;
            }
            goto code_r0x12d0;
          }
LAB_ram_13a3:
          pcVar24 = pcVar24 + 1;
          cVar5 = *pcVar24;
        } while ((byte)cVar5 < (byte)SUB21((ushort)param_1 >> 8,0));
      } while( true );
    case 0x54:
      cVar5 = (code)((char)cVar5 + cVar12 + -1);
      if (cVar5 != (code)0x0) {
        return cVar5;
      }
      if (DAT_ram_829b != (code)0x0) {
        return DAT_ram_829b;
      }
      DAT_ram_83b4 = DAT_ram_829b;
      initDisplayFieldOnce();
      clearAndSeedScoreField();
      loadActivePlayerLaneParams();
      switchD_ram:0fbd::caseD_1d = (code)0x0;
      renderFrogAndArmObjects();
      blitFourTileGroupColumn(&UNK_ram_a850);
      resetFrogObject();
      dispatchFrogAnimationArm();
      switchD_ram:0fbd::caseD_1d = (code)0x1;
      DAT_ram_829b = (code)0x1;
      return (code)0x1;
    case 0x55:
    case 0x65:
    case 0x75:
    case 0x85:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x56:
    case 0x58:
switchD_ram_0fbd_caseD_56:
      *pcVar18 = cVar5;
      pcVar9 = (char *)CONCAT11((char)((ushort)pcVar18 >> 8),(char)pcVar18 + -2);
      cVar13 = *pcVar9 + '\x01';
      *pcVar9 = cVar13;
      puVar23 = &UNK_ram_abde;
      do {
        puVar23 = puVar23 + -0x20;
        cVar13 = cVar13 + -1;
      } while (cVar13 != '\0');
      *puVar23 = 0x4d;
      cVar5 = (code)enqueueSoundCommand(7);
LAB_ram_0936:
      if (param_1 <= DAT_ram_83ef) {
        return cVar5;
      }
      DAT_ram_83ef = param_1;
      return cVar5;
    case 0x57:
    case 0x67:
    case 0x77:
    case 0x87:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x5a:
    case 0x6a:
    case 0x7a:
    case 0x8a:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x5b:
    case 0x6b:
    case 0x7b:
    case 0x8b:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x5f:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x66:
    case 0x68:
      *param_1 = cVar5;
dispatch_1222:
      cVar5 = (code)FUN_ram_1270(CONCAT11((char)((ushort)pcVar18 >> 8),0x5c),
                                 &switchD_ram:14c6::caseD_3c);
      return cVar5;
    case 0x6f:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x73:
      goto code_r0x13f3;
    case 0x74:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x76:
    case 0x78:
      cVar13 = (char)cVar5 + cVar13;
code_r0x1b22:
      if (cVar13 != '\0') goto animateFrogHop;
      if ((char)(DAT_ram_824b + DAT_ram_824a) == '\0') {
        if (((DAT_ram_e004 & 8) == 0) || (DAT_ram_83fd == '\x01')) {
          bVar7 = DAT_ram_e004 & 0x10;
        }
        else {
          bVar7 = DAT_ram_e000 & 1;
        }
        if (bVar7 == 0) {
          if (DAT_ram_8251 == (code)0x0) {
            enqueueSoundCommand(4);
            if (pcVar24[1] != (code)0x1e) {
              DAT_ram_8045 = 0x1e;
              goto LAB_ram_1bfa;
            }
          }
          else {
LAB_ram_1bfa:
            DAT_ram_8251 = (code)((char)DAT_ram_8251 + '\x01');
            if (DAT_ram_8251 == (code)0x0) {
              return (code)0x0;
            }
          }
          DAT_ram_8251 = DAT_ram_8257;
animateFrogHop:
          advanceHomeBaySlotCursor();
          if (DAT_ram_824d != (code)0x0) {
            return DAT_ram_824d;
          }
          DAT_ram_8249 = 1;
          cVar13 = (char)DAT_ram_8251 + -1;
          if (cVar13 == '\0') {
            DAT_ram_824d = DAT_ram_8251;
            DAT_ram_8249 = cVar13;
            DAT_ram_8251 = (code)cVar13;
            pcVar24[1] = (code)0x1e;
            cVar5 = (code)scoreFrogRowProgress();
            return cVar5;
          }
          DAT_ram_8251 = (code)cVar13;
          *param_1 = (code)((char)*param_1 - DAT_ram_8254);
          pcVar24[1] = (code)0x1c;
          return (code)0x1c;
        }
        DAT_ram_824d = (code)0x0;
        DAT_ram_8251 = DAT_ram_824d;
      }
      if (DAT_ram_824a != '\0') goto animateFrogHop;
      if (((ushort)pcVar18 & 0x10) != 0) {
        DAT_ram_824e = (code)0x0;
        DAT_ram_8252 = (code)0x0;
        if (DAT_ram_824b != '\0') goto animateFrogHop;
        if (((ushort)pcVar18 & 0x20) != 0) {
          DAT_ram_824f = (code)0x0;
          DAT_ram_8253 = (code)0x0;
          return (code)0x0;
        }
        if ((byte)DAT_ram_8047 < 0x30) {
          return DAT_ram_8047;
        }
        if ((byte)DAT_ram_8044 < 0x20) {
          return DAT_ram_8044;
        }
        if (DAT_ram_8253 == (code)0x0) {
          enqueueSoundCommand(4);
          if (pcVar24[1] != (code)0x21) {
            DAT_ram_8045 = 0x21;
            goto LAB_ram_1cc2;
          }
        }
        else {
LAB_ram_1cc2:
          DAT_ram_8253 = (code)((char)DAT_ram_8253 + '\x01');
          if (DAT_ram_8253 == (code)0x0) {
            return (code)0x0;
          }
        }
        DAT_ram_8253 = DAT_ram_8259;
animateFrogHop:
        if (DAT_ram_824f != (code)0x0) {
          return DAT_ram_824f;
        }
        DAT_ram_824b = 1;
        cVar13 = (char)DAT_ram_8253 + -1;
        if (cVar13 == '\0') {
          cVar5 = DAT_ram_8253;
          DAT_ram_824f = DAT_ram_8253;
          DAT_ram_824b = cVar13;
          DAT_ram_8253 = (code)cVar13;
          pcVar24[1] = (code)0x21;
          return cVar5;
        }
        DAT_ram_8253 = (code)cVar13;
        *pcVar24 = (code)((char)*pcVar24 - DAT_ram_8255);
        pcVar24[1] = (code)0x1f;
        return (code)0x1f;
      }
      if ((byte)DAT_ram_8047 < 0x30) {
        return DAT_ram_8047;
      }
      if (0xdf < (byte)DAT_ram_8044) {
        return DAT_ram_8044;
      }
      if (DAT_ram_8252 == (code)0x0) {
        enqueueSoundCommand(4);
        if (pcVar24[1] != (code)0xa1) {
          DAT_ram_8045 = 0xa1;
          goto LAB_ram_1c63;
        }
      }
      else {
LAB_ram_1c63:
        DAT_ram_8252 = (code)((char)DAT_ram_8252 + '\x01');
        if (DAT_ram_8252 == (code)0x0) {
          return (code)0x0;
        }
      }
      DAT_ram_8252 = DAT_ram_8258;
animateFrogHop:
      if (DAT_ram_824e != (code)0x0) {
        return DAT_ram_824e;
      }
      DAT_ram_824a = 1;
      cVar13 = (char)DAT_ram_8252 + -1;
      if (cVar13 == '\0') {
        cVar5 = DAT_ram_8252;
        DAT_ram_824e = DAT_ram_8252;
        DAT_ram_824a = cVar13;
        DAT_ram_8252 = (code)cVar13;
        pcVar24[1] = (code)0xa1;
        return cVar5;
      }
      DAT_ram_8252 = (code)cVar13;
      *pcVar24 = (code)((char)*pcVar24 + DAT_ram_8255);
      pcVar24[1] = (code)0x9f;
      return (code)0x9f;
    case 0x7f:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x83:
      goto code_r0x13f5;
    case 0x84:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x86:
    case 0x88:
      if (DAT_ram_825e != (code)0x0) {
        return DAT_ram_825e;
      }
      DAT_ram_ab64 = 0x2c;
      DAT_ram_ab65 = 0x2d;
      DAT_ram_ab84 = 0x2e;
      DAT_ram_ab85 = 0x2f;
      return (code)0x0;
    case 0x8e:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x8f:
      break;
    case 0x90:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x91:
      if (!bVar2) {
                    /* WARNING: Bad instruction - Truncating control flow here */
        halt_baddata();
      }
dispatch_011c:
      advanceAnimationFrameBuffer();
      goto LAB_ram_0245;
    case 0x92:
      goto LAB_ram_0205;
    case 0x94:
    case 0x96:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x97:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x9b:
    case 0xab:
    case 0xbb:
    case 0xcb:
    case 0xdb:
    case 0xeb:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x9d:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xa1:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xa5:
    case 0xa7:
code_r0x8136:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xad:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xb1:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xb3:
      goto switchD_ram_0fbd_caseD_b3;
    case 0xb5:
    case 0xb7:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xbd:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xc1:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xc3:
      goto switchD_ram_0fbd_caseD_c3;
    case 0xc5:
    case 199:
code_r0x8148:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xcd:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xd1:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xd3:
      goto switchD_ram_0fbd_caseD_e3;
    case 0xd5:
    case 0xd7:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xdd:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xe1:
switchD_ram_0fbd_caseD_e1:
      if (cVar5 == (code)0x0) {
        driveAttractDemoSequencer();
      }
      driveInPlayFrameUpdate();
      cVar5 = (code)0x0;
      DAT_ram_83cd = '\0';
      DAT_ram_83cf = '\0';
      DAT_ram_83b5 = '\0';
      _DAT_ram_8293 = seatStackAndEnterColdBoot;
      puVar21 = &DAT_ram_825c;
      puVar16 = DAT_ram_825d;
      sVar8 = 0xb;
      DAT_ram_825c = '\0';
      do {
        *puVar16 = *puVar21;
        puVar16 = puVar16 + 1;
        puVar21 = puVar21 + 1;
        sVar8 = sVar8 + -1;
      } while (sVar8 != 0);
      UNK_ram_83af = 0x80;
      pcVar24 = (code *)&UNK_ram_83b0;
      UNK_ram_83b0 = 0;
      goto switchD_ram_0fbd_caseD_18;
    case 0xe2:
      cVar5 = (code)((char)cVar5 + 0xf);
code_r0x1115:
      cVar5 = (code)renderFrogAnimTileColumns(cVar5,pcVar18,pcVar24);
      return cVar5;
    case 0xe3:
      goto switchD_ram_0fbd_caseD_e3;
    case 0xe5:
    case 0xe7:
code_r0x815a:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xed:
      clearTilemapToTile16();
      clearTilemapToTile16();
      clearTilemapToTile16();
      clearTilemapToTile16();
      clearTilemapToTile16();
      goto switchD_ram_0fbd_caseD_32;
    case 0xee:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xef:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xf0:
      if (cVar5 != (code)0x55) {
        cVar5 = (code)initColdBootAndEnterMainLoop(switchD_ram:0fbd::caseD_47);
        return cVar5;
      }
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xf1:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xf2:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xf3:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xf4:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xf5:
      do {
        pcVar18 = (code *)((ushort)pcVar18 & 0xff00);
switchD_ram_0fbd_caseD_f8:
        while( true ) {
          bVar7 = (byte)pcVar24 >> 7;
          bVar26 = (char)((ushort)pcVar24 >> 8) << 1;
          pcVar24 = (code *)CONCAT11(bVar26 | bVar7,(byte)pcVar24 << 1 | bVar7);
          bVar7 = (char)((ushort)pcVar18 >> 8) - 1;
          pcVar18 = (code *)((ushort)bVar7 << 8);
          if (bVar7 == 0) {
            return cVar5;
          }
          cVar5 = (code)(bVar26 & 4);
          if ((bVar26 & 4) == 0) break;
          pcVar18 = (code *)((ushort)bVar7 << 8);
        }
      } while( true );
    case 0xf6:
    case 0xf9:
    case 0xfd:
    case 0xfe:
    case 0xff:
      cVar13 = (char)cVar5 + cVar12;
code_r0x01cc:
      if (cVar13 != '\0') {
        DAT_ram_8380 = '\0';
        DAT_ram_8382 = 0x40;
        copyRunUpTileColumn(&UNK_ram_aa51,&UNK_ram_2f7b);
      }
LAB_ram_01e2:
      if ((DAT_ram_8384 != '\0') && (DAT_ram_8384 = DAT_ram_8384 + -1, DAT_ram_8384 == '\0')) {
        blitFourTileGroupColumn(&UNK_ram_a850);
      }
      advanceScrollLaneObjects();
      advanceAnimationFrameBuffer();
      if (DAT_ram_8107 != '\0') {
        DAT_ram_8109 = DAT_ram_8109 + -1;
      }
LAB_ram_0205:
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
      goto LAB_ram_0245;
    case 0xf7:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xf8:
      goto switchD_ram_0fbd_caseD_f8;
    case 0xfa:
      cVar13 = (char)cVar5 + 0x7d;
code_r0x05cd:
      forceClearPlayerWorkRam(cVar13);
      cVar5 = (code)endForegroundPassAtPaceTail();
      return cVar5;
    case 0xfb:
      clearTilemapToTile16();
      if ((char)((ushort)pcVar18 >> 8) != '\x01') goto dispatch_14dd;
      halt();
      goto dispatch_14dd;
    case 0xfc:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    }
    bVar7 = switchD_ram:0fbd::caseD_36 + 1;
    _caseD_36 = CONCAT21(switchD_ram:0fbd::caseD_40,bVar7);
    in_AF = CONCAT11(bVar7,bVar7 < 0xb);
  } while (bVar7 < 0xb);
  cVar5 = (code)0x0;
  _caseD_36 = ZEXT23(switchD_ram:0fbd::caseD_40) << 8;
switchD_ram_0fbd_caseD_3c:
  return cVar5;
switchD_ram_0fbd_caseD_33:
  if (cVar5 == (code)0x3) {
    DAT_ram_83d8 = (code)((char)DAT_ram_83d8 - 1);
    DAT_ram_83d7 = '\0';
    DAT_ram_83b3 = '\0';
    fillTilemapBlock22x32();
    DAT_ram_8019 = 3;
    puVar16 = &DAT_ram_801f;
    uVar6 = SUB21(pcVar18,0);
    cVar13 = '\x05';
    do {
      *puVar16 = 0;
      puVar16 = (undefined1 *)CONCAT11((char)((ushort)puVar16 >> 8),(char)puVar16 + '\x04');
      cVar13 = cVar13 + -1;
    } while (cVar13 != '\0');
    placeScoreRankMarkers();
    unaff_BC_ = CONCAT11(0xd,uVar6);
    copyRunUpTileColumn(&UNK_ram_aaac,&UNK_ram_2ee5);
    cVar5 = (code)0x1;
    do {
      uVar6 = (undefined1)unaff_BC_;
      writeScoreDigitStepUp(CONCAT11(0xaa,(char)cVar5 * '\x02' + -0x33));
      unaff_BC_ = CONCAT11(3,uVar6);
      in_I = cVar5;
      bVar7 = copyRunUpTileColumn((char)((ushort)unaff_AF_ >> 8),unaff_BC_);
      unaff_AF_ = (ushort)bVar7 << 8;
      pcVar24 = (code *)&DAT_ram_83ef;
      cVar14 = cVar5;
      do {
        pcVar24 = (code *)CONCAT11((char)((ushort)pcVar24 >> 8),(char)pcVar24 + '\x02');
        cVar14 = (code)((char)cVar14 - 1);
      } while (cVar14 != (code)0x0);
      param_1 = (code *)(ushort)(byte)*pcVar24;
switchD_ram_0fbd_caseD_38:
      writeScoreField(CONCAT11(0xa9,(char)cVar5 * '\x02' + -0x13),
                      CONCAT11(*(undefined1 *)
                                CONCAT11((char)((ushort)pcVar24 >> 8),(char)pcVar24 + '\x01'),
                               (char)param_1));
      copyRunUpTileColumn(&UNK_ram_2fba);
      cVar5 = (code)((char)in_I + 1);
    } while (cVar5 != (code)0x6);
    DAT_ram_8039 = 0;
    cVar5 = (code)copyRunUpTileColumn(CONCAT11(0xf,(char)unaff_BC_),&UNK_ram_aafc,&UNK_ram_2f4d);
    return cVar5;
  }
switchD_ram_0c81_caseD_9c:
  bVar2 = DAT_ram_83e1 == 0;
switchD_ram_0c81_caseD_a0:
  cVar5 = DAT_ram_83d6;
  if (!bVar2) {
initInPlayBoardOnce:
    clearActivePlayerWorkRam(pcVar18);
    cVar14 = DAT_ram_83ba;
switchD_ram_0c81_caseD_d0:
    cVar5 = (code)0x0;
    if (cVar14 != (code)0x0) {
      return cVar14;
    }
switchD_ram_0c81_caseD_d2:
    pcVar24 = (code *)CONCAT11(cVar5,cVar5);
switchD_ram_0c81_caseD_d4:
    _DAT_ram_81b3 = pcVar24;
    _DAT_ram_8293 = pcVar24;
switchD_ram_0c81_caseD_da:
    switchD_ram:0fbd::caseD_1d = cVar5;
    DAT_ram_829a = cVar5;
switchD_ram_0c81_caseD_e0:
    cVar5 = (code)((char)cVar5 + 1);
    DAT_ram_83ba = cVar5;
switchD_ram_0c81_caseD_e4:
    loadActivePlayerLaneParams(cVar5);
    activateFrogObject();
switchD_ram_0c81_caseD_ea:
    fillTilemapBlock28x32();
    clearObjectBlocksAndMirrorToObjRam();
switchD_ram_0c81_caseD_f0:
    cVar5 = (code)0x4;
switchD_ram_0c81_caseD_f2:
    DAT_ram_8029 = (code)0x6;
    DAT_ram_801b = cVar5;
switchD_ram_0c81_caseD_fa:
    cVar13 = (char)&UNK_ram_2f77;
    copyRunUpTileColumn(&UNK_ram_aa28,&UNK_ram_2f77);
    copyRunUpTileColumn(&UNK_ram_aaad,cVar13 + '\x01');
    blitPlayerSelectPrompt();
    copyRunUpTileColumn(&UNK_ram_ab74,&UNK_ram_2f88);
    copyRunUpTileColumn(&UNK_ram_2fa8);
    puVar23 = &UNK_ram_2fae;
    copyRunUpTileColumn(&UNK_ram_2fae);
    copyRunUpTileColumn(puVar23 + 1);
    writeScoreField(&UNK_ram_a994,DAT_ram_2e08);
    cVar5 = (code)copyRunUpTileColumn(&UNK_ram_2fba);
    return cVar5;
  }
switchD_ram_0c81_caseD_a6:
  bVar2 = cVar5 == (code)0x4;
switchD_ram_0c81_caseD_a8:
  if (bVar2) {
    if (DAT_ram_83d7 == '\0') {
      DAT_ram_83d7 = '\x05';
    }
    goto LAB_ram_0c77;
  }
  goto code_r0x0d2d;
switchD_ram_0c81_caseD_a2:
  pcVar18 = (code *)CONCAT11(cVar12,cVar13 + -1);
  cVar5 = DAT_ram_83d6;
  goto switchD_ram_0c81_caseD_a6;
switchD_ram_0c81_caseD_9a:
  pcVar18 = pcVar18 + -1;
  goto switchD_ram_0c81_caseD_9c;
switchD_ram_0c81_caseD_8e:
  cVar5 = DAT_ram_83d8;
switchD_ram_0c81_caseD_92:
  if (cVar5 != (code)0x0) {
    return cVar5;
  }
switchD_ram_0c81_caseD_94:
  cVar5 = DAT_ram_83d6;
  goto switchD_ram_0fbd_caseD_33;
  while (bVar7 = (char)((ushort)pcVar18 >> 8) - 1, pcVar18 = (code *)((ushort)bVar7 << 8),
        bVar7 != 0) {
LAB_ram_12a1:
    pcVar24 = pcVar24 + 1;
    if (((byte)cVar19 <= (byte)*pcVar24) || ((byte)*pcVar24 < (byte)cVar14)) {
      if (0x7f < (byte)DAT_ram_8047) {
        cVar5 = (code)dispatchFrogMoveAgainstLanes();
        return cVar5;
      }
      goto code_r0x12e4;
    }
  }
  if (0x7f < (byte)DAT_ram_8047) {
    cVar5 = (code)resolveFrogMoveAgainstLanes();
    return cVar5;
  }
  goto code_r0x12d0;
switchD_ram_14c6_caseD_1c:
  while( true ) {
    cVar13 = (char)pcVar18;
    pcVar24 = (code *)((short)pcVar24 * 2);
    pcRam0206 = pcVar24;
    FUN_ram_2219(&UNK_ram_2231);
    if ((char)(cVar13 + -1) == '\0') break;
    pcVar18 = (code *)CONCAT11(2,cVar13 + -1);
  }
  cVar5 = (code)FUN_ram_2229();
  return cVar5;
switchD_ram_0fbd_caseD_32:
  clearTilemapToTile16();
  bVar7 = clearTilemapToTile16();
  pcVar18 = (code *)(ushort)bVar7;
  if (DAT_ram_83fe == 0) {
    return (code)0x0;
  }
code_r0x001e:
  DAT_ram_8300 = (code)((char)DAT_ram_8300 + '\x01');
  cVar5 = DAT_ram_8300;
  *(char *)CONCAT11(0x83,DAT_ram_8300) = (char)pcVar18;
  return cVar5;
switchD_ram_0fbd_caseD_18:
  *(code *)CONCAT11((char)((ushort)pcVar24 >> 8),(char)pcVar24 + '\x01') = cVar5;
LAB_ram_0245:
  DAT_ram_b808 = 1;
  return SUB21((ushort)param_4 >> 8,0);
  while (bVar7 = (char)((ushort)pcVar18 >> 8) - 1, pcVar18 = (code *)((ushort)bVar7 << 8),
        bVar7 != 0) {
LAB_ram_1284:
    pcVar24 = pcVar24 + 1;
    if (((byte)cVar19 <= (byte)*pcVar24) && ((byte)*pcVar24 < (byte)cVar14)) {
      if (0x7f < (byte)DAT_ram_8047) {
        cVar5 = (code)dispatchFrogMoveAgainstLanes();
        return cVar5;
      }
code_r0x12e4:
      if (DAT_ram_8004 != (code)0x0) {
        return DAT_ram_8004;
      }
      cVar5 = (code)((char)DAT_ram_8047 + 0xfU & 0xf);
      if (4 < (byte)cVar5) {
                    /* WARNING: Could not recover jumptable at 0x130a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        cVar5 = (code)(**(code **)(&DAT_ram_130b +
                                  (ushort)((byte)((char)DAT_ram_8047 + 0xfU) >> 4) * 2))();
        return cVar5;
      }
      return cVar5;
    }
  }
  if (0x7f < (byte)DAT_ram_8047) {
    cVar5 = (code)resolveFrogMoveAgainstLanes();
    return cVar5;
  }
code_r0x12d0:
  DAT_ram_8004 = (code)0x1;
  if (0x7f < (byte)DAT_ram_8047) {
    return DAT_ram_8047;
  }
  if ((byte)DAT_ram_8047 < 0x30) {
    return DAT_ram_8047;
  }
  DAT_ram_829c = 1;
  return (code)0x1;
LAB_ram_0c77:
  DAT_ram_83d7 = DAT_ram_83d7 + -1;
  cVar5 = (code)(DAT_ram_83d7 * '\x02');
  bVar2 = cVar5 == (code)0x0;
  puVar23 = (undefined *)(ushort)(byte)cVar5;
  bVar3 = ((ushort)(puVar23 + 0xc82) & 0x1000) != 0;
  bVar1 = &UNK_ram_f37d < puVar23;
  pcVar24 = (code *)(puVar23 + 0xc82);
  cVar13 = (char)pcVar18;
  cVar12 = (char)((ushort)pcVar18 >> 8);
  cVar14 = cVar5;
  switch(cVar5) {
  case (code)0x0:
    cVar5 = (code)dispatch_0cc0();
    return cVar5;
  case (code)0x2:
    cVar5 = (code)dispatch_0c51();
    return cVar5;
  case (code)0x4:
    cVar5 = (code)dispatch_0ceb();
    return cVar5;
  case (code)0x6:
    cVar5 = (code)dispatch_0cc6();
    return cVar5;
  case (code)0x8:
    cVar5 = (code)0x6;
  case (code)0xa:
    DAT_ram_801d = cVar5;
    DAT_ram_8023 = cVar5;
  case (code)0x10:
switchD_ram_0c81_caseD_10:
    DAT_ram_8029 = cVar5;
    DAT_ram_802f = cVar5;
switchD_ram_0c81_caseD_16:
    cVar5 = (code)0x3;
switchD_ram_0c81_caseD_18:
    DAT_ram_801b = cVar5;
    DAT_ram_8021 = cVar5;
switchD_ram_0c81_caseD_1e:
    DAT_ram_8027 = cVar5;
    DAT_ram_802d = cVar5;
switchD_ram_0c81_caseD_24:
    cVar5 = (code)0x10;
switchD_ram_0c81_caseD_26:
    pcVar24 = (code *)&UNK_ram_ab6d;
code_r0x0cab:
    writePackedBcdByte(cVar5,pcVar24);
switchD_ram_0c81_caseD_2c:
    puVar23 = &UNK_ram_2fba;
code_r0x0cb3:
    copyRunUpTileColumn(puVar23);
switchD_ram_0c81_caseD_32:
    cVar5 = (code)copyRunUpTileColumn(&UNK_ram_2ed1);
switchD_ram_0c81_caseD_38:
    DAT_ram_83d8 = (code)0x80;
    return cVar5;
  case (code)0xc:
    cVar5 = (code)((char)cVar5 + cVar12);
    DAT_ram_8023 = cVar5;
    goto switchD_ram_0c81_caseD_10;
  case (code)0xe:
    cVar5 = (code)((char)cVar5 + cVar12);
    goto switchD_ram_0c81_caseD_10;
  case (code)0x12:
    DAT_ram_802f = (code)((char)cVar5 + cVar12);
    goto switchD_ram_0c81_caseD_16;
  case (code)0x14:
  case (code)0x16:
    goto switchD_ram_0c81_caseD_16;
  case (code)0x18:
    goto switchD_ram_0c81_caseD_18;
  case (code)0x1a:
    cVar5 = (code)((char)cVar5 + cVar12);
    DAT_ram_8021 = cVar5;
    goto switchD_ram_0c81_caseD_1e;
  case (code)0x1c:
    cVar5 = (code)BCDadjust(cVar5,bVar1,bVar3);
    BCDadjustCarry(cVar5,bVar1,bVar3);
    hasEvenParity(cVar5);
  case (code)0x20:
    DAT_ram_802d = (code)((char)cVar5 + cVar12);
    goto switchD_ram_0c81_caseD_24;
  case (code)0x1e:
    goto switchD_ram_0c81_caseD_1e;
  case (code)0x22:
  case (code)0x24:
    goto switchD_ram_0c81_caseD_24;
  case (code)0x26:
    goto switchD_ram_0c81_caseD_26;
  case (code)0x28:
    cVar5 = (code)0x0;
    goto code_r0x0cab;
  case (code)0x2a:
  case (code)0x2c:
    goto switchD_ram_0c81_caseD_2c;
  case (code)0x2e:
    goto code_r0x0cb3;
  case (code)0x30:
    goto code_r0x0cb3;
  case (code)0x32:
    goto switchD_ram_0c81_caseD_32;
  case (code)0x34:
  case (code)0x36:
    DAT_ram_83d8 = (code)0x80;
    return cVar5;
  case (code)0x38:
    goto switchD_ram_0c81_caseD_38;
  case (code)0x3a:
    DAT_ram_83d8 = (code)0x80;
    return (code)(DAT_ram_83d7 * '\x04');
  case (code)0x3c:
    return (code)((char)cVar5 + cVar12);
  case (code)0x3e:
    pcVar24 = DAT_ram_83d8;
    goto code_r0x0cc3;
  case (code)0x40:
    cVar5 = (code)(DAT_ram_83d7 * '\x04');
code_r0x0cc3:
    *pcVar24 = (code)0xc0;
    return cVar5;
  case (code)0x42:
    if (!bVar2) {
      return cVar5;
    }
    return (code)0x0;
  case (code)0x44:
    pcVar24 = (code *)&UNK_ram_ab70;
    goto code_r0x0cc9;
  case (code)0x46:
code_r0x0cc9:
    cVar5 = (code)0x50;
code_r0x0ccb:
    writePackedBcdByte(cVar5,pcVar24);
  case (code)0x4c:
switchD_ram_0c81_caseD_4c:
    puVar23 = &UNK_ram_2fba;
code_r0x0cd3:
    cVar5 = (code)copyRunUpTileColumn(puVar23);
switchD_ram_0c81_caseD_52:
    puVar23 = &UNK_ram_2f43;
code_r0x0cd9:
    copyRunUpTileColumn(cVar5,puVar23);
switchD_ram_0c81_caseD_58:
    puVar23 = &UNK_ram_2fae;
code_r0x0cdf:
    copyRunUpTileColumn(puVar23);
switchD_ram_0c81_caseD_5e:
    pcVar24 = (code *)&UNK_ram_ab71;
code_r0x0ce3:
    puVar23 = &UNK_ram_2f17;
switchD_ram_0c81_caseD_64:
switchD_ram_0c81_caseD_66:
    cVar5 = (code)copyRunUpTileColumn(pcVar24,puVar23);
    DAT_ram_83d8 = (code)0x80;
    return cVar5;
  case (code)0x48:
    goto code_r0x0ccb;
  case (code)0x4a:
    goto switchD_ram_0c81_caseD_4c;
  case (code)0x4e:
    goto code_r0x0cd3;
  case (code)0x50:
    goto code_r0x0cd3;
  case (code)0x52:
    goto switchD_ram_0c81_caseD_52;
  case (code)0x54:
    cVar5 = (code)~(byte)cVar5;
    goto code_r0x0cd9;
  case (code)0x56:
    cVar5 = *pcVar18;
    goto code_r0x0cd9;
  case (code)0x58:
    goto switchD_ram_0c81_caseD_58;
  case (code)0x5a:
    goto code_r0x0cdf;
  case (code)0x5c:
    goto code_r0x0cdf;
  case (code)0x5e:
    goto switchD_ram_0c81_caseD_5e;
  case (code)0x60:
    goto code_r0x0ce3;
  case (code)0x62:
  case (code)0x64:
    goto switchD_ram_0c81_caseD_64;
  case (code)0x66:
    goto switchD_ram_0c81_caseD_66;
  case (code)0x68:
    RST1();
    pcVar24 = (code *)&UNK_ram_ab73;
  case (code)0x6c:
switchD_ram_0c81_caseD_6c:
    writePackedBcdWord(pcVar24,0x1000);
switchD_ram_0c81_caseD_72:
    puVar23 = &UNK_ram_2fba;
code_r0x0cf9:
    cVar5 = (code)copyRunUpTileColumn(puVar23);
switchD_ram_0c81_caseD_78:
    puVar23 = &UNK_ram_2f39;
code_r0x0cff:
    copyRunUpTileColumn(cVar5,puVar23);
switchD_ram_0c81_caseD_7e:
    puVar23 = &UNK_ram_2fae;
code_r0x0d03:
    copyRunUpTileColumn(puVar23);
switchD_ram_0c81_caseD_84:
    pcVar24 = (code *)&UNK_ram_ab74;
code_r0x0d09:
    puVar23 = &UNK_ram_2f2a;
switchD_ram_0c81_caseD_8a:
switchD_ram_0c81_caseD_8c:
    cVar5 = (code)copyRunUpTileColumn(pcVar24,puVar23);
    DAT_ram_83d8 = (code)0x80;
    return cVar5;
  case (code)0x6a:
    *pcVar24 = cVar5;
    goto switchD_ram_0c81_caseD_6c;
  case (code)0x6e:
    if (cVar12 != '\x01') {
      return cVar5;
    }
  case (code)0x70:
  case (code)0x72:
    goto switchD_ram_0c81_caseD_72;
  case (code)0x74:
    goto code_r0x0cf9;
  case (code)0x76:
    goto code_r0x0cf9;
  case (code)0x78:
    goto switchD_ram_0c81_caseD_78;
  case (code)0x7a:
    cVar5 = (code)~(byte)cVar5;
    goto code_r0x0cff;
  case (code)0x7c:
    cVar5 = *pcVar18;
    goto code_r0x0cff;
  case (code)0x7e:
    goto switchD_ram_0c81_caseD_7e;
  case (code)0x80:
    goto code_r0x0d03;
  case (code)0x82:
  case (code)0x84:
    goto switchD_ram_0c81_caseD_84;
  case (code)0x86:
    goto code_r0x0d09;
  case (code)0x88:
    pcVar24 = pcRam062f;
    goto switchD_ram_0c81_caseD_8c;
  case (code)0x8a:
    goto switchD_ram_0c81_caseD_8a;
  case (code)0x8c:
    goto switchD_ram_0c81_caseD_8c;
  case (code)0x8e:
    goto switchD_ram_0c81_caseD_8e;
  case (code)0x90:
    if (bVar1) {
      return cVar5;
    }
    cVar5 = (code)(DAT_ram_83d7 * '\x04');
  case (code)0x92:
    goto switchD_ram_0c81_caseD_92;
  case (code)0x94:
    goto switchD_ram_0c81_caseD_94;
  case (code)0x96:
    cVar5 = (code)(DAT_ram_83d7 * '\x04');
    goto switchD_ram_0fbd_caseD_33;
  case (code)0x98:
    pcVar18 = pcVar18 + 1;
    goto switchD_ram_0fbd_caseD_33;
  case (code)0x9a:
    goto switchD_ram_0c81_caseD_9a;
  case (code)0x9c:
    goto switchD_ram_0c81_caseD_9c;
  case (code)0x9e:
    goto switchD_ram_0c81_caseD_9c;
  case (code)0xa0:
    goto switchD_ram_0c81_caseD_a0;
  case (code)0xa2:
    goto switchD_ram_0c81_caseD_a2;
  case (code)0xa4:
    cVar5 = (code)((char)cVar5 + 0x7d);
  case (code)0xa6:
    goto switchD_ram_0c81_caseD_a6;
  case (code)0xa8:
    goto switchD_ram_0c81_caseD_a8;
  case (code)0xaa:
    pcVar18 = (code *)CONCAT11(cVar12,cVar13 + '\x01');
    goto code_r0x0d2d;
  case (code)0xac:
    *pcVar18 = cVar5;
code_r0x0d2d:
    if (cVar5 == (code)0x2) {
      DAT_ram_83d8 = (code)0xff;
      fillTilemapBlock28x32(pcVar18);
      DAT_ram_829b = (code)0x0;
      DAT_ram_8021 = (code)0x0;
      DAT_ram_801b = (code)0x5;
      DAT_ram_802b = 3;
      copyRunUpTileColumn(&UNK_ram_aa8d,&UNK_ram_2f5c);
      if (9 < (byte)DAT_ram_83e4) {
        return DAT_ram_83e4;
      }
      writeScoreDigitStepUp(&UNK_ram_ab15);
      copyRunUpTileColumn(&UNK_ram_2fae);
      copyRunUpTileColumn(&UNK_ram_2f73);
      cVar5 = (code)copyRunUpTileColumn(&UNK_ram_2f92);
      return cVar5;
    }
  case (code)0xb0:
switchD_ram_0c81_caseD_b0:
    bVar2 = cVar5 == (code)0x5;
switchD_ram_0c81_caseD_b2:
    if (!bVar2) {
      return cVar5;
    }
    pcVar24 = DAT_ram_83d8;
switchD_ram_0c81_caseD_b6:
    *pcVar24 = (code)0x30;
switchD_ram_0c81_caseD_b8:
    pcVar24 = (code *)CONCAT11((char)((ushort)pcVar24 >> 8),(char)pcVar24 + -1);
    cVar5 = (code)0x0;
switchD_ram_0c81_caseD_ba:
    *pcVar24 = cVar5;
    DAT_ram_8015 = cVar5;
    break;
  case (code)0xae:
    cVar5 = (code)((char)cVar5 + cVar12 + bVar1);
    goto switchD_ram_0c81_caseD_b0;
  case (code)0xb2:
    goto switchD_ram_0c81_caseD_b2;
  case (code)0xb4:
    if (bVar1) {
      return cVar5;
    }
  case (code)0xb6:
    goto switchD_ram_0c81_caseD_b6;
  case (code)0xb8:
    goto switchD_ram_0c81_caseD_b8;
  case (code)0xba:
    goto switchD_ram_0c81_caseD_ba;
  case (code)0xbc:
    break;
  case (code)0xc0:
    break;
  case (code)0xc2:
    if (bVar2) {
      cVar5 = (code)seatStackAndEnterColdBoot();
      *pcVar24 = (code)0xfe;
      if (cVar5 == (code)0x50) {
        puVar16 = &DAT_ram_a924;
      }
      else {
        if (cVar5 != (code)0x30) {
          if (cVar5 != (code)0x10) {
            return cVar5;
          }
          fillTwoByTwoTileBlock(&DAT_ram_ab64);
          fillTwoByTwoTileBlock(&DAT_ram_aaa4);
          fillTwoByTwoTileBlock(&DAT_ram_a9e4);
          fillTwoByTwoTileBlock(&DAT_ram_a924);
          fillTwoByTwoTileBlock(&DAT_ram_a864);
          DAT_ram_842f = 0;
          cVar5 = (code)awardExtraLife();
          return cVar5;
        }
        puVar16 = &DAT_ram_a864;
      }
      *puVar16 = 0xfc;
      puVar16[1] = 0xfd;
      puVar16[0x20] = 0xfe;
      puVar16[0x21] = 0xff;
      return cVar5;
    }
    pcVar18 = (code *)CONCAT11(cVar12,cVar13 + -1);
    goto switchD_ram_0c81_caseD_c6;
  case (code)0xc4:
    goto switchD_ram_0c81_caseD_c4;
  case (code)0xc6:
    goto switchD_ram_0c81_caseD_c6;
  case (code)0xc8:
    pcVar18 = (code *)CONCAT11(cVar12,cVar13 + '\x01');
  case (code)0xca:
    goto initInPlayBoardOnce;
  case (code)0xcc:
    goto initInPlayBoardOnce;
  case (code)0xce:
    cVar14 = (code)(DAT_ram_83d7 * '\x04');
  case (code)0xd0:
    goto switchD_ram_0c81_caseD_d0;
  case (code)0xd2:
    goto switchD_ram_0c81_caseD_d2;
  case (code)0xd4:
    goto switchD_ram_0c81_caseD_d4;
  case (code)0xd6:
    goto switchD_ram_0c81_caseD_d4;
  case (code)0xd8:
    cVar5 = (code)((char)cVar5 + cVar13);
  case (code)0xda:
    goto switchD_ram_0c81_caseD_da;
  case (code)0xdc:
    goto switchD_ram_0c81_caseD_da;
  case (code)0xde:
    cVar5 = (code)((char)cVar5 - bVar1);
  case (code)0xe0:
    goto switchD_ram_0c81_caseD_e0;
  case (code)0xe2:
    cVar5 = (code)(DAT_ram_83d7 * '\x04');
  case (code)0xe4:
    goto switchD_ram_0c81_caseD_e4;
  case (code)0xe6:
    pcRam04cd = pcVar24;
    goto switchD_ram_0c81_caseD_ea;
  case (code)0xe8:
  case (code)0xea:
    goto switchD_ram_0c81_caseD_ea;
  case (code)0xec:
    goto switchD_ram_0c81_caseD_ea;
  case (code)0xee:
    goto switchD_ram_0c81_caseD_f2;
  case (code)0xf0:
    goto switchD_ram_0c81_caseD_f0;
  case (code)0xf2:
    goto switchD_ram_0c81_caseD_f2;
  case (code)0xf4:
    cVar5 = (code)((char)cVar5 + cVar12);
    goto switchD_ram_0c81_caseD_f2;
  case (code)0xf6:
  case (code)0xf8:
  case (code)0xfa:
    goto switchD_ram_0c81_caseD_fa;
  case (code)0xfc:
    goto switchD_ram_0c81_caseD_fa;
  case (code)0xfe:
    *pcVar24 = cVar5;
    goto switchD_ram_0c81_caseD_fa;
  }
  puVar23 = &UNK_ram_2f01;
  pcVar24 = (code *)&UNK_ram_aaca;
switchD_ram_0c81_caseD_c4:
  pcVar18 = (code *)CONCAT11(0xd,(char)pcVar18);
switchD_ram_0c81_caseD_c6:
  copyRunUpTileColumn(pcVar18,pcVar24,puVar23);
  cVar5 = (code)blitMode3FinalStrip();
  return cVar5;
}

