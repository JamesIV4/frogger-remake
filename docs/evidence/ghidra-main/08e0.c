
/* WARNING: Removing unreachable block (ram,0x08fb) */
/* WARNING: Removing unreachable block (ram,0x08f5) */

char addScoreAndAwardExtraLife(undefined2 param_1)

{
  bool bVar1;
  bool bVar2;
  byte bVar3;
  byte bVar4;
  char cVar5;
  undefined1 *puVar6;
  char *pcVar7;
  byte bVar8;
  byte bVar10;
  ushort uVar9;
  byte *pbVar11;
  undefined *puVar12;
  
  if (DAT_ram_83fe == '\0') {
    return '\0';
  }
  if (DAT_ram_83fd == '\x01') {
    pbVar11 = (byte *)&DAT_ram_83ed;
  }
  else {
    pbVar11 = (byte *)&DAT_ram_83eb;
  }
  bVar8 = (byte)param_1;
  bVar3 = *pbVar11;
  bVar1 = ((bVar8 & 0xf) + (bVar3 & 0xf) & 0x10) != 0;
  bVar4 = BCDadjust(bVar8 + bVar3,CARRY1(bVar8,bVar3),bVar1);
  bVar3 = BCDadjustCarry(bVar4,CARRY1(bVar8,bVar3),bVar1);
  hasEvenParity(bVar4);
  *pbVar11 = bVar4;
  bVar10 = (byte)((ushort)param_1 >> 8);
  bVar8 = pbVar11[1];
  bVar3 = bVar3 & 1;
  bVar1 = ((bVar10 & 0xf) + (bVar8 & 0xf) + bVar3 & 0x10) != 0;
  bVar2 = CARRY1(bVar10,bVar8) || CARRY1(bVar10 + bVar8,bVar3);
  bVar3 = BCDadjust(bVar10 + bVar8 + bVar3,bVar2,bVar1);
  BCDadjustCarry(bVar3,bVar2,bVar1);
  hasEvenParity(bVar3);
  pbVar11[1] = bVar3;
  uVar9 = CONCAT11(bVar3,bVar4);
  if (DAT_ram_83fd == '\x01') {
    puVar6 = &DAT_ram_83e7;
    cVar5 = DAT_ram_83e7;
  }
  else {
    puVar6 = &DAT_ram_83e8;
    cVar5 = DAT_ram_83e8;
  }
  if ((cVar5 == '\0') && ((DAT_ram_2e08 == uVar9 || (DAT_ram_2e08 < uVar9)))) {
    DAT_ram_83cf = cVar5;
    *puVar6 = 1;
    pcVar7 = (char *)CONCAT11((char)((ushort)puVar6 >> 8),(char)puVar6 + -2);
    cVar5 = *pcVar7 + '\x01';
    *pcVar7 = cVar5;
    puVar12 = &UNK_ram_abde;
    do {
      puVar12 = puVar12 + -0x20;
      cVar5 = cVar5 + -1;
    } while (cVar5 != '\0');
    *puVar12 = 0x4d;
    cVar5 = enqueueSoundCommand(7);
  }
  if (DAT_ram_83ef < uVar9) {
    DAT_ram_83ef = uVar9;
    return cVar5;
  }
  return cVar5;
}

