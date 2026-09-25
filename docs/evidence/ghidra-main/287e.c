
char armTwoPairFigureFrame(void)

{
  char cVar1;
  
  if (DAT_ram_814f != '\0') {
    return DAT_ram_814f;
  }
  DAT_ram_8150 = 1;
  cVar1 = FUN_ram_289c();
  return cVar1;
}

