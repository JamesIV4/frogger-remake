
void dispatchFrogMoveAgainstLanes(void)

{
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

