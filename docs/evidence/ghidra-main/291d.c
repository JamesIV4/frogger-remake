
byte animateTwoPairFigure(void)

{
  if (DAT_ram_8101 == '\0') {
    DAT_ram_833f = 0;
    return 0;
  }
  if ((DAT_ram_8150 & 1) == 0) {
    return DAT_ram_8150;
  }
  if (DAT_ram_814f != 0) {
    return DAT_ram_814f;
  }
  DAT_ram_833f = DAT_ram_833f + 1;
  if (DAT_ram_833f != 0x40) {
    if (DAT_ram_833f != 0x70) {
      return DAT_ram_833f;
    }
    DAT_ram_a846 = 0xd0;
    DAT_ram_a847 = 0xd1;
    DAT_ram_a866 = 0xd2;
    DAT_ram_a867 = 0xd3;
    DAT_ram_833f = 0;
    return 0;
  }
  DAT_ram_a846 = 0x68;
  DAT_ram_a847 = 0x69;
  DAT_ram_a866 = 0x6a;
  DAT_ram_a867 = 0x6b;
  return 0x40;
}

