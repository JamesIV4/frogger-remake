
void audio_0230(void)

{
  if ((DAT_ram_404b & 3) == 0) {
    return;
  }
  audio_0008((DAT_ram_404b & 3) + 7);
  return;
}

