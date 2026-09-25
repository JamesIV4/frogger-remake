
/* WARNING: Removing unreachable block (ram,0x08fb) */
/* WARNING: Removing unreachable block (ram,0x08f5) */
/* WARNING: Removing unreachable block (ram,0x08a2) */

char driveScoreDisplayCountdown(void)

{
  bool bVar1;
  bool bVar2;
  byte bVar3;
  byte bVar4;
  char cVar5;
  undefined1 *puVar6;
  char *pcVar7;
  ushort uVar8;
  byte bVar9;
  byte bVar12;
  byte *pbVar10;
  undefined *puVar11;
  
  if (DAT_ram_83cd != '\0') {
    return DAT_ram_83cd;
  }
  if (DAT_ram_8004 != '\0') {
    return DAT_ram_8004;
  }
  if (DAT_ram_83ae == '\0') {
    DAT_ram_83ae = '\x01';
    enqueueSoundCommand(6);
  }
  initDisplayFieldOnce();
  if (DAT_ram_83df != '\0') {
    if (DAT_ram_83e0 != '\0') {
      return DAT_ram_83e0;
    }
    DAT_ram_83e0 = 1;
    copyRunUpTileColumn(&UNK_ram_aa51,&UNK_ram_2f6e);
    uVar8 = (ushort)DAT_ram_83de;
    writePackedBcdByte();
    if (DAT_ram_83fe != '\0') {
      if (DAT_ram_83fd == '\x01') {
        pbVar10 = (byte *)&DAT_ram_83ed;
      }
      else {
        pbVar10 = (byte *)&DAT_ram_83eb;
      }
      bVar12 = (byte)uVar8;
      bVar3 = *pbVar10;
      bVar2 = ((bVar12 & 0xf) + (bVar3 & 0xf) & 0x10) != 0;
      bVar4 = BCDadjust(bVar12 + bVar3,CARRY1(bVar12,bVar3),bVar2);
      bVar3 = BCDadjustCarry(bVar4,CARRY1(bVar12,bVar3),bVar2);
      hasEvenParity(bVar4);
      *pbVar10 = bVar4;
      bVar9 = (byte)(uVar8 >> 8);
      bVar12 = pbVar10[1];
      bVar3 = bVar3 & 1;
      bVar2 = ((bVar9 & 0xf) + (bVar12 & 0xf) + bVar3 & 0x10) != 0;
      bVar1 = CARRY1(bVar9,bVar12) || CARRY1(bVar9 + bVar12,bVar3);
      bVar3 = BCDadjust(bVar9 + bVar12 + bVar3,bVar1,bVar2);
      BCDadjustCarry(bVar3,bVar1,bVar2);
      hasEvenParity(bVar3);
      pbVar10[1] = bVar3;
      uVar8 = CONCAT11(bVar3,bVar4);
      if (DAT_ram_83fd == '\x01') {
        puVar6 = &DAT_ram_83e7;
        cVar5 = DAT_ram_83e7;
      }
      else {
        puVar6 = &DAT_ram_83e8;
        cVar5 = DAT_ram_83e8;
      }
      if ((cVar5 == '\0') && ((DAT_ram_2e08 == uVar8 || (DAT_ram_2e08 < uVar8)))) {
        DAT_ram_83cf = cVar5;
        *puVar6 = 1;
        pcVar7 = (char *)CONCAT11((char)((ushort)puVar6 >> 8),(char)puVar6 + -2);
        cVar5 = *pcVar7 + '\x01';
        *pcVar7 = cVar5;
        puVar11 = &UNK_ram_abde;
        do {
          puVar11 = puVar11 + -0x20;
          cVar5 = cVar5 + -1;
        } while (cVar5 != '\0');
        *puVar11 = 0x4d;
        cVar5 = enqueueSoundCommand(7);
      }
      if (uVar8 <= DAT_ram_83ef) {
        return cVar5;
      }
      DAT_ram_83ef = uVar8;
      return cVar5;
    }
    return '\0';
  }
  DAT_ram_83dc = DAT_ram_83dc + -1;
  if (DAT_ram_83dc != '\0') {
    return '\0';
  }
  DAT_ram_83dc = 0x20;
  if (DAT_ram_83dd != '\0') {
    DAT_ram_83dd = DAT_ram_83dd + -1;
    bVar2 = ((DAT_ram_83de & 0xf) - 1 & 0x10) != 0;
    DAT_ram_83de = BCDadjust(DAT_ram_83de - 1,0,bVar2);
    BCDadjustCarry(DAT_ram_83de,0,bVar2);
    hasEvenParity(DAT_ram_83de);
    pbVar10 = (byte *)&DAT_ram_83dd;
    if (DAT_ram_83de == '\x10') {
      enqueueSoundCommand(5);
      DAT_ram_803f = 0;
    }
    bVar12 = *pbVar10;
    bVar3 = bVar12 & 3 ^ bVar12;
    cVar5 = '\x10' - (bVar12 & 3);
    (&DAT_ram_a8df)[(ushort)(byte)((bVar3 << 1 | bVar12 >> 7) << 1 | (bVar3 & 0x7f) >> 6) * 2] =
         cVar5;
    return cVar5;
  }
  copyRunUpTileColumn(&UNK_ram_aa51,&UNK_ram_2f6e);
  copyRunUpTileColumn(&UNK_ram_2f12);
  DAT_ram_8004 = 1;
  return '\x01';
}

