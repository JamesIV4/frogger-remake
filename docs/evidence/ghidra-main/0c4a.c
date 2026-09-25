
void stampRankMarkerIfPlaced(char param_1,undefined2 param_2,undefined2 param_3)

{
  char cVar1;
  
  cVar1 = (char)((ushort)param_3 >> 8);
  param_1 = cVar1 - param_1;
  if (param_1 == cVar1) {
    return;
  }
  *(char *)CONCAT11((char)((ushort)param_2 >> 8),param_1) = (char)param_3;
  return;
}

