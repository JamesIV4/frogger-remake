
/* WARNING: Removing unreachable block (ram,0x2d5b) */

char FUN_ram_2d49(void)

{
  bool bVar1;
  bool bVar2;
  byte bVar3;
  char cVar4;
  short sVar5;
  undefined1 *puVar6;
  undefined1 *puVar7;
  
  bVar1 = ((DAT_ram_83e1 & 0xf) + 1 & 0x10) != 0;
  bVar2 = 0xfe < DAT_ram_83e1;
  DAT_ram_83e1 = BCDadjust(DAT_ram_83e1 + 1,bVar2,bVar1);
  bVar3 = BCDadjustCarry(DAT_ram_83e1,bVar2,bVar1);
  hasEvenParity(DAT_ram_83e1);
  if ((bVar3 & 1) != 0) {
    DAT_ram_83e1 = 0x99;
  }
  if (DAT_ram_83fe == '\0') {
    if (DAT_ram_83d6 == '\x05') {
      blitPlayerSelectPrompt();
    }
    DAT_ram_83d6 = 5;
    DAT_ram_83d8 = 0;
    puVar7 = &DAT_ram_8040;
    puVar6 = &DAT_ram_8041;
    sVar5 = 0x1f;
    DAT_ram_8040 = 0;
    do {
      *puVar6 = *puVar7;
      puVar6 = puVar6 + 1;
      puVar7 = puVar7 + 1;
      sVar5 = sVar5 + -1;
    } while (sVar5 != 0);
    cVar4 = renderCreditLine();
    return cVar4;
  }
  return DAT_ram_83fe;
}

