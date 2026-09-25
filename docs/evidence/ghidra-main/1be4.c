
void animateFrogHop(short param_1,char *param_2)

{
  char cVar1;
  
  if (DAT_ram_8251 == '\0') {
    enqueueSoundCommand(4);
    if (*(char *)(param_1 + 1) == '\x1e') goto LAB_ram_1c07;
    DAT_ram_8045 = 0x1e;
  }
  DAT_ram_8251 = DAT_ram_8251 + '\x01';
  if (DAT_ram_8251 == '\0') {
    return;
  }
LAB_ram_1c07:
  DAT_ram_8251 = DAT_ram_8257;
  advanceHomeBaySlotCursor();
  if (DAT_ram_824d != '\0') {
    return;
  }
  DAT_ram_8249 = 1;
  cVar1 = DAT_ram_8251 + -1;
  if (cVar1 != '\0') {
    DAT_ram_8251 = cVar1;
    *param_2 = *param_2 - DAT_ram_8254;
    *(undefined1 *)(param_1 + 1) = 0x1c;
    return;
  }
  DAT_ram_824d = DAT_ram_8251;
  DAT_ram_8249 = cVar1;
  DAT_ram_8251 = cVar1;
  *(undefined1 *)(param_1 + 1) = 0x1e;
  scoreFrogRowProgress(param_2);
  return;
}

