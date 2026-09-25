
void audio_04f8(void)

{
  DAT_ram_4171 = DAT_ram_4171 + -1;
  if (DAT_ram_4171 != '\0') {
    return;
  }
  DAT_ram_4171 = 0xc;
  DAT_ram_4173 = DAT_ram_4173 + '\x01';
  return;
}

