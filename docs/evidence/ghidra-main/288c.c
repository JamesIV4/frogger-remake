
char resetDiveSurfaceCounter(void)

{
  char cVar1;
  
  if (DAT_ram_814f != '\0') {
    return DAT_ram_814f;
  }
  DAT_ram_8150 = DAT_ram_8150 + '\x01';
  cVar1 = FUN_ram_289c();
  return cVar1;
}

