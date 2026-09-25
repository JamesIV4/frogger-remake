
void dispatch_0c8a(void)

{
  DAT_ram_801d = 6;
  DAT_ram_8023 = 6;
  DAT_ram_8029 = 6;
  DAT_ram_802f = 6;
  DAT_ram_801b = 3;
  DAT_ram_8021 = 3;
  DAT_ram_8027 = 3;
  DAT_ram_802d = 3;
  writePackedBcdByte(0x10,&UNK_ram_ab6d);
  copyRunUpTileColumn(&UNK_ram_2fba);
  copyRunUpTileColumn(&UNK_ram_2ed1);
  DAT_ram_83d8 = 0x80;
  return;
}

