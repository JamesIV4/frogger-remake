
void blitScrollBand(void)

{
  byte bVar1;
  char cVar2;
  short sVar3;
  undefined1 *puVar4;
  
  puVar4 = &switchD_ram:0fbd::caseD_7f;
  bVar1 = 0;
  cVar2 = DAT_ram_827d;
  do {
    bVar1 = bVar1 + 0x20;
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  sVar3 = 0;
  cVar2 = DAT_ram_827e + -1;
  do {
    sVar3 = sVar3 + (ushort)switchD_ram:0fbd::caseD_7f + (ushort)bVar1;
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  cVar2 = '\x03';
  if (DAT_ram_8111 == '\0') {
LAB_ram_21df:
    do {
      FUN_ram_2219(&UNK_ram_2231);
      cVar2 = cVar2 + -1;
    } while (cVar2 != '\0');
    FUN_ram_2229();
    return;
  }
  if (DAT_ram_8111 != '0') {
    if (DAT_ram_8111 == 'P') {
      do {
        FUN_ram_2219(&UNK_ram_2239);
        cVar2 = cVar2 + -1;
      } while (cVar2 != '\0');
      DAT_ram_8108 = 1;
      FUN_ram_2229();
      return;
    }
    if (DAT_ram_8111 != '`') {
      if (DAT_ram_8111 != 'p') {
        FUN_ram_2229(&UNK_ram_a80e + sVar3);
        return;
      }
      goto LAB_ram_21df;
    }
  }
  do {
    FUN_ram_2219(&UNK_ram_2235);
    cVar2 = cVar2 + -1;
  } while (cVar2 != '\0');
  if (DAT_ram_8108 != '\0') {
    DAT_ram_8108 = 0;
    FUN_ram_2229();
    return;
  }
  DAT_ram_8119 = puVar4[2] + -1;
  return;
}

