
void awardExtraLife(void)

{
  byte *pbVar1;
  
  DAT_ram_83cc = 0;
  pbVar1 = &DAT_ram_83b8;
  if (DAT_ram_83fd != '\x01') {
    pbVar1 = &DAT_ram_83b9;
  }
  *pbVar1 = *pbVar1 + 1;
  DAT_ram_83b7 = *pbVar1;
  if (0xf < DAT_ram_83b7) {
    return;
  }
  (&DAT_ram_a85e)[(ushort)(byte)(DAT_ram_83b7 * '\x10') * 2] = 0x4c;
  return;
}

