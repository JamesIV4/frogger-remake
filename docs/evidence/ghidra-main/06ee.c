
void swapInActivePlayerPages(void)

{
  short sVar1;
  undefined1 *puVar2;
  undefined *puVar3;
  undefined1 *puVar4;
  
  if (DAT_ram_83fd == '\x01') {
    puVar4 = &switchD_ram:14c6::caseD_1e;
    puVar2 = &DAT_ram_85c0;
    sVar1 = 0x2b;
    do {
      *puVar2 = *puVar4;
      puVar2 = puVar2 + 1;
      puVar4 = puVar4 + 1;
      sVar1 = sVar1 + -1;
    } while (sVar1 != 0);
    puVar2 = &DAT_ram_80ff;
    puVar3 = &UNK_ram_8600;
    sVar1 = 0xb7;
    do {
      *puVar3 = *puVar2;
      puVar3 = puVar3 + 1;
      puVar2 = puVar2 + 1;
      sVar1 = sVar1 + -1;
    } while (sVar1 != 0);
    puVar3 = &UNK_ram_86c0;
    puVar2 = &switchD_ram:14c6::caseD_1e;
    sVar1 = 0x2b;
    do {
      *puVar2 = *puVar3;
      puVar2 = puVar2 + 1;
      puVar3 = puVar3 + 1;
      sVar1 = sVar1 + -1;
    } while (sVar1 != 0);
    DAT_ram_803f = 1;
    puVar4 = &DAT_ram_8500;
    puVar2 = &DAT_ram_80ff;
    sVar1 = 0xb7;
    do {
      *puVar2 = *puVar4;
      puVar2 = puVar2 + 1;
      puVar4 = puVar4 + 1;
      sVar1 = sVar1 + -1;
    } while (sVar1 != 0);
    return;
  }
  puVar4 = &DAT_ram_80ff;
  puVar2 = &DAT_ram_8500;
  sVar1 = 0xb7;
  do {
    *puVar2 = *puVar4;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  puVar2 = &switchD_ram:14c6::caseD_1e;
  puVar3 = &UNK_ram_86c0;
  sVar1 = 0x2b;
  do {
    *puVar3 = *puVar2;
    puVar3 = puVar3 + 1;
    puVar2 = puVar2 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  puVar3 = &UNK_ram_8600;
  puVar2 = &DAT_ram_80ff;
  sVar1 = 0xb7;
  do {
    *puVar2 = *puVar3;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  puVar4 = &DAT_ram_85c0;
  puVar2 = &switchD_ram:14c6::caseD_1e;
  sVar1 = 0x2b;
  do {
    *puVar2 = *puVar4;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  DAT_ram_803f = 1;
  if (DAT_ram_8295 != '\0') {
    return;
  }
  switchD_ram:0fbd::caseD_1d = 0;
  DAT_ram_8295 = 1;
  return;
}

