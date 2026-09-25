
void renderFrogAndArmObjects(void)

{
  char cVar1;
  char cVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  undefined1 *puVar5;
  
  puVar4 = &DAT_ram_a843;
  cVar2 = '\x05';
  do {
    puVar3 = &DAT_ram_19f6;
    cVar1 = '\x04';
    do {
      puVar5 = puVar4;
      *puVar5 = *puVar3;
      puVar3 = puVar3 + 1;
      cVar1 = cVar1 + -1;
      puVar4 = puVar5 + 0x20;
    } while (cVar1 != '\0');
    puVar4 = puVar5 + 0x60;
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  puVar4 = &DAT_ram_a8a4;
  cVar2 = '\x04';
  do {
    puVar3 = &DAT_ram_19fa;
    cVar1 = '\x04';
    do {
      puVar5 = puVar4;
      *puVar5 = *puVar3;
      puVar3 = puVar3 + 1;
      cVar1 = cVar1 + -1;
      puVar4 = puVar5 + 0x20;
    } while (cVar1 != '\0');
    puVar4 = puVar5 + 0x60;
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  puVar4 = &DAT_ram_a8a5;
  cVar2 = '\x04';
  do {
    puVar3 = &DAT_ram_19fe;
    cVar1 = '\x04';
    do {
      puVar5 = puVar4;
      *puVar5 = *puVar3;
      puVar3 = puVar3 + 1;
      cVar1 = cVar1 + -1;
      puVar4 = puVar5 + 0x20;
    } while (cVar1 != '\0');
    puVar4 = puVar5 + 0x60;
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  puVar4 = &DAT_ram_a8c3;
  cVar2 = '\x04';
  do {
    *puVar4 = 0x47;
    puVar4[0x20] = 0x47;
    puVar4 = puVar4 + 0xc0;
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  DAT_ram_a844 = 0x41;
  DAT_ram_a845 = 0x42;
  DAT_ram_aba4 = 0x45;
  DAT_ram_aba5 = 0x46;
  blitFourTileGroupColumn(&UNK_ram_a85c);
  DAT_ram_8007 = 1;
  DAT_ram_8009 = 1;
  DAT_ram_800b = 1;
  seedObjectAnimationState();
  return;
}

