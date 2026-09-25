
undefined1 audio_0661(void)

{
  char cVar1;
  undefined1 uVar2;
  undefined1 *puVar3;
  
  if (DAT_ram_41e4 == 0) {
    DAT_ram_41e4 = 1;
    audio_0018(0);
  }
  else if (DAT_ram_41e4 == 1) {
    DAT_ram_41e7 = 0x300;
    audio_0028();
    audio_0018();
    DAT_ram_41e4 = DAT_ram_41e4 + 1;
  }
  else if (DAT_ram_41e4 < 3) {
    puVar3 = &DAT_ram_41e1;
    DAT_ram_41e1 = DAT_ram_41e1 + -1;
    if (DAT_ram_41e1 == '\0') {
      DAT_ram_41e1 = '\x03';
      uVar2 = audio_024d();
      audio_0028(uVar2,puVar3 + -8);
      DAT_ram_41e2 = DAT_ram_41e2 + -1;
      if (DAT_ram_41e2 == '\0') {
        DAT_ram_41e2 = '\x14';
        DAT_ram_41e4 = DAT_ram_41e4 + 1;
      }
    }
  }
  else {
    if (DAT_ram_41e4 != 3) {
      cVar1 = audio_0010();
      if (cVar1 == '\x01') {
        DAT_ram_42a5 = 0;
        return 0xff;
      }
      audio_0018();
      return 0;
    }
    DAT_ram_41e3 = DAT_ram_41e3 + -1;
    if (DAT_ram_41e3 == '\0') {
      DAT_ram_41e3 = '\x01';
      DAT_ram_41e7 = DAT_ram_41e7 + -0x20;
    }
    audio_0028(3,DAT_ram_41e7);
    if (DAT_ram_41e5 + -1 == 0) {
      DAT_ram_41e4 = DAT_ram_41e4 + 1;
    }
    else {
      DAT_ram_41e4 = DAT_ram_41e4 - 1;
      DAT_ram_41e5 = DAT_ram_41e5 + -1;
    }
  }
  return 0;
}

