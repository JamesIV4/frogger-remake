
void FUN_ram_010b(undefined2 param_1,undefined1 *param_2)

{
  undefined1 uVar1;
  byte bVar2;
  undefined1 *puVar3;
  
  do {
    puVar3 = param_2;
    *puVar3 = (char)((ushort)param_1 >> 8);
    param_2 = puVar3 + 1;
  } while ((char)((ushort)(puVar3 + 1) >> 8) != 'D');
  DAT_ram_4280 = 0xff;
  setInterruptMode(1);
  *(undefined2 *)(puVar3 + -1) = 0x11f;
  audio_026d(0x3f);
  *(undefined2 *)(puVar3 + -1) = 0x124;
  audio_0008(8);
  *(undefined2 *)(puVar3 + -1) = 0x127;
  audio_0008(9);
  *(undefined2 *)(puVar3 + -1) = 0x12a;
  audio_0008(10);
  *(undefined2 *)(puVar3 + -1) = 0x12f;
  DAT_ram_6000 = audio_0008(7);
  DAT_ram_404e = 0x6000;
  do {
    enableMaskableInterrupts();
    DAT_ram_403f = DAT_ram_403f + '\x01';
    do {
      *(undefined2 *)(puVar3 + -1) = 0x140;
      bVar2 = audio_02c1(0xf);
    } while ((bVar2 & 8) != 0);
    do {
      *(undefined2 *)(puVar3 + -1) = 0x149;
      bVar2 = audio_02c1(0xf);
      uVar1 = DAT_ram_4040;
    } while ((bVar2 & 8) == 0);
    disableMaskableInterrupts();
    DAT_ram_404b = '\x01';
    if (DAT_ram_4041 == '\0') {
      *(undefined2 *)(puVar3 + -1) = 399;
      audio_01d9(uVar1);
    }
    else {
      *(undefined2 *)(puVar3 + -1) = 0x15f;
      audio_01e8(uVar1);
    }
    uVar1 = DAT_ram_4042;
    enableMaskableInterrupts();
    disableMaskableInterrupts();
    DAT_ram_404b = DAT_ram_404b + '\x01';
    if (DAT_ram_4043 == '\0') {
      *(undefined2 *)(puVar3 + -1) = 0x195;
      audio_01d9(uVar1);
    }
    else {
      *(undefined2 *)(puVar3 + -1) = 0x174;
      audio_01e8(uVar1);
    }
    uVar1 = DAT_ram_4044;
    enableMaskableInterrupts();
    disableMaskableInterrupts();
    DAT_ram_404b = DAT_ram_404b + '\x01';
    if (DAT_ram_4045 == '\0') {
      *(undefined2 *)(puVar3 + -1) = 0x19b;
      audio_01d9(uVar1);
    }
    else {
      *(undefined2 *)(puVar3 + -1) = 0x189;
      audio_01e8(uVar1);
    }
  } while( true );
}

