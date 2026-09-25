
void clearAndSeedScoreField(void)

{
  char cVar1;
  
  clearActivePlayerWorkRam();
  DAT_ram_839a = 0;
  DAT_ram_839b = 0;
  DAT_ram_83cc = 1;
  cVar1 = ' ';
  do {
    fillTenCellRun(cVar1);
    cVar1 = fillTenCellRun();
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  return;
}

