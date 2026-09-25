
undefined1 audio_148c(void)

{
  if (DAT_ram_42c8 != '\0') {
    return 0xff;
  }
  audio_0020(0);
  if (DAT_ram_42b0 == -1) {
    return 0xff;
  }
  audio_14a9();
  return 0;
}

