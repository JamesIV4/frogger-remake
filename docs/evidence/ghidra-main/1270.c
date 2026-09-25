
byte FUN_ram_1270(byte param_1,byte *param_2)

{
  byte bVar1;
  byte bVar2;
  
  if (DAT_ram_8047 < 0x80) {
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
        if (0x7f < DAT_ram_8047) {
          bVar1 = dispatchFrogMoveAgainstLanes();
          return bVar1;
        }
        goto code_r0x12e4;
      }
      bVar2 = bVar2 - 1;
    } while (bVar2 != 0);
    if (0x7f < DAT_ram_8047) {
      bVar1 = resolveFrogMoveAgainstLanes();
      return bVar1;
    }
  }
  else {
    do {
      param_2 = param_2 + 1;
      if ((bVar1 <= *param_2) && (*param_2 < (byte)(bVar1 + param_1))) {
        if (0x7f < DAT_ram_8047) {
          bVar1 = dispatchFrogMoveAgainstLanes();
          return bVar1;
        }
code_r0x12e4:
        if (DAT_ram_8004 != 0) {
          return DAT_ram_8004;
        }
        bVar1 = DAT_ram_8047 + 0xf & 0xf;
        if (4 < bVar1) {
                    /* WARNING: Could not recover jumptable at 0x130a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          bVar1 = (**(code **)(&DAT_ram_130b + (ushort)((byte)(DAT_ram_8047 + 0xf) >> 4) * 2))();
          return bVar1;
        }
        return bVar1;
      }
      bVar2 = bVar2 - 1;
    } while (bVar2 != 0);
    if (0x7f < DAT_ram_8047) {
      bVar1 = resolveFrogMoveAgainstLanes();
      return bVar1;
    }
  }
  DAT_ram_8004 = 1;
  if (0x7f < DAT_ram_8047) {
    return DAT_ram_8047;
  }
  if (DAT_ram_8047 < 0x30) {
    return DAT_ram_8047;
  }
  DAT_ram_829c = 1;
  return 1;
}

