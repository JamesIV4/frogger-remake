
byte spawnSpriteObject(char *param_1)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  char cVar5;
  byte *pbVar6;
  char *pcVar7;
  undefined1 uVar8;
  
  if (DAT_ram_83b7 < 3) {
    return DAT_ram_83b7;
  }
  if (param_1[6] != 0) {
    return param_1[6];
  }
  bVar2 = DAT_ram_83b7;
  bVar1 = nextSpawnRandomByte(0);
  bVar2 = bVar2 * '\b' + 0x80;
  if (bVar2 < bVar1) {
    return bVar2;
  }
  bVar1 = nextSpawnRandomByte(CONCAT11(bVar1,0x40));
  bVar2 = bVar1 & 7;
  if (4 < bVar2) {
    return bVar2;
  }
  bVar3 = bVar1 & 7;
  bVar1 = (byte)(bVar2 >> 1 | bVar1 << 7) >> 1;
  param_1[4] = ((byte)((byte)(bVar1 | (bVar2 >> 1) << 7) >> 1 | bVar1 << 7) >> 1) + 0x30;
  nextSpawnRandomByte();
  pbVar6 = &DAT_ram_2ce6 + (byte)(bVar3 * '\x02');
  bVar4 = *pbVar6;
  cVar5 = *(char *)CONCAT11((char)((ushort)pbVar6 >> 8),(char)pbVar6 + '\x01');
  param_1[0xb] = cVar5;
  bVar2 = *(byte *)CONCAT11(0x80,cVar5);
  pcVar7 = &DAT_ram_2cdc + (byte)(bVar3 * '\x02');
  uVar8 = *(undefined1 *)CONCAT11((char)((ushort)pcVar7 >> 8),(char)pcVar7 + '\x01');
  bVar3 = *(byte *)CONCAT11(uVar8,*pcVar7);
  bVar1 = bVar3 >> 1;
  bVar1 = ((byte)(bVar1 | bVar3 << 7) >> 1 | bVar1 << 7) - 0x10;
  cVar5 = *(char *)CONCAT11(uVar8,*pcVar7 + '\x02');
  do {
    bVar3 = bVar2 - bVar4;
    if (bVar2 < bVar4) {
      return bVar3;
    }
    bVar2 = bVar3 - bVar1;
  } while ((bVar1 <= bVar3) && (cVar5 = cVar5 + -1, cVar5 != '\0'));
  cVar5 = *(char *)CONCAT11(0x80,param_1[0xb]);
  param_1[2] = cVar5;
  cVar5 = cVar5 - (bVar2 + bVar1);
  param_1[1] = cVar5;
  *param_1 = cVar5 + bVar1;
  bVar2 = nextSpawnRandomByte();
  if ((bool)(bVar2 & 1)) {
    param_1[5] = '\0';
    param_1[3] = '\0';
  }
  else {
    param_1[5] = -0x80;
    param_1[3] = -0x10;
  }
  param_1[6] = '\x01';
  param_1[9] = '\b';
  return bVar2 >> 1 | bVar2 << 7;
}

