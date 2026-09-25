
void stampHomeGoalAndResetFrog(undefined1 *param_1,undefined2 param_2)

{
  undefined1 uVar1;
  char cVar2;
  char *pcVar3;
  undefined1 *puVar4;
  
  uVar1 = 0;
  if (DAT_ram_8134 != '\0') {
    param_2 = 0x20;
    addScoreAndAwardExtraLife(DAT_ram_8134);
    uVar1 = clearCollisionSpriteBlock();
  }
  *param_1 = 0x6c;
  param_1[1] = 0x6d;
  param_1[0x20] = 0x6e;
  param_1[0x21] = 0x6f;
  addScoreAndAwardExtraLife(uVar1,5,param_2,param_1 + 0x21);
  driveScoreDisplayCountdown();
  if (DAT_ram_83fe != '\0') {
    DAT_ram_8382 = 0;
    enqueueSoundCommand();
    enqueueSoundCommand(0xf0);
    pcVar3 = &DAT_ram_825c;
    if (DAT_ram_83fd != '\x01') {
      pcVar3 = &DAT_ram_825d;
    }
    if (*pcVar3 == '\x04') {
      DAT_ram_842f = *pcVar3;
      clearActivePlayerWorkRam();
      puVar4 = &DAT_ram_b040;
      cVar2 = '\x18';
      do {
        *puVar4 = 0;
        puVar4 = (undefined1 *)CONCAT11((char)((ushort)puVar4 >> 8),(char)puVar4 + '\x01');
        cVar2 = cVar2 + -1;
      } while (cVar2 != '\0');
      clearCollisionSpriteBlock();
    }
    else {
      enqueueSoundCommand(8);
      enqueueSoundCommand(0xe);
      DAT_ram_8381 = DAT_ram_8381 + -1;
      if (DAT_ram_8381 == '\0') {
        DAT_ram_8381 = '\x14';
      }
      DAT_ram_8382 = CONCAT11(*(undefined1 *)CONCAT11(0x2e,DAT_ram_8381 * '\x02' + -0x78),
                              *(undefined1 *)CONCAT11(0x2e,DAT_ram_8381 * '\x02' + -0x79));
    }
  }
  DAT_ram_826a = 0x20;
  enqueueSoundCommand(0x80);
  DAT_ram_8044 = 0;
  DAT_ram_8045 = 0;
  DAT_ram_8046 = 0;
  DAT_ram_8047 = 0xf0;
  DAT_ram_829b = 0;
  DAT_ram_83ea = 0;
  DAT_ram_824d = 0;
  DAT_ram_8249 = 0;
  DAT_ram_8251 = 0;
  DAT_ram_826c = 1;
  DAT_ram_83cd = 1;
  DAT_ram_8268 = 0x10;
  return;
}

