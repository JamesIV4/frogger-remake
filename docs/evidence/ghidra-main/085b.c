
void blitEndStripAndSetHold(void)

{
  copyRunUpTileColumn(&UNK_ram_aa51,&UNK_ram_2f6e);
  copyRunUpTileColumn(&UNK_ram_2f12);
  DAT_ram_8004 = 1;
  return;
}

