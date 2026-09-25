
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void audio_06eb(void)

{
  DAT_ram_4180 = 0x50;
  _DAT_ram_4182 = &UNK_ram_0924;
  DAT_ram_4184 = 0;
  audio_027c();
  audio_023c(0x50);
  audio_0260();
  audio_02c7();
  return;
}

