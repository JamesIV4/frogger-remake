
void animateFrogHop(short param_1,char *param_2)

{
  char cVar1;
  
  if (DAT_ram_824c != '\0') {
    return;
  }
  DAT_ram_8248 = 1;
  cVar1 = DAT_ram_8250 + -1;
  if (cVar1 == '\0') {
    DAT_ram_824c = DAT_ram_8250;
    DAT_ram_8248 = cVar1;
    DAT_ram_8250 = cVar1;
    *(undefined1 *)(param_1 + 1) = 0xde;
    return;
  }
  DAT_ram_8250 = cVar1;
  *param_2 = DAT_ram_8254 + *param_2;
  *(undefined1 *)(param_1 + 1) = 0xdc;
  return;
}

