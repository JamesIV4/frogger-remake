
void FUN_ram_0097(undefined1 param_1,undefined1 *param_2)

{
  byte in_F;
  undefined1 uVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  byte bVar5;
  
  DAT_ram_4046 = param_1;
  audio_00e6();
  if ((bool)(in_F & 1)) {
    return;
  }
  uVar1 = audio_00e6();
  if (!(bool)(in_F & 1)) {
    bVar2 = audio_0102(DAT_ram_4040);
    bVar3 = audio_0102(DAT_ram_4042);
    DAT_ram_4049 = audio_0102(DAT_ram_4044);
    bVar4 = audio_0102(DAT_ram_4046);
    param_2 = &DAT_ram_4049;
    bVar5 = bVar2;
    if (bVar3 <= bVar2) {
      bVar5 = bVar3;
    }
    if (DAT_ram_4049 <= bVar5) {
      bVar5 = DAT_ram_4049;
    }
    if (bVar4 <= bVar5) {
      return;
    }
    uVar1 = 1;
    if ((bVar5 != bVar2) && (uVar1 = 2, bVar5 != bVar3)) {
      uVar1 = 3;
    }
  }
  audio_008c(uVar1);
  *param_2 = DAT_ram_4046;
  param_2[1] = 0;
  return;
}

