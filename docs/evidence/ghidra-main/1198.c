
byte computeVramColumnIndex(short param_1)

{
  byte bVar1;
  byte in_F;
  char cVar2;
  ushort uVar3;
  byte bVar4;
  
  cVar2 = '\x06';
  uVar3 = (ushort)(&UNK_ram_5800 + (param_1 - (ushort)(in_F & 1))) & 0xffe0;
  do {
    bVar4 = (byte)(uVar3 >> 8);
    bVar1 = (byte)uVar3 >> 7;
    uVar3 = CONCAT11(bVar4 << 1 | bVar1,(byte)uVar3 << 1 | bVar1);
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  return bVar4 & 4;
}

