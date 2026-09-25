
void FUN_ram_2673(undefined2 param_1)

{
  if (DAT_ram_8120 == '\0') {
    DAT_ram_805d = 0x19;
    DAT_ram_805e = 3;
    DAT_ram_805f = 0x20;
    DAT_ram_8340 = 0xa0;
    DAT_ram_805c = (char)((ushort)param_1 >> 8);
    addScoreAndAwardExtraLife(0x20);
    return;
  }
  DAT_ram_8004 = 1;
  return;
}

