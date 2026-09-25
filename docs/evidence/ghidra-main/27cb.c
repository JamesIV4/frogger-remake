
void armHomeGoalSprite(undefined2 param_1)

{
  DAT_ram_8040 = (char)((ushort)param_1 >> 8);
  DAT_ram_8041 = 0x19;
  DAT_ram_8042 = 3;
  DAT_ram_8043 = 0x10;
  DAT_ram_8340 = 0xa0;
  return;
}

