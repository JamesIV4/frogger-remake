
void audio_02b0(void)

{
  char cVar1;
  byte bVar2;
  byte bVar3;
  
  bVar2 = 0x80;
  bVar3 = 0xfb;
  cVar1 = DAT_ram_404b;
  do {
    bVar3 = bVar3 << 1 | bVar3 >> 7;
    bVar2 = bVar2 << 1 | bVar2 >> 7;
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  DAT_io_0080 = 7;
  DAT_ram_404c = DAT_ram_404c & bVar3 | bVar2;
  DAT_io_0040 = DAT_ram_404c;
  return;
}

