
undefined1 audio_0475(void)

{
  short sVar1;
  
  sVar1 = DAT_ram_4130 + -1;
  DAT_ram_4130 = sVar1;
  if (sVar1 == 0) {
    return 0xff;
  }
  audio_024d(0xff);
  audio_023c(sVar1 + 3);
  return 0;
}

