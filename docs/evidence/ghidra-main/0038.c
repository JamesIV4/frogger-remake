
void clearTilemapToTile16(void)

{
  char cVar1;
  char cVar2;
  char cVar3;
  undefined1 *puVar4;
  
  cVar3 = ' ';
  puVar4 = &DAT_ram_a800;
  do {
    cVar1 = ' ';
    do {
      *puVar4 = 0x10;
      puVar4 = puVar4 + 1;
      cVar1 = cVar1 + -1;
    } while (cVar1 != '\0');
    cVar1 = '\x15';
    cVar2 = '\0';
    do {
      do {
        cVar2 = cVar2 + -1;
      } while (cVar2 != '\0');
      cVar1 = cVar1 + -1;
    } while (cVar1 != '\0');
    cVar3 = cVar3 + -1;
  } while (cVar3 != '\0');
  return;
}

