
void spawnSpriteObjectArmA(char *param_1)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  char cVar5;
  
  bVar4 = DAT_ram_83b7;
  if (DAT_ram_83b7 < 3) {
    return;
  }
  cVar5 = param_1[10] + -1;
  param_1[10] = cVar5;
  if (cVar5 != '\0') {
    return;
  }
  if (param_1[6] != '\0') {
    return;
  }
  bVar2 = nextSpawnRandomByte(0);
  if ((byte)(bVar4 * '\b' + 0x80) < bVar2) {
    return;
  }
  bVar3 = nextSpawnRandomByte();
  bVar2 = switchD_ram:14c6::caseD_40;
  bVar4 = 0;
  if ((bVar3 & 3) != 0) {
    bVar3 = ((byte)(switchD_ram:0fbd::caseD_5f >> 1 | switchD_ram:0fbd::caseD_5f << 7) >> 1 |
            (switchD_ram:0fbd::caseD_5f >> 1) << 7) + 0x24;
    bVar4 = switchD_ram:14c6::caseD_40 - 0x10;
    cVar5 = DAT_ram_8278;
    if (0xf < switchD_ram:14c6::caseD_40) {
      do {
        bVar1 = bVar4 - 0x40;
        if (bVar4 < 0x40) {
          param_1[2] = switchD_ram:14c6::caseD_40;
          param_1[1] = bVar2 - bVar4;
          *param_1 = (bVar2 - bVar4) + '@';
          param_1[4] = 'N';
          goto LAB_ram_2ad2;
        }
        bVar4 = bVar1 - bVar3;
      } while ((bVar3 <= bVar1) && (cVar5 = cVar5 + -1, cVar5 != '\0'));
    }
  }
  param_1[4] = '~';
  bVar4 = nextSpawnRandomByte(bVar4);
  if ((bool)(bVar4 & 1)) {
LAB_ram_2ad2:
    param_1[5] = -0x80;
    param_1[3] = '\0';
  }
  else {
    param_1[5] = '\0';
    param_1[3] = -0x10;
  }
  param_1[6] = '\x01';
  param_1[8] = '\v';
  param_1[9] = '\b';
  if (DAT_ram_8371 != '\0') {
    return;
  }
  DAT_ram_8371 = 1;
  enqueueSoundCommand(0x90);
  return;
}

