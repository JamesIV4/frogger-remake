
void armBoardCompleteReveal(void)

{
  DAT_ram_826d = 1;
  DAT_ram_825a = 1;
  DAT_ram_83cd = 1;
  switchD_ram:0fbd::caseD_1d = 0;
  DAT_ram_83ea = 0;
  DAT_ram_8297 = 0xff;
  DAT_ram_8298 = 0x40;
  return;
}

