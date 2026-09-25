
void renderFrogAnimArm6(void)

{
  switchD_ram:0fbd::caseD_5a = DAT_ram_8282;
  switchD_ram:0fbd::caseD_40 = 0x149f;
  renderFrogAnimTileColumns
            (DAT_ram_8284,DAT_ram_13f9,&switchD_ram:14c6::caseD_80,&switchD_ram:14c6::caseD_80);
  return;
}

