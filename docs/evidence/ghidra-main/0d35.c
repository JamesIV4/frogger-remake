
void dispatch_0d35(void)

{
  DAT_ram_83d8 = 0x30;
  DAT_ram_83d7 = 0;
  DAT_ram_8015 = 0;
  copyRunUpTileColumn(&UNK_ram_aaca,&UNK_ram_2f01);
  blitMode3FinalStrip();
  return;
}

