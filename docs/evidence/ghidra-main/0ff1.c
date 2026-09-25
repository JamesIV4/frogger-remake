
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void renderFrogAnimTileColumns
               (undefined2 param_1,undefined1 *param_2,undefined1 *param_3,char *param_4,
               undefined1 *param_5)

{
  ushort uVar1;
  char cVar2;
  undefined2 uVar3;
  char cVar4;
  
code_r0x0ff1:
  uVar3 = param_1;
  computeVramColumnIndex();
  if (switchD_ram:0fbd::caseD_1d == '\0') {
    param_5[1] = ~(byte)uVar3 + 1;
    *param_4 = *param_4 + '\x01';
  }
  DAT_ram_8003 = (undefined1)((ushort)param_1 >> 8);
  while( true ) {
    *param_2 = *param_3;
    param_2[1] = param_3[1];
    param_2 = param_2 + 0x20;
    cVar2 = (char)param_1;
    cVar4 = (char)((ushort)param_1 >> 8) + -1;
    param_1 = CONCAT11(cVar4,cVar2);
    if (cVar4 == '\0') break;
    param_3 = param_3 + 2;
  }
  cVar2 = cVar2 + -1;
  if (cVar2 != '\0') {
    renderFrogAnimTileColumns
              (CONCAT11(DAT_ram_8003,cVar2),param_2 + switchD_ram:0fbd::caseD_5a,
               switchD_ram:0fbd::caseD_40);
    return;
  }
  do {
    cVar2 = switchD_ram:0fbd::caseD_36 + '\x01';
    uVar1 = switchD_ram:0fbd::caseD_40;
    _caseD_36 = CONCAT21(switchD_ram:0fbd::caseD_40,cVar2);
    switch(_caseD_36 & 0xff) {
    case 0:
      goto renderFrogAnimArm0;
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
      _caseD_36 = CONCAT21(&UNK_ram_143b,cVar2);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_8277,DAT_ram_8278),uRam13f1,&switchD_ram:14c6::caseD_3c,
                 &switchD_ram:14c6::caseD_3c);
      return;
    case 3:
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_6f;
      _caseD_36 = CONCAT21(&UNK_ram_1453,cVar2);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_827a,DAT_ram_827b),uRam13f3,&UNK_ram_811b,&UNK_ram_811b);
      return;
    case 4:
      switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_7f;
      _caseD_36 = CONCAT21(&UNK_ram_145f,cVar2);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_827d,DAT_ram_827e),switchD_ram:0fbd::caseD_83,
                 &switchD_ram:14c6::caseD_5e,&switchD_ram:14c6::caseD_5e);
      return;
    case 5:
      break;
    case 6:
      switchD_ram:0fbd::caseD_5a = DAT_ram_8282;
      _caseD_36 = CONCAT21(0x149f,cVar2);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_8283,DAT_ram_8284),DAT_ram_13f9,&switchD_ram:14c6::caseD_80,
                 &switchD_ram:14c6::caseD_80);
      return;
    case 7:
      switchD_ram:0fbd::caseD_5a = DAT_ram_8285;
      _caseD_36 = CONCAT21(0x14a7,cVar2);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_8286,DAT_ram_8287),DAT_ram_13fb,&switchD_ram:0fbd::caseD_b5,
                 &switchD_ram:0fbd::caseD_b5);
      return;
    case 8:
      switchD_ram:0fbd::caseD_5a = DAT_ram_8288;
      _caseD_36 = CONCAT21(0x14ab,cVar2);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_8289,DAT_ram_828a),DAT_ram_13fd,&switchD_ram:14c6::caseD_a2,
                 &switchD_ram:14c6::caseD_a2);
      return;
    case 9:
      switchD_ram:0fbd::caseD_5a = DAT_ram_828b;
      _caseD_36 = CONCAT21(0x14af,cVar2);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_828c,DAT_ram_828d),DAT_ram_13ff,&switchD_ram:0fbd::caseD_d5,
                 &switchD_ram:0fbd::caseD_d5);
      return;
    case 10:
      switchD_ram:0fbd::caseD_5a = DAT_ram_828e;
      _caseD_36 = CONCAT21(0x14b3,cVar2);
      renderFrogAnimTileColumns
                (CONCAT11(DAT_ram_828f,DAT_ram_8290),_UNK_ram_1401,&switchD_ram:14c6::caseD_c4,
                 &switchD_ram:14c6::caseD_c4);
      return;
    default:
      _caseD_36 = (uint3)uVar1 << 8;
      return;
    }
  } while( true );
renderFrogAnimArm0:
  param_1 = CONCAT11(DAT_ram_8271,DAT_ram_8272);
  param_3 = &switchD_ram:0fbd::caseD_11;
  param_5 = &switchD_ram:14c6::caseD_1a;
  param_4 = &switchD_ram:14c6::caseD_1a;
  switchD_ram:0fbd::caseD_5a = DAT_ram_8270;
  _caseD_36 = CONCAT21(&switchD_ram:0fbd::caseD_11,cVar2);
  param_2 = DAT_ram_13ed;
  goto code_r0x0ff1;
}

