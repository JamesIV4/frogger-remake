
undefined1 audio_0333(void)

{
  undefined1 uVar1;
  char cVar2;
  undefined1 *puVar3;
  
  if (DAT_ram_4064 == 0) {
    DAT_ram_4060 = DAT_ram_4060 + -1;
    if (DAT_ram_4060 == '\0') {
      DAT_ram_4060 = ' ';
      cVar2 = audio_0010();
      if ((char)(cVar2 + -1) == '\0') {
        DAT_ram_4064 = DAT_ram_4064 + 1;
      }
      audio_0018(cVar2 + -1);
    }
  }
  else if (DAT_ram_4064 == 1) {
    DAT_ram_4067 = 0x300;
    audio_0028();
    audio_0018();
    DAT_ram_4064 = DAT_ram_4064 + 1;
  }
  else if (DAT_ram_4064 < 3) {
    puVar3 = &DAT_ram_4061;
    DAT_ram_4061 = DAT_ram_4061 + -1;
    if (DAT_ram_4061 == '\0') {
      DAT_ram_4061 = '\x03';
      uVar1 = audio_024d();
      audio_0028(uVar1,puVar3 + -8);
      DAT_ram_4062 = DAT_ram_4062 + -1;
      if (DAT_ram_4062 == '\0') {
        DAT_ram_4062 = '\x14';
        DAT_ram_4064 = DAT_ram_4064 + 1;
      }
    }
  }
  else {
    if (DAT_ram_4064 != 3) {
      cVar2 = audio_0010();
      if (cVar2 == '\x01') {
        DAT_ram_42a5 = 0;
        return 0xff;
      }
      audio_0018();
      return 0;
    }
    DAT_ram_4063 = DAT_ram_4063 + -1;
    if (DAT_ram_4063 == '\0') {
      DAT_ram_4063 = '\x01';
      DAT_ram_4067 = DAT_ram_4067 + -0x20;
    }
    audio_0028(3,DAT_ram_4067);
    if (DAT_ram_4065 + -1 == 0) {
      DAT_ram_4064 = DAT_ram_4064 + 1;
    }
    else {
      DAT_ram_4064 = DAT_ram_4064 - 1;
      DAT_ram_4065 = DAT_ram_4065 + -1;
    }
  }
  return 0;
}

