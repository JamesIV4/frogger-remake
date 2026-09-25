
undefined1 audio_00e6(char param_1)

{
  undefined1 uVar1;
  undefined1 *puVar2;
  
  uVar1 = 1;
  puVar2 = &DAT_ram_4040;
  if (param_1 != DAT_ram_4040) {
    uVar1 = 2;
    puVar2 = &DAT_ram_4042;
    if (param_1 != DAT_ram_4042) {
      uVar1 = 3;
      puVar2 = &DAT_ram_4044;
      if (param_1 != DAT_ram_4044) {
        return 0;
      }
    }
  }
  puVar2[1] = 0;
  return uVar1;
}

