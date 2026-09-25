
void audio_0020(void)

{
  byte bVar1;
  byte bVar2;
  
  bVar1 = 0xff;
  bVar2 = 0xfc;
  if (DAT_ram_404b != 2) {
    if (DAT_ram_404b < 2) {
      bVar1 = 0x3f;
      bVar2 = 0xff;
    }
    else {
      bVar1 = 0xff;
      bVar2 = 0xf3;
    }
  }
  bVar1 = (byte)DAT_ram_404e & bVar1;
  DAT_ram_404e = (byte *)CONCAT11((byte)((ushort)DAT_ram_404e >> 8) & bVar2,bVar1);
  *DAT_ram_404e = bVar1;
  return;
}

