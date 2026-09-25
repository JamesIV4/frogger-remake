
void FUN_ram_138f(byte param_1,byte *param_2)

{
  byte bVar1;
  byte bVar2;
  
  if (DAT_ram_802f < 0x80) {
    bVar1 = DAT_ram_8044 + 0xc;
  }
  else {
    bVar1 = DAT_ram_8044 + 3;
  }
  bVar2 = *param_2;
  if (CARRY1(bVar1,param_1)) {
    do {
      param_2 = param_2 + 1;
      if ((bVar1 <= *param_2) || (*param_2 < (byte)(bVar1 + param_1))) {
        if (DAT_ram_8047 < 0x80) {
          return;
        }
        DAT_ram_8004 = 1;
        return;
      }
      bVar2 = bVar2 - 1;
    } while (bVar2 != 0);
    if (0x7f < DAT_ram_8047) {
      return;
    }
  }
  else {
    do {
      param_2 = param_2 + 1;
      if ((bVar1 <= *param_2) && (*param_2 < (byte)(bVar1 + param_1))) {
        if (DAT_ram_8047 < 0x80) {
          return;
        }
        DAT_ram_8004 = 1;
        return;
      }
      bVar2 = bVar2 - 1;
    } while (bVar2 != 0);
    if (0x7f < DAT_ram_8047) {
      return;
    }
  }
  DAT_ram_8004 = 1;
  if (0x7f < DAT_ram_8047) {
    return;
  }
  if (DAT_ram_8047 < 0x30) {
    return;
  }
  DAT_ram_829c = 1;
  return;
}

