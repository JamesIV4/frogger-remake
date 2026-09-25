
void nextSpawnRandomByte(void)

{
  byte bVar1;
  
  DAT_ram_8400 = DAT_ram_8400 + -1;
  if (DAT_ram_8400 == '\0') {
    DAT_ram_8400 = '\x1f';
  }
  bVar1 = DAT_ram_8400 + 0xd;
  if (0x1f < bVar1) {
    bVar1 = DAT_ram_8400 - 0x12;
  }
  *(byte *)CONCAT11(0x84,bVar1) =
       *(byte *)CONCAT11(0x84,DAT_ram_8400) ^ *(byte *)CONCAT11(0x84,bVar1);
  return;
}

