
void blitPlayerSelectPrompt(void)

{
  undefined1 *puVar1;
  
  if (DAT_ram_83e1 != '\x01') {
    DAT_ram_8023 = 3;
    puVar1 = &DAT_ram_ab11;
    copyRunUpTileColumn(&UNK_ram_2f88);
    copyRunUpTileColumn();
    *puVar1 = 0x23;
    return;
  }
  copyRunUpTileColumn(0,&UNK_ram_aaf1,&UNK_ram_2f88);
  copyRunUpTileColumn(&UNK_ram_2f93);
  return;
}

