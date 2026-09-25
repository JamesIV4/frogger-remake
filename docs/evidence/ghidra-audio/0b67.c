
void audio_0b67(void)

{
  short sVar1;
  undefined1 *puVar2;
  undefined *puVar3;
  undefined1 *puVar4;
  undefined *puVar5;
  
  audio_0020();
  DAT_ram_42a3 = DAT_ram_42a7 + '\x01';
  if (DAT_ram_42a3 == '\x01') {
    DAT_ram_42a7 = '\x05';
    DAT_ram_42a3 = '\x05';
  }
  else {
    DAT_ram_42a7 = DAT_ram_42a3;
    if (DAT_ram_42a3 == '\x18') {
      DAT_ram_42a7 = '\x04';
      DAT_ram_42a3 = '\x18';
    }
  }
  audio_0030();
  DAT_ram_42a5 = 1;
  puVar4 = &DAT_ram_0993;
  puVar2 = &DAT_ram_4280;
  sVar1 = 0x18;
  do {
    *puVar2 = *puVar4;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  puVar5 = &UNK_ram_09ab + (byte)(DAT_ram_42a3 * '\x06');
  audio_0989(&UNK_ram_4282);
  audio_0989(&UNK_ram_428a);
  puVar3 = &UNK_ram_4292;
  UNK_ram_4292 = *puVar5;
  audio_0990();
  *puVar3 = *puVar5;
  return;
}

