
void stampHomeBayFrogByColumn(char param_1)

{
  undefined1 *puVar1;
  
  if (param_1 == -0x40) {
    puVar1 = &DAT_ram_ab64;
  }
  else if (param_1 == -0x70) {
    puVar1 = &DAT_ram_aaa4;
  }
  else if (param_1 == 'p') {
    puVar1 = &DAT_ram_a9e4;
  }
  else if (param_1 == 'P') {
    puVar1 = &DAT_ram_a924;
  }
  else {
    if (param_1 != '0') {
      if (param_1 != '\x10') {
        return;
      }
      fillTwoByTwoTileBlock(&DAT_ram_ab64);
      fillTwoByTwoTileBlock(&DAT_ram_aaa4);
      fillTwoByTwoTileBlock(&DAT_ram_a9e4);
      fillTwoByTwoTileBlock(&DAT_ram_a924);
      fillTwoByTwoTileBlock(&DAT_ram_a864);
      DAT_ram_842f = 0;
      awardExtraLife();
      return;
    }
    puVar1 = &DAT_ram_a864;
  }
  *puVar1 = 0xfc;
  puVar1[1] = 0xfd;
  puVar1[0x20] = 0xfe;
  puVar1[0x21] = 0xff;
  return;
}

