
void animateSpriteObjectFrame(short param_1,short param_2)

{
  char cVar1;
  byte bVar2;
  
  cVar1 = *(char *)(param_2 + 8) + -1;
  *(char *)(param_2 + 8) = cVar1;
  if (cVar1 != '\0') {
    return;
  }
  *(undefined1 *)(param_2 + 8) = 0xc;
  if (*(char *)(param_2 + 6) == '\0') {
    return;
  }
  bVar2 = *(char *)(param_2 + 6) - 1;
  if (bVar2 == 0) {
    bVar2 = 4;
  }
  *(byte *)(param_2 + 6) = bVar2;
  bVar2 = *(byte *)(bVar2 + 0x2cd5) | *(byte *)(param_2 + 5);
  *(byte *)(param_1 + 1) = bVar2;
  *(byte *)(param_1 + 5) = bVar2 + 1;
  *(undefined1 *)(param_1 + 2) = 4;
  *(undefined1 *)(param_1 + 6) = 4;
  return;
}

