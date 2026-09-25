
void audio_030f(void)

{
  audio_0020();
  DAT_ram_4060 = 0x20;
  DAT_ram_4061 = 3;
  DAT_ram_4062 = 0x14;
  DAT_ram_4063 = 1;
  DAT_ram_4064 = 0;
  DAT_ram_4065 = 0x10;
  audio_0028(0x20);
  audio_0030();
  audio_0018();
  return;
}

