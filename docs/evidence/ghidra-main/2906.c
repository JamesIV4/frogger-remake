
byte enqueueLaneScrollSyncedCommand(void)

{
  byte bVar1;
  
  if (DAT_ram_83fe == '\0') {
    return 0;
  }
  if (0xe < switchD_ram:14c6::caseD_8e) {
    return switchD_ram:14c6::caseD_8e;
  }
  if (switchD_ram:14c6::caseD_8e < 2) {
    return switchD_ram:14c6::caseD_8e;
  }
  if (DAT_ram_8140 != 0) {
    return DAT_ram_8140;
  }
  bVar1 = enqueueSoundCommand(0xd0);
  return bVar1;
}

