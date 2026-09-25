
void audio_05ed(void)

{
  if ((DAT_ram_4110 & 1) == 0) {
    DAT_ram_4111 = DAT_ram_4111 + -1;
    if (DAT_ram_4111 != '\0') {
      return;
    }
    DAT_ram_4111 = '\x04';
    audio_027c(DAT_ram_4113);
  }
  else {
    DAT_ram_4112 = DAT_ram_4112 + -1;
    if (DAT_ram_4112 != '\0') {
      return;
    }
    DAT_ram_4112 = '\x04';
    audio_027c();
  }
  DAT_ram_4110 = DAT_ram_4110 ^ 1;
  return;
}

