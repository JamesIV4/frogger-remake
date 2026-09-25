
char renderFilledHomeSlots(char *param_1)

{
  char cVar1;
  
  if (*param_1 != '\0') {
    FUN_ram_0a05(&DAT_ram_ab64);
  }
  param_1 = param_1 + 1;
  if (*param_1 != '\0') {
    FUN_ram_0a05(&DAT_ram_aaa4);
  }
  param_1 = param_1 + 1;
  if (*param_1 != '\0') {
    FUN_ram_0a05(&DAT_ram_a9e4);
  }
  param_1 = param_1 + 1;
  if (*param_1 != '\0') {
    FUN_ram_0a05(&DAT_ram_a924);
  }
  cVar1 = param_1[1];
  if (param_1[1] == '\0') {
    return cVar1;
  }
  DAT_ram_a864 = 0x6c;
  DAT_ram_a865 = 0x6d;
  DAT_ram_a884 = 0x6e;
  DAT_ram_a885 = 0x6f;
  return cVar1;
}

