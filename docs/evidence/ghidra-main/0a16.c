
undefined1 renderTimeBar(void)

{
  char cVar1;
  char *pcVar2;
  undefined1 *puVar3;
  
  if (DAT_ram_83e4 != -1) {
    if (DAT_ram_83fe == '\0') {
      pcVar2 = &DAT_ram_83e4;
    }
    else if (DAT_ram_83fd == '\x01') {
      pcVar2 = &DAT_ram_83e5;
    }
    else {
      pcVar2 = &DAT_ram_83e6;
    }
    puVar3 = &DAT_ram_abbe;
    for (cVar1 = *pcVar2; cVar1 != '\0'; cVar1 = cVar1 + -1) {
      *puVar3 = 0x4d;
      puVar3 = puVar3 + -0x20;
    }
    *puVar3 = 0x10;
    return 0x4d;
  }
  return 0;
}

