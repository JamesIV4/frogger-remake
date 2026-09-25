
void renderFrogAnimArm4(void)

{
  switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_7f;
  switchD_ram:0fbd::caseD_40 = &UNK_ram_145f;
  renderFrogAnimTileColumns
            (DAT_ram_827e,switchD_ram:0fbd::caseD_83,&switchD_ram:14c6::caseD_5e,
             &switchD_ram:14c6::caseD_5e);
  return;
}

