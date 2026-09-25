
void RST6(undefined2 param_1,undefined1 *param_2,undefined1 *param_3)

{
  char cVar1;
  byte bVar2;
  
  cVar1 = (char)((ushort)param_1 >> 8);
  do {
    param_2 = (undefined1 *)CONCAT11((char)((ushort)param_2 >> 8) + -1,(char)param_2);
    do {
      param_3 = param_3 + 1;
      cVar1 = cVar1 + -1;
      if (cVar1 == '\0') {
        return;
      }
      *param_2 = *param_3;
      bVar2 = (byte)param_2;
      param_2 = (undefined1 *)CONCAT11((char)((ushort)param_2 >> 8),bVar2 - 0x20);
    } while (0x1f < bVar2);
  } while( true );
}

