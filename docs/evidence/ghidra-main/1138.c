
void renderFrogAnimArm8(void)

{
  switchD_ram:0fbd::caseD_5a = DAT_ram_8288;
  switchD_ram:0fbd::caseD_40 = 0x14ab;
  renderFrogAnimTileColumns
            (DAT_ram_828a,DAT_ram_13fd,&switchD_ram:14c6::caseD_a2,&switchD_ram:14c6::caseD_a2);
  return;
}

