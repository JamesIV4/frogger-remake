
char FUN_ram_166b(void)

{
  char cVar1;
  short sVar2;
  undefined1 *puVar3;
  char *pcVar4;
  undefined1 *puVar5;
  char *pcVar6;
  
  if (DAT_ram_42c8 != '\0') {
    return DAT_ram_42c8;
  }
  puVar5 = &DAT_ram_1694;
  puVar3 = &DAT_ram_42b0;
  sVar2 = 10;
  do {
    *puVar3 = *puVar5;
    puVar3 = puVar3 + 1;
    puVar5 = puVar5 + 1;
    sVar2 = sVar2 + -1;
  } while (sVar2 != 0);
  pcVar6 = &UNK_ram_169e + (byte)(DAT_ram_42c3 * '\x06');
  pcVar4 = (char *)&DAT_ram_42b2;
  DAT_ram_42b2._0_1_ = *pcVar6;
  audio_1691();
  cVar1 = *pcVar6;
  *pcVar4 = cVar1;
  return cVar1;
}

