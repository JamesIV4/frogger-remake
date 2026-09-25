
void renderMode2IntroScreen(void)

{
  DAT_ram_83d8 = 0xff;
  fillTilemapBlock28x32();
  DAT_ram_829b = 0;
  DAT_ram_8021 = 0;
  DAT_ram_801b = 5;
  DAT_ram_802b = 3;
  copyRunUpTileColumn(&UNK_ram_aa8d,&UNK_ram_2f5c);
  if (9 < DAT_ram_83e4) {
    return;
  }
  writeScoreDigitStepUp(&UNK_ram_ab15);
  copyRunUpTileColumn(&UNK_ram_2fae);
  copyRunUpTileColumn(&UNK_ram_2f73);
  copyRunUpTileColumn(&UNK_ram_2f92);
  return;
}

