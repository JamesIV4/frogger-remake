
void clearPlayerOneHomeBayGates(void)

{
  short sVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  
  DAT_ram_825c = 0;
  puVar3 = &DAT_ram_825e;
  puVar2 = &DAT_ram_825f;
  sVar1 = 4;
  DAT_ram_825e = 0;
  do {
    *puVar2 = *puVar3;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  coldStartClearPlayRamAndSetMode();
  return;
}

