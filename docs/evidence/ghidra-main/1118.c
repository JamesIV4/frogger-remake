
void renderFrogAnimArm7(void)

{
  switchD_ram:0fbd::caseD_5a = DAT_ram_8285;
  switchD_ram:0fbd::caseD_40 = 0x14a7;
  renderFrogAnimTileColumns
            (DAT_ram_8287,DAT_ram_13fb,&switchD_ram:0fbd::caseD_b5,&switchD_ram:0fbd::caseD_b5);
  return;
}

