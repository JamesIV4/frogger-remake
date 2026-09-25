
void clearTwoPlayerFrameCells(void)

{
  if (DAT_ram_83fe != '\x02') {
    return;
  }
  DAT_ram_814f = 0;
  DAT_ram_814e = 0;
  DAT_ram_8145 = 0;
  DAT_ram_8146 = 0;
  DAT_ram_8147 = 0;
  return;
}

