
char stepDiveFrameCounter(char *param_1)

{
  char cVar1;
  
  cVar1 = *param_1;
  if (cVar1 != '\0') {
    *param_1 = *param_1 + -1;
    return cVar1;
  }
  cVar1 = DAT_ram_8146;
  *param_1 = DAT_ram_8146;
  return cVar1;
}

