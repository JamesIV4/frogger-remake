
undefined1 renderScoreHeader(void)

{
  undefined1 uVar1;
  
  copyRunUpTileColumn(&UNK_ram_aa60,&UNK_ram_2ee2);
  writeScoreField(&UNK_ram_aa41,DAT_ram_83ef);
  writeScoreDigitStepUp(1,&UNK_ram_ab20);
  copyRunUpTileColumn(&UNK_ram_2edf);
  writeScoreField(&UNK_ram_ab41,DAT_ram_83ed);
  if (DAT_ram_8370 == '\x01') {
    return 0;
  }
  writeScoreDigitStepUp(2,&UNK_ram_a900);
  copyRunUpTileColumn(&UNK_ram_2edf);
  uVar1 = writeScoreField(&switchD_ram:14c6::caseD_54,DAT_ram_83eb);
  return uVar1;
}

