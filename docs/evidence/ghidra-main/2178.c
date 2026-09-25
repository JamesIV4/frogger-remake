
void FUN_ram_2178(undefined2 param_1,undefined1 *param_2,undefined1 *param_3)

{
  char cVar1;
  
  cVar1 = (char)((ushort)param_1 >> 8);
  do {
    *param_2 = *param_3;
    param_2[1] = param_3[1];
    param_3 = param_3 + 2;
    param_2 = param_2 + 0x20;
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  return;
}

