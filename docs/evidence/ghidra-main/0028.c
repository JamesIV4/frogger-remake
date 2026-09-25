
void copyRunUpTileColumn(undefined2 param_1,undefined1 *param_2,undefined1 *param_3)

{
  char cVar1;
  char cVar2;
  
  cVar1 = (char)((ushort)param_1 >> 8);
  do {
    *param_2 = *param_3;
    cVar2 = (char)((ushort)param_2 >> 8);
    if ((byte)param_2 < 0x20) {
      cVar2 = cVar2 + -1;
    }
    param_2 = (undefined1 *)CONCAT11(cVar2,(byte)param_2 - 0x20);
    param_3 = param_3 + 1;
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  return;
}

