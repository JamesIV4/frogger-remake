
void mountOrKillFrogOnTwoPairFigure(void)

{
  byte bVar1;
  
  if ((DAT_ram_8150 & 1) == 0) {
    return;
  }
  if (DAT_ram_83b7 < 2) {
    return;
  }
  if ((byte)(DAT_ram_8047 + 8U) < 0x2a) {
    return;
  }
  if (0x3a < (byte)(DAT_ram_8047 + 8U)) {
    return;
  }
  bVar1 = DAT_ram_8044 + 8;
  if ((byte)(DAT_ram_8101 + 8U) < bVar1) {
    return;
  }
  if (bVar1 <= (byte)(DAT_ram_8101 - 0x20U)) {
    return;
  }
  if ((byte)(DAT_ram_8101 - 8U) < bVar1) {
    dispatchFrogMoveAgainstLanes();
    return;
  }
  DAT_ram_8004 = 1;
  DAT_ram_a846 = 0x68;
  DAT_ram_a847 = 0x69;
  DAT_ram_a866 = 0x6a;
  DAT_ram_a867 = 0x6b;
  return;
}

