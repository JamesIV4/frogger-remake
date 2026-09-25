
void audio_0304(void)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  
  bVar1 = 0;
  bVar2 = 2;
  bVar3 = 0xff;
  bVar4 = 0xfc;
  if (DAT_ram_404b != 2) {
    if (DAT_ram_404b < 2) {
      bVar3 = 0x3f;
      bVar4 = 0xff;
      bVar2 = 0;
      bVar1 = 0x80;
    }
    else {
      bVar3 = 0xff;
      bVar4 = 0xf3;
      bVar2 = 8;
      bVar1 = 0;
    }
  }
  bVar1 = (byte)DAT_ram_404e & bVar3 | bVar1;
  DAT_ram_404e = (byte *)CONCAT11((byte)((ushort)DAT_ram_404e >> 8) & bVar4 | bVar2,bVar1);
  *DAT_ram_404e = bVar1;
  return;
}

