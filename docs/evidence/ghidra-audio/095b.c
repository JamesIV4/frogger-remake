
char FUN_ram_095b(void)

{
  char cVar1;
  short sVar2;
  undefined1 *puVar3;
  char *pcVar4;
  undefined1 *puVar5;
  char *pcVar6;
  
  if (DAT_ram_42a5 != '\0') {
    return DAT_ram_42a5;
  }
  puVar5 = &DAT_ram_0993;
  puVar3 = &DAT_ram_4280;
  sVar2 = 0x18;
  do {
    *puVar3 = *puVar5;
    puVar3 = puVar3 + 1;
    puVar5 = puVar5 + 1;
    sVar2 = sVar2 + -1;
  } while (sVar2 != 0);
  pcVar6 = &UNK_ram_09ab + (byte)(DAT_ram_42a3 * '\x06');
  audio_0989(&UNK_ram_4282);
  audio_0989(&UNK_ram_428a);
  pcVar4 = &UNK_ram_4292;
  UNK_ram_4292 = *pcVar6;
  audio_0990();
  cVar1 = *pcVar6;
  *pcVar4 = cVar1;
  return cVar1;
}

