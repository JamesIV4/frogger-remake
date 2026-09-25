
void dispatch_0ceb(void)

{
  writePackedBcdWord(&UNK_ram_ab73,0x1000);
  copyRunUpTileColumn(&UNK_ram_2fba);
  copyRunUpTileColumn(&UNK_ram_2f39);
  copyRunUpTileColumn(&UNK_ram_2fae);
  copyRunUpTileColumn(&UNK_ram_ab74,&UNK_ram_2f2a);
  DAT_ram_83d8 = 0x80;
  return;
}

