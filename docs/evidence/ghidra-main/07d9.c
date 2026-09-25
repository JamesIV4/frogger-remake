
void clearSoundQueue(void)

{
  short sVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  
  puVar3 = &DAT_ram_8300;
  puVar2 = &DAT_ram_8301;
  sVar1 = 0x2f;
  DAT_ram_8300 = 0;
  do {
    *puVar2 = *puVar3;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  return;
}

