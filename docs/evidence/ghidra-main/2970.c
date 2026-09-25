
/* WARNING: Instruction at (ram,0x299c) overlaps instruction at (ram,0x299a)
    */

void driveSpriteObjectCluster(undefined2 param_1)

{
  byte bVar1;
  char cVar2;
  undefined *puVar3;
  
  cVar2 = (char)((ushort)param_1 >> 8);
  if (2 < DAT_ram_83b7) {
    if ((char)(DAT_ram_83fd + -1) == '\0') {
      puVar3 = &UNK_ram_8440;
    }
    else {
      puVar3 = &UNK_ram_8460;
    }
    dispatchSpriteObjectArmsA(DAT_ram_83fd + -1,&DAT_ram_8048);
    bVar1 = DAT_ram_83b7;
    if (DAT_ram_83b7 < 6) {
      bVar1 = DAT_ram_83b7 + cVar2;
    }
    dispatchSpriteObjectArmsA(bVar1,&DAT_ram_8050,puVar3 + 0x10);
  }
  if ((char)(DAT_ram_83fd + -1) == '\0') {
    puVar3 = &UNK_ram_8480;
  }
  else {
    puVar3 = &UNK_ram_8490;
  }
  updateSpriteObject(DAT_ram_83fd + -1,&DAT_ram_8058,puVar3);
  return;
}

