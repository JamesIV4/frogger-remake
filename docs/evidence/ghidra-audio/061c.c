
byte audio_061c(byte param_1)

{
  byte bVar1;
  
  bVar1 = DAT_ram_4113;
  DAT_ram_4114 = DAT_ram_4114 + -1;
  if (DAT_ram_4114 != '\0') {
    return param_1;
  }
  DAT_ram_4114 = 0x68;
  if ((DAT_ram_4110 & 2) == 0) {
    DAT_ram_4113 = DAT_ram_4113 + 1;
    if (DAT_ram_4113 != '\a') {
      return 7;
    }
    DAT_ram_4110 = DAT_ram_4110 ^ 2;
    return DAT_ram_4110;
  }
  DAT_ram_4113 = DAT_ram_4113 - 1;
  if (bVar1 != 0) {
    return bVar1;
  }
  return 0xff;
}

