
void dispatch_0cc6(void)

{
  writePackedBcdByte(0x50,&UNK_ram_ab70);
  copyRunUpTileColumn(&UNK_ram_2fba);
  copyRunUpTileColumn(&UNK_ram_2f43);
  copyRunUpTileColumn(&UNK_ram_2fae);
  copyRunUpTileColumn(&UNK_ram_ab71,&UNK_ram_2f17);
  DAT_ram_83d8 = 0x80;
  return;
}

