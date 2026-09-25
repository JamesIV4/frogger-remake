
byte flagSpriteObjectFrogHitAhead(char *param_1,short param_2)

{
  byte bVar1;
  byte bVar2;
  
  if (*(char *)(param_2 + 6) == '\0') {
    return 0;
  }
  if (*(byte *)(param_2 + 4) != DAT_ram_8047) {
    return *(byte *)(param_2 + 4);
  }
  if (*(char *)(param_2 + 5) == '\0') {
    bVar2 = *param_1 + 0x14;
  }
  else {
    bVar2 = *param_1 - 4;
  }
  bVar1 = bVar2 - DAT_ram_8044;
  if (bVar2 < DAT_ram_8044) {
    return bVar1;
  }
  if (0xf < (byte)(bVar2 - DAT_ram_8044)) {
    return bVar1;
  }
  DAT_ram_8004 = 1;
  *(undefined1 *)(param_2 + 6) = 2;
  return 1;
}

