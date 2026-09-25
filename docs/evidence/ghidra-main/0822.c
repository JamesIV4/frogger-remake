
byte handOffToOtherPlayer(void)

{
  undefined1 *puVar1;
  
  DAT_ram_8371 = 0;
  if (DAT_ram_83fe == '\x01') {
    return 0;
  }
  DAT_ram_83fd = DAT_ram_83fd ^ 3;
  puVar1 = &DAT_ram_83b8;
  if (DAT_ram_83fd != 1) {
    puVar1 = &DAT_ram_83b9;
  }
  DAT_ram_83b7 = *puVar1;
  DAT_ram_83b6 = 0;
  DAT_ram_825a = 1;
  if (DAT_ram_83c2 == '\0') {
    return 0;
  }
  DAT_ram_83cb = DAT_ram_83cb ^ 1;
  DAT_ram_b810 = DAT_ram_83cb;
  DAT_ram_b80c = DAT_ram_83cb;
  return DAT_ram_83cb;
}

