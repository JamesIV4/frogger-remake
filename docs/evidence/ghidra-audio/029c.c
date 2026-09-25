
void audio_029c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = 0x84;
  cVar2 = DAT_ram_404b;
  do {
    bVar1 = bVar1 << 1 | bVar1 >> 7;
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  audio_026d(bVar1);
  audio_0230();
  audio_02c7();
  return;
}

