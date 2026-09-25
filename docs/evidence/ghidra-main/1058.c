
void renderFrogAnimArm1(void)

{
  blitFrogAnimColumnOnTrigger();
  switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
  switchD_ram:0fbd::caseD_40 = &UNK_ram_1423;
  renderFrogAnimTileColumns(DAT_ram_8275,switchD_ram:0fbd::caseD_53,&DAT_ram_8109,&DAT_ram_8109);
  return;
}

