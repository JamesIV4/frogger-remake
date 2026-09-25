
char writePackedBcdWord(byte *param_1,byte param_2)

{
  byte bVar1;
  byte bVar2;
  char cVar3;
  
  writePackedBcdByte();
  bVar1 = (byte)(param_2 >> 1 | param_2 << 7) >> 1;
  bVar2 = (byte)(bVar1 | (param_2 >> 1) << 7) >> 1;
  writeScoreDigitStepUp((byte)(bVar2 | bVar1 << 7) >> 1 | bVar2 << 7);
  *param_1 = param_2 & 0xf;
  cVar3 = (byte)param_1 - 0x20;
  if (0x1f < (byte)param_1) {
    return cVar3;
  }
  return cVar3;
}

