
void forceClearPlayerWorkRam(void)

{
  short sVar1;
  undefined1 *puVar2;
  undefined *puVar3;
  undefined1 *puVar4;
  undefined *puVar5;
  
  puVar4 = &DAT_ram_8044;
  puVar2 = &DAT_ram_8045;
  sVar1 = 0x1f;
  DAT_ram_8044 = 0;
  do {
    *puVar2 = *puVar4;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  puVar5 = &UNK_ram_8420;
  puVar3 = &UNK_ram_8421;
  sVar1 = 0xb;
  UNK_ram_8420 = 0;
  do {
    *puVar3 = *puVar5;
    puVar3 = puVar3 + 1;
    puVar5 = puVar5 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  return;
}

