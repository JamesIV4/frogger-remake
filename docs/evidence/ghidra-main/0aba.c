
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void initDisplayFieldOnce(void)

{
  char cVar1;
  undefined1 *puVar2;
  
  if (DAT_ram_842d != '\0') {
    return;
  }
  DAT_ram_842d = 1;
  DAT_ram_803f = 3;
  DAT_ram_83e0 = 0;
  copyRunUpTileColumn(&UNK_ram_a8bf,&UNK_ram_2f6e);
  puVar2 = &DAT_ram_a8df;
  cVar1 = '\x0f';
  do {
    *puVar2 = 0xc;
    puVar2 = puVar2 + 0x20;
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  _DAT_ram_83dc = &UNK_ram_3c20;
  DAT_ram_83de = 0x60;
  return;
}

