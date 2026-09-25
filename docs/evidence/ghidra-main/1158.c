
void renderFrogAnimArm9(void)

{
  switchD_ram:0fbd::caseD_5a = DAT_ram_828b;
  switchD_ram:0fbd::caseD_40 = 0x14af;
  renderFrogAnimTileColumns
            (DAT_ram_828d,DAT_ram_13ff,&switchD_ram:0fbd::caseD_d5,&switchD_ram:0fbd::caseD_d5);
  return;
}

