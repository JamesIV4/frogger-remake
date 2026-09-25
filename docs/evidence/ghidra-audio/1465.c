
void audio_1465(void)

{
  short sVar1;
  undefined1 *puVar2;
  undefined2 *puVar3;
  undefined1 *puVar4;
  undefined *puVar5;
  
  audio_0020();
  DAT_ram_42c8 = 1;
  DAT_ram_42c3 = '\x01';
  audio_0030();
  puVar4 = &DAT_ram_1694;
  puVar2 = &DAT_ram_42b0;
  sVar1 = 10;
  do {
    *puVar2 = *puVar4;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  puVar5 = &UNK_ram_169e + (byte)(DAT_ram_42c3 * '\x06');
  puVar3 = &DAT_ram_42b2;
  DAT_ram_42b2._0_1_ = *puVar5;
  audio_1691();
  *(undefined *)puVar3 = *puVar5;
  return;
}

