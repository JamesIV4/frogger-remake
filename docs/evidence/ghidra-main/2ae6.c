
void raiseSpriteArmOneShotAndQueueSound(void)

{
  if (DAT_ram_8371 != '\0') {
    return;
  }
  DAT_ram_8371 = 1;
  enqueueSoundCommand(0x90);
  return;
}

