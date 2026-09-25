
char setUpPlayStartOnce(void)

{
  if ((char)(DAT_ram_83d6 + -1) != '\0') {
    return DAT_ram_83d6 + -1;
  }
  if (DAT_ram_829b != '\0') {
    return DAT_ram_829b;
  }
  DAT_ram_83b4 = DAT_ram_829b;
  initDisplayFieldOnce();
  clearAndSeedScoreField();
  loadActivePlayerLaneParams();
  switchD_ram:0fbd::caseD_1d = 0;
  renderFrogAndArmObjects();
  blitFourTileGroupColumn(&UNK_ram_a850);
  resetFrogObject();
  dispatchFrogAnimationArm();
  switchD_ram:0fbd::caseD_1d = 1;
  DAT_ram_829b = 1;
  return '\x01';
}

