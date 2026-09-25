
char writeScoreDigitStepUp(byte param_1,byte *param_2)

{
  char cVar1;
  
  *param_2 = param_1 & 0xf;
  cVar1 = (byte)param_2 - 0x20;
  if (0x1f < (byte)param_2) {
    return cVar1;
  }
  return cVar1;
}

