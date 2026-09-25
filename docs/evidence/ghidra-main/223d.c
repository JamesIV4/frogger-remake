
void loadActivePlayerLaneParams(void)

{
  short sVar1;
  undefined1 *puVar2;
  char *pcVar3;
  undefined1 *puVar4;
  
  pcVar3 = &DAT_ram_8293;
  if (DAT_ram_83fd != '\x01') {
    pcVar3 = &DAT_ram_8294;
  }
  puVar4 = *(undefined1 **)(&DAT_ram_2260 + (byte)(*pcVar3 * '\x02'));
  puVar2 = &DAT_ram_8270;
  sVar1 = 0x21;
  do {
    *puVar2 = *puVar4;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  return;
}

