
void audio_05c3(void)

{
  DAT_ram_4110 = 0;
  DAT_ram_4111 = 4;
  DAT_ram_4112 = 4;
  DAT_ram_4113 = 4;
  DAT_ram_4114 = 0x68;
  audio_02b0();
  audio_0008(6);
  audio_027c();
  audio_02c7();
  return;
}

