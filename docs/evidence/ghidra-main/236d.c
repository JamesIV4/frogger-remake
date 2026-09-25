
char driveAttractDemoFrogHop(void)

{
  if (DAT_ram_826c != '\0') {
    return DAT_ram_826c;
  }
  if (DAT_ram_8004 != '\0') {
    return DAT_ram_8004;
  }
  if (DAT_ram_8299 != '\0') {
    DAT_ram_8299 = DAT_ram_8299 + -1;
    return DAT_ram_8299;
  }
  DAT_ram_8299 = 0x30;
  DAT_ram_829a = DAT_ram_829a + 1;
  if ((&DAT_ram_2e68)[DAT_ram_829a] != -1) {
    return '0';
  }
  DAT_ram_829a = 0;
  DAT_ram_8299 = 0;
  switchD_ram:0fbd::caseD_1d = 0;
  return '\0';
}

