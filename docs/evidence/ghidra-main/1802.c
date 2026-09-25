
char advanceAnimationFrameBuffer(void)

{
  char cVar1;
  short sVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  
  if (DAT_ram_814f != '\0') {
    return DAT_ram_814f;
  }
  if (DAT_ram_815b != '\0') {
    return DAT_ram_815b;
  }
  cVar1 = DAT_ram_81b4;
  if (DAT_ram_81b4 != '\0') {
    DAT_ram_81b4 = DAT_ram_81b4 + -1;
    return cVar1;
  }
  puVar4 = *(undefined1 **)(&DAT_ram_1841 + (ushort)DAT_ram_81b3 * 2);
  DAT_ram_81b4 = 0x15;
  cVar1 = DAT_ram_81b3 - 9;
  if (cVar1 == '\0') {
    DAT_ram_81b3 = cVar1;
    return cVar1;
  }
  puVar3 = &DAT_ram_819b;
  sVar2 = 0xb;
  DAT_ram_81b3 = DAT_ram_81b3 + 1;
  do {
    *puVar3 = *puVar4;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
    sVar2 = sVar2 + -1;
  } while (sVar2 != 0);
  return cVar1;
}

