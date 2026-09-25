
byte moveSpriteObjectArmA(byte *param_1,byte *param_2)

{
  byte bVar1;
  byte bVar2;
  char cVar3;
  
  if (param_2[6] == 0) {
    return 0;
  }
  bVar2 = DAT_ram_842c;
  if (DAT_ram_842c != 0) {
    return bVar2;
  }
  bVar1 = param_2[9] - 1;
  param_2[9] = bVar1;
  if (bVar1 != 0) {
    return bVar2;
  }
  param_2[9] = 8;
  if (param_1[3] < 0x60) {
    param_2[7] = 1;
    if (param_2[5] == 0) {
      bVar2 = switchD_ram:14c6::caseD_40 - *param_2;
      if (switchD_ram:14c6::caseD_40 < *param_2) {
        return bVar2;
      }
      if (bVar2 < *param_1) {
        param_2[2] = param_2[2] + 1;
        return bVar2;
      }
    }
    else {
      bVar2 = switchD_ram:14c6::caseD_40 - param_2[1];
      if (*param_1 <= bVar2) {
        param_2[2] = param_2[2] - 1;
        return bVar2;
      }
    }
    param_2[5] = param_2[5] ^ 0x80;
    *param_1 = param_1[4];
    bVar2 = param_1[1];
    param_1[1] = bVar2 ^ 0x80;
    return bVar2 ^ 0x80;
  }
  param_2[7] = 1;
  if (param_2[5] == 0) {
    cVar3 = -2;
  }
  else {
    cVar3 = '\x02';
  }
  bVar2 = param_2[3];
  param_2[3] = cVar3 + bVar2;
  return cVar3 + bVar2;
}

