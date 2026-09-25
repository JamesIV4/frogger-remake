
undefined1 blitFrogAnimColumnOnTrigger(void)

{
  char cVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  
  if (DAT_ram_8118 == '\0') {
    return 0;
  }
  puVar2 = &DAT_ram_a806;
  cVar1 = '\b';
  puVar3 = &DAT_ram_1413;
  do {
    *puVar2 = *puVar3;
    puVar2[1] = puVar3[1];
    puVar3 = puVar3 + 2;
    puVar2 = puVar2 + 0x20;
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  DAT_ram_8118 = 0;
  return 0;
}

