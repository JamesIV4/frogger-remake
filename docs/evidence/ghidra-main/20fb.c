
void stampScrollRevealColumn(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  undefined1 *puVar4;
  
  puVar4 = &switchD_ram:0fbd::caseD_4f;
  bVar1 = 0;
  cVar2 = DAT_ram_8274;
  do {
    bVar1 = bVar1 + 0x20;
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  sVar3 = 0;
  cVar2 = DAT_ram_8275 + -1;
  do {
    sVar3 = sVar3 + (ushort)switchD_ram:0fbd::caseD_4f + (ushort)bVar1;
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  cVar2 = '\x02';
  if (DAT_ram_8110 == 'P') {
LAB_ram_213e:
    do {
      FUN_ram_2178(&UNK_ram_2190);
      cVar2 = cVar2 + -1;
    } while (cVar2 != '\0');
    FUN_ram_2188();
    return;
  }
  if (DAT_ram_8110 != -0x80) {
    if (DAT_ram_8110 == -0x60) {
      do {
        FUN_ram_2178(&UNK_ram_2198);
        cVar2 = cVar2 + -1;
      } while (cVar2 != '\0');
      DAT_ram_8107 = 1;
      FUN_ram_2188();
      return;
    }
    if (DAT_ram_8110 != -0x50) {
      if (DAT_ram_8110 != -0x30) {
        FUN_ram_2188(&DAT_ram_a808 + sVar3);
        return;
      }
      goto LAB_ram_213e;
    }
  }
  do {
    FUN_ram_2178(&UNK_ram_2194);
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  if (DAT_ram_8107 != '\0') {
    DAT_ram_8107 = 0;
    FUN_ram_2188();
    return;
  }
  DAT_ram_811a = puVar4[2] + -1;
  return;
}

