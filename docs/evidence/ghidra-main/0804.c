
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void activateFrogObject(void)

{
  DAT_ram_8044 = 1;
  DAT_ram_8045 = 0;
  DAT_ram_8047 = 0;
  if (DAT_ram_83fe != '\x02') {
    return;
  }
  DAT_ram_83d2 = 0x40;
  _DAT_ram_83da = 0x40;
  return;
}

