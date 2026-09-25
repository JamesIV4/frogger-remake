
char driveInPlayFrameUpdate(void)

{
  char cVar1;
  
  if ((char)(DAT_ram_83d6 + -1) != '\0') {
    return DAT_ram_83d6 + -1;
  }
  if (DAT_ram_829b == '\0') {
    return '\0';
  }
  driveAttractDemoFrogHop(DAT_ram_829b);
  orchestrateCollisionsAndFrogInput();
  advanceAttractDemoFrogHop();
  renderFrogSceneAndTickTimer();
  driveScoreDisplayCountdown();
  advanceScrollLaneObjects();
  advanceAnimationFrameBuffer();
  dispatchFrogMoveAgainstLanes();
  driveFrogDeathAnimation();
  tickGatedCountdown();
  cVar1 = moveLaneObjectsAndCarryFrog();
  return cVar1;
}

