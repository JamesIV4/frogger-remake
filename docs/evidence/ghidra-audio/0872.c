
void audio_0872(undefined2 param_1,byte *param_2)

{
  byte bVar1;
  byte bVar2;
  
  bVar1 = (byte)((ushort)param_1 >> 8);
  bVar1 = ((bVar1 >> 7) << 1 | (bVar1 & 0x60) >> 6) << 1 | (bVar1 & 0x20) >> 5;
  bVar2 = 1;
  while (bVar1 = bVar1 - 1, bVar1 != 0) {
    bVar2 = bVar2 << 1 | bVar2 >> 7;
  }
  *param_2 = bVar2;
  return;
}

