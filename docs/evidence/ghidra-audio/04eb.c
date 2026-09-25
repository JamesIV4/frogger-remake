
void audio_04eb(void)

{
  DAT_ram_4170 = DAT_ram_4170 + -1;
  if (DAT_ram_4170 != '\0') {
    return;
  }
  DAT_ram_4170 = 8;
  DAT_ram_4173 = DAT_ram_4173 + '\x01';
  return;
}

