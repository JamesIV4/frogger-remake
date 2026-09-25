
void scoreFrogRowProgress(undefined2 param_1)

{
  if (DAT_ram_8047 < 0x30) {
    return;
  }
  if (DAT_ram_8047 == 0xd0) {
    if (DAT_ram_8269 == 0) {
      DAT_ram_8269 = 0xe0;
    }
  }
  else if (0xcf < DAT_ram_8047) {
    return;
  }
  if (DAT_ram_8269 < DAT_ram_8047) {
    return;
  }
  if (DAT_ram_8269 == DAT_ram_8047) {
    return;
  }
  DAT_ram_8269 = DAT_ram_8047;
  if (DAT_ram_8047 == 0x80) {
    return;
  }
  addScoreAndAwardExtraLife(1,param_1);
  return;
}

