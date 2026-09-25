
char insertHighScoreEntry(undefined2 param_1)

{
  byte bVar1;
  ushort uVar2;
  char cVar3;
  byte bVar4;
  byte bVar5;
  byte *pbVar6;
  undefined1 uVar8;
  byte *pbVar7;
  
  cVar3 = '\x05';
  pbVar6 = &DAT_ram_83f2;
  do {
    bVar5 = (byte)((ushort)param_1 >> 8);
    uVar8 = (undefined1)((ushort)pbVar6 >> 8);
    if (*pbVar6 <= bVar5) {
      bVar4 = (byte)param_1;
      if ((bVar5 != *pbVar6) || (bVar1 = *(byte *)CONCAT11(uVar8,(char)pbVar6 + -1), bVar1 < bVar4))
      {
        bVar1 = 0;
        if ((char)(cVar3 + -1) != '\0') {
          bVar1 = (cVar3 + -1) * '\x02';
          uVar2 = (ushort)bVar1;
          pbVar6 = &DAT_ram_83fa;
          pbVar7 = &DAT_ram_83f8;
          do {
            *pbVar6 = *pbVar7;
            pbVar6 = pbVar6 + -1;
            pbVar7 = pbVar7 + -1;
            uVar2 = uVar2 - 1;
          } while (uVar2 != 0);
        }
        *pbVar6 = bVar5;
        *(byte *)CONCAT11((char)((ushort)pbVar6 >> 8),(char)pbVar6 + -1) = bVar4;
LAB_ram_0aa5:
        return bVar1 * '\x02' + '\x01';
      }
      if ((bVar1 == bVar4) && (bVar1 = 0, cVar3 == '\x01')) goto LAB_ram_0aa5;
    }
    pbVar6 = (byte *)CONCAT11(uVar8,(char)pbVar6 + '\x02');
    cVar3 = cVar3 + -1;
    if (cVar3 == '\0') {
      return '\0';
    }
  } while( true );
}

