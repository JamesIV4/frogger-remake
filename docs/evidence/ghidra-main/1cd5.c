
void animateFrogHop(char *param_1)

{
  char cVar1;
  
  if (DAT_ram_824f != '\0') {
    return;
  }
  DAT_ram_824b = 1;
  cVar1 = DAT_ram_8253 + -1;
  if (cVar1 == '\0') {
    DAT_ram_824f = DAT_ram_8253;
    DAT_ram_824b = cVar1;
    DAT_ram_8253 = cVar1;
    param_1[1] = '!';
    return;
  }
  DAT_ram_8253 = cVar1;
  *param_1 = *param_1 - DAT_ram_8255;
  param_1[1] = '\x1f';
  return;
}

