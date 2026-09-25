
void dispatch_0e35(void)

{
  char cVar1;
  undefined1 *puVar2;
  
  UNK_ram_a986 = 0xf4;
  UNK_ram_a987 = 0xf5;
  UNK_ram_a9a6 = 0xf6;
  UNK_ram_a9a7 = 0xf7;
  puVar2 = &DAT_ram_8050;
  cVar1 = '\x04';
  do {
    *puVar2 = 0;
    puVar2 = (undefined1 *)CONCAT11((char)((ushort)puVar2 >> 8),(char)puVar2 + '\x01');
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  DAT_ram_83d7 = DAT_ram_83d7 + -1;
  if (DAT_ram_83d7 != '\0') {
    return;
  }
  DAT_ram_83d7 = 7;
  DAT_ram_83bf = 0;
  DAT_ram_83bb = 0;
  DAT_ram_83d6 = 5;
  return;
}

