
void renderLivesRow(void)

{
  byte bVar1;
  undefined1 *puVar2;
  
  puVar2 = &DAT_ram_a87e;
  bVar1 = DAT_ram_83b7;
  if (0xe < DAT_ram_83b7) {
    bVar1 = 0xf;
  }
  do {
    *puVar2 = 0x4c;
    puVar2 = puVar2 + 0x20;
    bVar1 = bVar1 - 1;
  } while (bVar1 != 0);
  return;
}

