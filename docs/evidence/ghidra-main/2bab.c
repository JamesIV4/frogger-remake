
byte steerSpriteObjectTowardTarget(byte *param_1,char *param_2)

{
  char cVar1;
  byte bVar2;
  short sVar3;
  char *pcVar4;
  undefined1 *puVar5;
  undefined1 *puVar6;
  
  bVar2 = param_2[6];
  if (param_2[6] == 0) {
    return bVar2;
  }
  cVar1 = param_2[9] + -1;
  param_2[9] = cVar1;
  if (cVar1 != '\0') {
    return bVar2;
  }
  param_2[9] = '\b';
  if (param_2[5] == '\0') {
    bVar2 = *(char *)CONCAT11(0x80,param_2[0xb]) - param_2[1];
    if (*param_1 <= bVar2) {
      param_2[2] = param_2[2] + -1;
      return bVar2;
    }
  }
  else {
    bVar2 = *(char *)CONCAT11(0x80,param_2[0xb]) - *param_2;
    if (bVar2 < *param_1) {
      param_2[2] = param_2[2] + '\x01';
      return bVar2;
    }
  }
  bVar2 = DAT_ram_8004;
  if (DAT_ram_8004 != 0) {
    return bVar2;
  }
  pcVar4 = (char *)CONCAT11((char)((ushort)param_2 >> 8),(char)param_2 + '\x01');
  sVar3 = 0xf;
  *param_2 = '\0';
  do {
    *pcVar4 = *param_2;
    pcVar4 = pcVar4 + 1;
    param_2 = param_2 + 1;
    sVar3 = sVar3 + -1;
  } while (sVar3 != 0);
  puVar6 = &DAT_ram_8058;
  puVar5 = &DAT_ram_8059;
  sVar3 = 3;
  DAT_ram_8058 = 0;
  do {
    *puVar5 = *puVar6;
    puVar5 = puVar5 + 1;
    puVar6 = puVar6 + 1;
    sVar3 = sVar3 + -1;
  } while (sVar3 != 0);
  return bVar2;
}

