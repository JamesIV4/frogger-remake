
byte driveDiveAnimByLevel(void)

{
  byte bVar1;
  ushort uVar2;
  ushort uVar3;
  
  if (DAT_ram_83b7 < 2) {
    return DAT_ram_83b7;
  }
  if (4 < DAT_ram_83b7) {
    if (DAT_ram_8101 == '\0') {
      armTwoPairFigureFrame(0);
    }
    bVar1 = stepDiveSurfaceTimer();
    return bVar1;
  }
  if (DAT_ram_8101 == '\0') {
    resetDiveSurfaceCounter(0);
  }
  if (DAT_ram_814f == '\0') {
    return 0;
  }
  if (DAT_ram_8146 != DAT_ram_8147) {
    bVar1 = DAT_ram_8147;
    if (DAT_ram_8147 != 0) {
      DAT_ram_8147 = DAT_ram_8147 - 1;
      return bVar1;
    }
    DAT_ram_8147 = DAT_ram_8146;
    return DAT_ram_8146;
  }
  DAT_ram_8147 = DAT_ram_8147 - 1;
  if ((DAT_ram_8150 & 1) != 0) {
    uVar2 = (ushort)DAT_ram_814e;
    DAT_ram_814e = DAT_ram_814e + 2;
    uVar3 = (ushort)DAT_ram_8145;
    DAT_ram_8145 = DAT_ram_8145 + 0x20;
    (&DAT_ram_a806)[uVar3] = (&DAT_ram_1413)[uVar2];
    (&DAT_ram_a807)[uVar3] = (&DAT_ram_1414)[uVar2];
    if (DAT_ram_814e < 0x10) {
      return DAT_ram_814e;
    }
    DAT_ram_814f = 0;
    DAT_ram_814e = 0;
    DAT_ram_8145 = 0;
    DAT_ram_8146 = 0;
    DAT_ram_8147 = 0;
    return 0;
  }
  bVar1 = copyDiveAnimFrame(&switchD_ram:0fbd::caseD_11);
  return bVar1;
}

