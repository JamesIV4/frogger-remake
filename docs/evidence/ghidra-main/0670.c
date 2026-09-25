
void fillAllHomeSlotsAndAwardLife(void)

{
  fillTwoByTwoTileBlock(&DAT_ram_ab64);
  fillTwoByTwoTileBlock(&DAT_ram_aaa4);
  fillTwoByTwoTileBlock(&DAT_ram_a9e4);
  fillTwoByTwoTileBlock(&DAT_ram_a924);
  fillTwoByTwoTileBlock(&DAT_ram_a864);
  DAT_ram_842f = 0;
  awardExtraLife();
  return;
}

