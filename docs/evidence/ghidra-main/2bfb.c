
void writeSpriteObjectSlotAttr(short param_1,short param_2)

{
  if (*(byte *)(param_2 + 6) == 0) {
    return;
  }
  *(byte *)(param_1 + 1) = (&DAT_ram_2cd9)[*(byte *)(param_2 + 6)] | *(byte *)(param_2 + 5);
  *(undefined1 *)(param_1 + 2) = 2;
  return;
}

