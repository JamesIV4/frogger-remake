
/* WARNING: Instruction at (ram,0x0743) overlaps instruction at (ram,0x0742)
    */

undefined1 audio_070e(void)

{
  undefined1 uVar1;
  short sVar2;
  
  sVar2 = DAT_ram_4180 + -1;
  DAT_ram_4180 = sVar2;
  if (sVar2 == 0) {
    if ((DAT_ram_4184 & 4) == 0) {
      audio_06eb();
      uVar1 = audio_027c();
      DAT_ram_4184 = DAT_ram_4184 | 4;
      return uVar1;
    }
    audio_027c(0);
    return 0xff;
  }
  if ((DAT_ram_4184 & 1) == 0) {
    audio_024d(0);
    if ((DAT_ram_4184 & 2) == 0) {
      sVar2 = sVar2 + 10;
    }
    audio_023c(0x19,sVar2 + -10);
    uVar1 = audio_0759(2);
    DAT_ram_4183 = DAT_ram_4183 + -1;
    if (DAT_ram_4183 != '\0') {
      return uVar1;
    }
    DAT_ram_4183 = '\t';
    audio_027c();
  }
  else {
    DAT_ram_4182 = DAT_ram_4182 + -1;
    if (DAT_ram_4182 != '\0') {
      return 0;
    }
    DAT_ram_4182 = '$';
    audio_027c();
  }
  DAT_ram_4184 = DAT_ram_4184 ^ 1;
  return 0;
}

