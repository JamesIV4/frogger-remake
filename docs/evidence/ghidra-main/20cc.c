
void blitScrollTileGrid(undefined2 param_1,undefined1 *param_2)

{
  char cVar1;
  char cVar2;
  undefined1 *puVar3;
  
  DAT_ram_8003 = (char)((ushort)param_1 >> 8);
  cVar1 = (char)param_1;
  cVar2 = DAT_ram_8003;
  puVar3 = switchD_ram:0fbd::caseD_53;
  switchD_ram:0fbd::caseD_40 = param_2;
  do {
    do {
      *puVar3 = *param_2;
      puVar3[1] = param_2[1];
      puVar3 = puVar3 + 0x20;
      param_2 = param_2 + 2;
      cVar2 = cVar2 + -1;
    } while (cVar2 != '\0');
    puVar3 = puVar3 + switchD_ram:0fbd::caseD_5a;
    cVar1 = cVar1 + -1;
    cVar2 = DAT_ram_8003;
    param_2 = switchD_ram:0fbd::caseD_40;
  } while (cVar1 != '\0');
  return;
}

