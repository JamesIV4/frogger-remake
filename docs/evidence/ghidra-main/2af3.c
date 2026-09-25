
char placeSpriteObjectSlotAndRetire(char *param_1,undefined1 *param_2)

{
  char cVar1;
  char cVar2;
  short sVar3;
  undefined1 *puVar4;
  char *pcVar5;
  undefined1 *puVar6;
  
  if (param_2[6] == '\0') {
    return '\0';
  }
  raiseSpriteArmOneShotAndQueueSound(param_2[6]);
  if ((byte)param_2[4] < 0x60) {
    cVar2 = switchD_ram:14c6::caseD_40 - param_2[2];
    *param_1 = cVar2;
  }
  else {
    cVar2 = param_2[3];
    *param_1 = cVar2;
  }
  cVar1 = param_2[4];
  param_1[3] = cVar1;
  param_1[7] = cVar1;
  if (param_2[5] == '\0') {
    param_1[4] = cVar2 + '\x0f';
    if ((char)(cVar2 + '\x10') != '\0') {
      return cVar2 + '\x10';
    }
  }
  else {
    param_1[4] = cVar2 + -0xf;
    if (cVar2 != '\0') {
      return cVar2;
    }
  }
  cVar2 = param_2[7];
  if (param_2[7] != '\0') {
    puVar4 = (undefined1 *)CONCAT11((char)((ushort)param_2 >> 8),(char)param_2 + '\x01');
    sVar3 = 0xf;
    *param_2 = 0;
    puVar6 = param_2;
    do {
      *puVar4 = *puVar6;
      puVar4 = puVar4 + 1;
      puVar6 = puVar6 + 1;
      sVar3 = sVar3 + -1;
    } while (sVar3 != 0);
    sVar3 = 7;
    pcVar5 = (char *)CONCAT11((char)((ushort)param_1 >> 8),(char)param_1 + '\x01');
    *param_1 = '\0';
    do {
      *pcVar5 = *param_1;
      pcVar5 = pcVar5 + 1;
      param_1 = param_1 + 1;
      sVar3 = sVar3 + -1;
    } while (sVar3 != 0);
    param_2[10] = 0x20;
    return cVar2;
  }
  return cVar2;
}

