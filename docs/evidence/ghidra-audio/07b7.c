
void audio_07b7(byte *param_1)

{
  byte bVar1;
  byte bVar2;
  undefined2 *puVar3;
  
  bVar1 = param_1[1] - 1;
  param_1[1] = bVar1;
  if (bVar1 != 0) {
    return;
  }
  param_1[1] = DAT_ram_42a2;
  if (((*param_1 & 1) == 0) && (-1 < (char)(param_1[7] - 1))) {
    param_1[7] = param_1[7] - 1;
    audio_0018();
  }
  bVar1 = *param_1;
  *param_1 = bVar1 - 1;
  if ((byte)(bVar1 - 1) != 0) {
    return;
  }
  bVar2 = **(byte **)(param_1 + 2);
  bVar1 = bVar2 & 0x1f;
  if (bVar1 == 0) {
    audio_0872();
    audio_0018();
  }
  else {
    if (bVar1 == 0x1f) {
      *(byte **)(param_1 + 2) = *(byte **)(param_1 + 2) + 1;
      return;
    }
    audio_0872();
    bVar1 = (bVar2 & 0x1f) - 1;
    puVar3 = (undefined2 *)(*(short *)(param_1 + 4) + (ushort)(byte)(bVar1 * '\x02' | bVar1 >> 7));
    audio_0028(*puVar3,(short)puVar3 + 1);
    param_1[7] = param_1[6];
    audio_0018();
  }
  *(short *)(param_1 + 2) = *(short *)(param_1 + 2) + 1;
  return;
}

