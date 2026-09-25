
void dequeueSoundCommand(void)

{
  ushort uVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  
  if (DAT_ram_8300 == 0) {
    return;
  }
  uVar1 = (ushort)DAT_ram_8300;
  puVar2 = &DAT_ram_8301;
  DAT_ram_8300 = DAT_ram_8300 - 1;
  issueSoundCommand(DAT_ram_8301);
  puVar3 = (undefined1 *)CONCAT11((char)((ushort)puVar2 >> 8),(char)puVar2 + '\x01');
  uVar1 = uVar1 & 0xff;
  do {
    *puVar2 = *puVar3;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
    uVar1 = uVar1 - 1;
  } while (uVar1 != 0);
  return;
}

