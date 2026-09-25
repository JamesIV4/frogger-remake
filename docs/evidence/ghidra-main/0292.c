
byte tickFrogRespawnDelay(void)

{
  byte bVar1;
  
  if ((char)((ushort)DAT_ram_829d >> 8) == '\0' && (char)DAT_ram_829d == '\0') {
    return 0;
  }
  DAT_ram_829d = DAT_ram_829d + -1;
  bVar1 = (byte)((ushort)DAT_ram_829d >> 8) | (byte)DAT_ram_829d;
  if (bVar1 != 0) {
    return bVar1;
  }
  DAT_ram_83ae = bVar1;
  return bVar1;
}

