
byte flagSpriteObjectFrogHit(byte *param_1,short param_2)

{
  byte bVar1;
  byte bVar2;
  
  if (*(char *)(param_2 + 6) == '\0') {
    return 0;
  }
  bVar2 = *(char *)(param_2 + 4) + 2;
  if (bVar2 != DAT_ram_8047) {
    return bVar2;
  }
  bVar2 = *param_1;
  if (*(char *)(param_2 + 5) != '\0') {
    bVar2 = bVar2 + 0x10;
  }
  bVar1 = bVar2 - DAT_ram_8044;
  if (bVar2 < DAT_ram_8044) {
    return bVar1;
  }
  if (0xf < (byte)(bVar2 - DAT_ram_8044)) {
    return bVar1;
  }
  DAT_ram_8004 = 1;
  DAT_ram_842c = 1;
  return 1;
}

