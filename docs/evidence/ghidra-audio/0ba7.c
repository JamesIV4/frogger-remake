
undefined1 audio_0ba7(void)

{
  if (DAT_ram_42a5 == '\0') {
    if (DAT_ram_4280 != -1) {
      audio_07b7();
      return 0;
    }
    DAT_ram_42a5 = '\0';
    DAT_ram_42a6 = 0;
  }
  return 0xff;
}

