
void copyDiveAnimFrame(short param_1,short param_2)

{
  ushort uVar1;
  ushort uVar2;
  
  uVar1 = (ushort)DAT_ram_814e;
  DAT_ram_814e = DAT_ram_814e + 2;
  uVar2 = (ushort)DAT_ram_8145;
  DAT_ram_8145 = DAT_ram_8145 + 0x20;
  *(undefined1 *)(param_2 + uVar2) = *(undefined1 *)(param_1 + uVar1);
  ((undefined1 *)(param_2 + uVar2))[1] = ((undefined1 *)(param_1 + uVar1))[1];
  if (DAT_ram_814e < 0x10) {
    return;
  }
  DAT_ram_814f = 0;
  DAT_ram_814e = 0;
  DAT_ram_8145 = 0;
  DAT_ram_8146 = 0;
  DAT_ram_8147 = 0;
  return;
}

