
char writePackedBcdByte(byte param_1,byte *param_2)

{
  byte bVar1;
  byte bVar2;
  char cVar3;
  
  bVar1 = (byte)(param_1 >> 1 | param_1 << 7) >> 1;
  bVar2 = (byte)(bVar1 | (param_1 >> 1) << 7) >> 1;
  writeScoreDigitStepUp((byte)(bVar2 | bVar1 << 7) >> 1 | bVar2 << 7);
  *param_2 = param_1 & 0xf;
  cVar3 = (byte)param_2 - 0x20;
  if (0x1f < (byte)param_2) {
    return cVar3;
  }
  return cVar3;
}

