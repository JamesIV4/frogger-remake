
void advanceScrollLaneObjects(void)

{
  char cVar1;
  char cVar2;
  undefined *puVar3;
  undefined1 *puVar4;
  
  DAT_ram_811a = DAT_ram_8275;
  DAT_ram_8110 = DAT_ram_8110 + 1;
  if (0x4f < DAT_ram_8110) {
    stampScrollRevealColumn();
  }
  DAT_ram_8119 = DAT_ram_827e;
  DAT_ram_8111 = DAT_ram_8111 + 2;
  if (DAT_ram_8111 < 0xa0) {
    blitScrollBand();
  }
  DAT_ram_826e = DAT_ram_826e + '\x01';
  if (DAT_ram_826e == '\x10') {
    switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
    blitScrollTileGrid(DAT_ram_811a,&UNK_ram_1423);
    puVar3 = &UNK_ram_145f;
    cVar1 = DAT_ram_8119;
    cVar2 = DAT_ram_827d;
    puVar4 = switchD_ram:0fbd::caseD_83;
    switchD_ram:0fbd::caseD_40 = puVar3;
    DAT_ram_8003 = DAT_ram_827d;
    switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_7f;
  }
  else if (DAT_ram_826e == ' ') {
    switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
    blitScrollTileGrid(DAT_ram_811a,&UNK_ram_142b);
    puVar3 = &UNK_ram_1473;
    cVar1 = DAT_ram_8119;
    cVar2 = DAT_ram_827d;
    puVar4 = switchD_ram:0fbd::caseD_83;
    switchD_ram:0fbd::caseD_40 = puVar3;
    DAT_ram_8003 = DAT_ram_827d;
    switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_7f;
  }
  else {
    if (DAT_ram_826e != '0') {
      return;
    }
    switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_4f;
    DAT_ram_826e = '\0';
    blitScrollTileGrid(DAT_ram_811a,&UNK_ram_1433);
    puVar3 = &UNK_ram_1487;
    cVar1 = DAT_ram_8119;
    cVar2 = DAT_ram_827d;
    puVar4 = switchD_ram:0fbd::caseD_83;
    switchD_ram:0fbd::caseD_40 = puVar3;
    DAT_ram_8003 = DAT_ram_827d;
    switchD_ram:0fbd::caseD_5a = switchD_ram:0fbd::caseD_7f;
  }
  do {
    do {
      *puVar4 = *puVar3;
      puVar4[1] = puVar3[1];
      puVar4 = puVar4 + 0x20;
      cVar2 = cVar2 + -1;
      puVar3 = puVar3 + 2;
    } while (cVar2 != '\0');
    cVar1 = cVar1 + -1;
    cVar2 = DAT_ram_8003;
    puVar3 = switchD_ram:0fbd::caseD_40;
    puVar4 = puVar4 + switchD_ram:0fbd::caseD_5a;
  } while (cVar1 != '\0');
  return;
}

