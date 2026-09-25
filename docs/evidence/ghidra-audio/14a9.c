
byte audio_14a9(byte param_1,byte *param_2)

{
  byte bVar1;
  byte bVar2;
  undefined1 *puVar3;
  undefined2 *puVar4;
  
  bVar2 = param_2[1] - 1;
  param_2[1] = bVar2;
  if (bVar2 != 0) {
    return param_1;
  }
  param_2[1] = DAT_ram_42c2;
  bVar2 = 0;
  if (param_2[8] != 0) {
    puVar3 = &DAT_ram_42c4;
    DAT_ram_42c4 = DAT_ram_42c4 + -1;
    if (DAT_ram_42c4 == '\0') {
      param_2[8] = 0;
      bVar2 = 0;
    }
    else {
      audio_024d();
      bVar2 = audio_0028(puVar3 + DAT_ram_42c5);
    }
  }
  if (((*param_2 & 1) == 0) && (bVar2 = param_2[7] - 1, -1 < (char)bVar2)) {
    param_2[7] = bVar2;
    bVar2 = audio_0018();
  }
  bVar1 = *param_2;
  *param_2 = bVar1 - 1;
  if ((byte)(bVar1 - 1) != 0) {
    return bVar2;
  }
  bVar2 = **(byte **)(param_2 + 2);
  if ((bVar2 & 0x1f) == 0) {
    audio_159c();
    bVar2 = audio_0018();
  }
  else {
    if ((bVar2 & 0x1f) == 0x1f) {
      *(byte **)(param_2 + 2) = *(byte **)(param_2 + 2) + 1;
      return (bVar2 & 0xe0) >> 4;
    }
    audio_159c();
    bVar2 = (bVar2 & 0x1f) - 1;
    puVar4 = (undefined2 *)(*(short *)(param_2 + 4) + (ushort)(byte)(bVar2 * '\x02' | bVar2 >> 7));
    audio_0028(*puVar4,(short)puVar4 + 1);
    param_2[8] = param_2[9];
    param_2[7] = param_2[6];
    bVar2 = audio_0018();
  }
  *(short *)(param_2 + 2) = *(short *)(param_2 + 2) + 1;
  return bVar2;
}

