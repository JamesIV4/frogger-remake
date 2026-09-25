
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void initInPlayBoardOnce(void)

{
  char cVar1;
  undefined *puVar2;
  
  clearActivePlayerWorkRam();
  if (DAT_ram_83ba != '\0') {
    return;
  }
  _DAT_ram_8293 = 0;
  _DAT_ram_81b3 = 0;
  switchD_ram:0fbd::caseD_1d = DAT_ram_83ba;
  DAT_ram_829a = DAT_ram_83ba;
  DAT_ram_83ba = 1;
  loadActivePlayerLaneParams();
  activateFrogObject();
  fillTilemapBlock28x32();
  clearObjectBlocksAndMirrorToObjRam();
  DAT_ram_801b = 4;
  DAT_ram_8029 = 6;
  cVar1 = (char)&UNK_ram_2f77;
  copyRunUpTileColumn(&UNK_ram_aa28,&UNK_ram_2f77);
  copyRunUpTileColumn(&UNK_ram_aaad,cVar1 + '\x01');
  blitPlayerSelectPrompt();
  copyRunUpTileColumn(&UNK_ram_ab74,&UNK_ram_2f88);
  copyRunUpTileColumn(&UNK_ram_2fa8);
  puVar2 = &UNK_ram_2fae;
  copyRunUpTileColumn(&UNK_ram_2fae);
  copyRunUpTileColumn(puVar2 + 1);
  writeScoreField(&UNK_ram_a994,DAT_ram_2e08);
  copyRunUpTileColumn(&UNK_ram_2fba);
  return;
}

