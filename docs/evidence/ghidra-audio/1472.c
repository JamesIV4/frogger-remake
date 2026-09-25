
undefined1 audio_1472(void)

{
  if (DAT_ram_42b0 != -1) {
    audio_14a9();
    return 0;
  }
  DAT_ram_42c8 = 0;
  return 0xff;
}

