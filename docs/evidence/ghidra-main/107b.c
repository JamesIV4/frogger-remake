
void renderFrogAnimArm2(void)

{
  switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_5f;
  switchD_ram:0fbd::caseD_40 = &UNK_ram_143b;
  renderFrogAnimTileColumns
            (DAT_ram_8278,uRam13f1,&switchD_ram:14c6::caseD_3c,&switchD_ram:14c6::caseD_3c);
  return;
}

