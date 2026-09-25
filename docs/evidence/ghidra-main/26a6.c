
byte animateFlyEatCollision(void)

{
  if (DAT_ram_8134 == '\0') {
    if (DAT_ram_811c == '\0') {
      FUN_ram_270d(0);
    }
    if ((DAT_ram_813d & 1) != 0) {
      if (DAT_ram_8135 == '\0') {
        return 0;
      }
      DAT_ram_8134 = 0;
      DAT_ram_8040 = 0;
      DAT_ram_8041 = 0;
      DAT_ram_8042 = 0;
      DAT_ram_8043 = 0;
      DAT_ram_8135 = 0;
      return 0;
    }
    if (DAT_ram_8135 == '\0') {
      return 0;
    }
    if (DAT_ram_8134 == '\0') {
      driveFlyPatrol(0);
      if (DAT_ram_8047 < 0x5a) {
        return DAT_ram_8047;
      }
      if (0x67 < DAT_ram_8047) {
        return DAT_ram_8047;
      }
      if ((byte)(DAT_ram_8044 + 4U) < DAT_ram_8040) {
        return DAT_ram_8044 + 4U;
      }
      if (DAT_ram_8040 <= (byte)(DAT_ram_8044 - 4U)) {
        return DAT_ram_8044 - 4U;
      }
      DAT_ram_8134 = '\x01';
      enqueueSoundCommand(0x18);
    }
  }
  DAT_ram_8040 = DAT_ram_8044;
  DAT_ram_8041 = DAT_ram_8045;
  DAT_ram_8043 = DAT_ram_8047 + 2;
  return DAT_ram_8047 + 2;
}

