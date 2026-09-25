
void animateFrogHop(char *param_1)

{
  char cVar1;
  
  if (DAT_ram_824e != '\0') {
    return;
  }
  DAT_ram_824a = 1;
  cVar1 = DAT_ram_8252 + -1;
  if (cVar1 == '\0') {
    DAT_ram_824e = DAT_ram_8252;
    DAT_ram_824a = cVar1;
    DAT_ram_8252 = cVar1;
    param_1[1] = -0x5f;
    return;
  }
  DAT_ram_8252 = cVar1;
  *param_1 = *param_1 + DAT_ram_8255;
  param_1[1] = -0x61;
  return;
}

