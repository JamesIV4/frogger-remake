
void updateSpriteObject(void)

{
  spawnSpriteObject();
  steerSpriteObjectTowardTarget();
  writeSpriteObjectSlotX();
  flagSpriteObjectFrogHitAhead();
  writeSpriteObjectSlotAttr();
  return;
}

