
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void renderFrogAnimArm0(void)

{
  ushort uVar1;
  byte bVar2;
  byte bVar3;
  char cVar4;
  undefined1 *puVar5;
  undefined1 *puVar6;
  char *pcVar7;
  
code_r0x0fd4:
  bVar2 = DAT_ram_8272;
  cVar4 = DAT_ram_8271;
  puVar5 = DAT_ram_13ed;
  puVar6 = &switchD_ram:14c6::caseD_1a;
  pcVar7 = &switchD_ram:14c6::caseD_1a;
  switchD_ram:0fbd::caseD_5a = DAT_ram_8270;
  _caseD_36 = CONCAT21(&switchD_ram:0fbd::caseD_11,switchD_ram:0fbd::caseD_36);
  bVar3 = DAT_ram_8272;
  computeVramColumnIndex();
  if (switchD_ram:0fbd::caseD_1d == '\0') {
    puVar6[1] = ~bVar3 + 1;
    *pcVar7 = *pcVar7 + '\x01';
  }
  puVar6 = &switchD_ram:0fbd::caseD_11;
  DAT_ram_8003 = cVar4;
  while( true ) {
    *puVar5 = *puVar6;
    puVar5[1] = puVar6[1];
    puVar5 = puVar5 + 0x20;
    cVar4 = cVar4 + -1;
    if (cVar4 == '\0') break;
    puVar6 = puVar6 + 2;
  }
  if ((byte)(bVar2 - 1) != '\0') {
    renderFrogAnimTileColumns
              (CONCAT11(DAT_ram_8003,bVar2 - 1),puVar5 + switchD_ram:0fbd::caseD_5a,
               switchD_ram:0fbd::caseD_40);
    return;
  }
  do {
    cVar4 = switchD_ram:0fbd::caseD_36 + '\x01';
    uVar1 = switchD_ram:0fbd::caseD_40;
    _caseD_36 = CONCAT21(switchD_ram:0fbd::caseD_40,cVar4);
    switch(_caseD_36 & 0xff) {
    case 0:
      goto code_r0x0fd4;
    case 1:
      blitFrogAnimColumnOnTrigger();
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
      _caseD_36 = CONCAT21(&UNK_ram_1423,switchD_ram:0fbd::caseD_36);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_8274,DAT_ram_8275),switchD_ram:0fbd::caseD_53,&DAT_ram_8109,
                 &DAT_ram_8109);
      return;
    case 2:
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_5f;
      _caseD_36 = CONCAT21(&UNK_ram_143b,cVar4);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_8277,DAT_ram_8278),uRam13f1,&switchD_ram:14c6::caseD_3c,
                 &switchD_ram:14c6::caseD_3c);
      return;
    case 3:
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_6f;
      _caseD_36 = CONCAT21(&UNK_ram_1453,cVar4);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_827a,DAT_ram_827b),uRam13f3,&UNK_ram_811b,&UNK_ram_811b);
      return;
    case 4:
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_7f;
      _caseD_36 = CONCAT21(&UNK_ram_145f,cVar4);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_827d,DAT_ram_827e),switchD_ram:0fbd::caseD_83,
                 &switchD_ram:14c6::caseD_5e,&switchD_ram:14c6::caseD_5e);
      return;
    case 5:
      break;
    case 6:
      switchD_ram:0fbd::caseD_5a = DAT_ram_8282;
      _caseD_36 = CONCAT21(0x149f,cVar4);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_8283,DAT_ram_8284),DAT_ram_13f9,&switchD_ram:14c6::caseD_80,
                 &switchD_ram:14c6::caseD_80);
      return;
    case 7:
      switchD_ram:0fbd::caseD_5a = DAT_ram_8285;
      _caseD_36 = CONCAT21(0x14a7,cVar4);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_8286,DAT_ram_8287),DAT_ram_13fb,&switchD_ram:0fbd::caseD_b5,
                 &switchD_ram:0fbd::caseD_b5);
      return;
    case 8:
      switchD_ram:0fbd::caseD_5a = DAT_ram_8288;
      _caseD_36 = CONCAT21(0x14ab,cVar4);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_8289,DAT_ram_828a),DAT_ram_13fd,&switchD_ram:14c6::caseD_a2,
                 &switchD_ram:14c6::caseD_a2);
      return;
    case 9:
      switchD_ram:0fbd::caseD_5a = DAT_ram_828b;
      _caseD_36 = CONCAT21(0x14af,cVar4);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_828c,DAT_ram_828d),DAT_ram_13ff,&switchD_ram:0fbd::caseD_d5,
                 &switchD_ram:0fbd::caseD_d5);
      return;
    case 10:
      switchD_ram:0fbd::caseD_5a = DAT_ram_828e;
      _caseD_36 = CONCAT21(0x14b3,cVar4);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_828f,DAT_ram_8290),_UNK_ram_1401,&switchD_ram:14c6::caseD_c4,
                 &switchD_ram:14c6::caseD_c4);
      return;
    default:
      _caseD_36 = (uint3)uVar1 << 8;
      return;
    }
  } while( true );
}

