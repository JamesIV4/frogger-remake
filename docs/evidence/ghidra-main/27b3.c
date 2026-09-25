
void clearLatchedCollision(void)

{
  if (DAT_ram_8135 == '\0') {
    return;
  }
  DAT_ram_8134 = 0;
  DAT_ram_8040 = 0;
  DAT_ram_8041 = 0;
  DAT_ram_8042 = 0;
  DAT_ram_8043 = 0;
  DAT_ram_8135 = 0;
  return;
}

