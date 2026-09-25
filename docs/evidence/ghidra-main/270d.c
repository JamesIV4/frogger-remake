
char FUN_ram_270d(void)

{
  if (DAT_ram_8135 != '\0') {
    return DAT_ram_8135;
  }
  DAT_ram_813d = DAT_ram_813d + '\x01';
  DAT_ram_8041 = 0x1e;
  DAT_ram_8042 = 4;
  DAT_ram_8043 = 0x60;
  DAT_ram_8135 = 1;
  DAT_ram_833d = 1;
  DAT_ram_833e = 0x3c;
  return '<';
}

