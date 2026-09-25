
void animateFrogHop(short param_1,char *param_2)

{
  char cVar1;
  
  advanceHomeBaySlotCursor();
  if (DAT_ram_824d != '\0') {
    return;
  }
  DAT_ram_8249 = 1;
  cVar1 = DAT_ram_8251 + -1;
  if (cVar1 == '\0') {
    DAT_ram_824d = DAT_ram_8251;
    DAT_ram_8249 = cVar1;
    DAT_ram_8251 = cVar1;
    *(undefined1 *)(param_1 + 1) = 0x1e;
    scoreFrogRowProgress(param_2);
    return;
  }
  DAT_ram_8251 = cVar1;
  *param_2 = *param_2 - DAT_ram_8254;
  *(undefined1 *)(param_1 + 1) = 0x1c;
  return;
}

