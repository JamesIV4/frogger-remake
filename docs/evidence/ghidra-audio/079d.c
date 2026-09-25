
undefined1 audio_079d(void)

{
  if (DAT_ram_4290 != -1) {
    audio_07b7();
    return 0;
  }
  DAT_ram_42a5 = 0;
  DAT_ram_42a6 = 0;
  return 0xff;
}

