
undefined1 audio_03d2(undefined2 param_1,undefined2 param_2)

{
  undefined1 uVar1;
  byte bVar2;
  char cVar3;
  char cVar4;
  
  cVar3 = (char)((ushort)param_1 >> 8);
  DAT_ram_405d = DAT_ram_405d - 1;
  if (DAT_ram_405d == 0) {
    audio_0304();
    DAT_ram_405e = 0x80;
    audio_027c();
    audio_023c(0xfc);
    audio_0030();
    DAT_ram_405d = 0;
    return 0;
  }
  if (DAT_ram_405d == 0xff) {
    while( true ) {
      cVar4 = (char)param_2;
      bVar2 = DAT_ram_405e - 1;
      DAT_ram_405e = bVar2;
      if (0x40 < bVar2) {
        audio_024d();
        audio_0028(cVar4 + -2);
        DAT_ram_405d = 0;
        return 0;
      }
      if (bVar2 == 0x40) {
        audio_0010();
        cVar3 = cVar3 + -1;
        bVar2 = audio_0018();
      }
      if (bVar2 != 0) break;
      audio_0010(0);
      cVar3 = cVar3 + -1;
      if (cVar3 == '\0') {
        return 0xff;
      }
      audio_0018();
      param_2 = 0;
      audio_023c(0);
      DAT_ram_405e = -0x80;
    }
    audio_024d();
    audio_0028(cVar4 + '\x02');
    DAT_ram_405d = 0;
    return 0;
  }
  if (0x1f < DAT_ram_405d) {
    if (DAT_ram_405d < 0x30) {
      audio_023c(0x3c);
      audio_027c();
      return 0;
    }
    if (0x6f < DAT_ram_405d) {
      return 0;
    }
  }
  uVar1 = audio_027c();
  return uVar1;
}

