
void fillTilemapBlock22x32(void)

{
  short sVar1;
  char cVar2;
  char cVar3;
  undefined1 *puVar4;
  
  puVar4 = &DAT_ram_a808;
  cVar3 = ' ';
  sVar1 = 10;
  do {
    sVar1 = CONCAT11(0x16,(char)sVar1);
    do {
      *puVar4 = 0x10;
      puVar4 = puVar4 + 1;
      cVar2 = (char)((ushort)sVar1 >> 8) + -1;
      sVar1 = CONCAT11(cVar2,(char)sVar1);
    } while (cVar2 != '\0');
    puVar4 = puVar4 + sVar1;
    cVar3 = cVar3 + -1;
  } while (cVar3 != '\0');
  return;
}

