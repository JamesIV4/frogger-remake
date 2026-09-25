
void fillTenCellRun(undefined1 *param_1)

{
  char cVar1;
  
  cVar1 = '\n';
  do {
    *param_1 = 0x10;
    param_1 = param_1 + 1;
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  return;
}

