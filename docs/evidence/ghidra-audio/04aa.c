
undefined1 audio_04aa(void)

{
  char cVar1;
  
  if (DAT_ram_4173 == 0) {
    cVar1 = audio_0010();
    if (cVar1 == '\f') {
      DAT_ram_4173 = DAT_ram_4173 + 1;
    }
    audio_0018();
  }
  else if (DAT_ram_4173 == 1) {
    audio_04eb();
  }
  else if (DAT_ram_4173 < 3) {
    cVar1 = audio_0010();
    if ((char)(cVar1 + -1) == '\0') {
      DAT_ram_4173 = DAT_ram_4173 + 1;
    }
    audio_0018(cVar1 + -1);
  }
  else if (DAT_ram_4173 == 3) {
    audio_04f8();
  }
  else {
    DAT_ram_4172 = DAT_ram_4172 + -1;
    if (DAT_ram_4172 == '\0') {
      return 0xff;
    }
    DAT_ram_4173 = 0;
  }
  return 0;
}

