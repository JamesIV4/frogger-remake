
void advanceHomeBaySlotCursor(void)

{
  DAT_ram_8123 = DAT_ram_8123 + 1;
  if (DAT_ram_8123 < 6) {
    return;
  }
  DAT_ram_8123 = 0;
  return;
}

