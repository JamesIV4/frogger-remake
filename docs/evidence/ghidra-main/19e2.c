
void blitFourTileGroupColumn(undefined1 *param_1)

{
  char cVar1;
  
  cVar1 = '\x0e';
  do {
    *param_1 = 0x48;
    param_1[1] = 0x49;
    param_1[0x20] = 0x4a;
    param_1[0x21] = 0x4b;
    param_1 = param_1 + 0x40;
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  return;
}

