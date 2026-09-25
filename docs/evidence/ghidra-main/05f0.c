
void advanceBoardForeground(void)

{
  bool bVar1;
  
  enqueueSoundCommand(0x10);
  enqueueSoundCommand(0x30);
  if (DAT_ram_83fd == '\x01') {
    bVar1 = DAT_ram_8293 == '\x04';
    DAT_ram_8293 = DAT_ram_8293 + '\x01';
    if (bVar1) {
      DAT_ram_8293 = '\0';
    }
  }
  else {
    bVar1 = DAT_ram_8294 == '\x04';
    DAT_ram_8294 = DAT_ram_8294 + '\x01';
    if (bVar1) {
      DAT_ram_8294 = '\0';
    }
  }
  clearAndSeedScoreField();
  clearObjectBlocksAndMirrorToObjRam();
  loadActivePlayerLaneParams();
  seedObjectAnimationState();
  DAT_ram_8380 = 1;
  addScoreAndAwardExtraLife(0x100);
  return;
}

