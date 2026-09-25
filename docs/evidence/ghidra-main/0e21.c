
void dispatch_0e21(void)

{
  char cVar1;
  undefined1 *puVar2;
  
  UNK_ram_aa46 = 0xdc;
  UNK_ram_aa47 = 0xdd;
  UNK_ram_aa66 = 0xde;
  UNK_ram_aa67 = 0xdf;
  puVar2 = &DAT_ram_8048;
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

