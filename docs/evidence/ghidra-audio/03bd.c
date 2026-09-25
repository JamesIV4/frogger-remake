
void audio_03bd(void)

{
  DAT_ram_405d = 0x80;
  audio_027c();
  audio_023c(0x70);
  audio_0030();
  audio_0304();
  return;
}

