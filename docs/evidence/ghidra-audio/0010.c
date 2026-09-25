
void audio_0010(void)

{
  if ((DAT_ram_404b & 3) == 0) {
    return;
  }
  audio_02c1((DAT_ram_404b & 3) + 7);
  return;
}

