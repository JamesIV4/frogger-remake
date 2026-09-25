
void renderCreditLine(void)

{
  char cVar1;
  undefined1 *puVar2;
  
  if (DAT_ram_83b4 == '\0') {
    DAT_ram_83b4 = '\x01';
    puVar2 = &DAT_ram_a81f;
    cVar1 = ' ';
    do {
      *puVar2 = 0x10;
      puVar2 = puVar2 + 0x20;
      cVar1 = cVar1 + -1;
    } while (cVar1 != '\0');
  }
  copyRunUpTileColumn(&UNK_ram_a97f,&UNK_ram_2f68);
  DAT_ram_803f = 1;
  writePackedBcdByte(DAT_ram_83e1,&UNK_ram_a89f);
  return;
}

