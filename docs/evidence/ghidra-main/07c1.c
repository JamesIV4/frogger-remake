
undefined1 raiseActivePlayerStartFlag(void)

{
  if (DAT_ram_83fd != '\x01') {
    switchD_ram:0fbd::caseD_1d = 1;
    return 1;
  }
  if (DAT_ram_826d == '\0') {
    return 0;
  }
  switchD_ram:0fbd::caseD_1d = 1;
  return 1;
}

