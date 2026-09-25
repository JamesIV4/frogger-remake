
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void renderFrogAnimArm10(void)

{
  switchD_ram:0fbd::caseD_5a = DAT_ram_828e;
  switchD_ram:0fbd::caseD_40 = 0x14b3;
  renderFrogAnimTileColumns
            (DAT_ram_8290,_UNK_ram_1401,&switchD_ram:14c6::caseD_c4,&switchD_ram:14c6::caseD_c4);
  return;
}

