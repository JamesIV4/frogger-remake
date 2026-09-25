
void clearObjectBlocksAndMirrorToObjRam(void)

{
  short sVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  
  puVar3 = &switchD_ram:14c6::caseD_1e;
  puVar2 = &DAT_ram_800d;
  sVar1 = 0x2b;
  switchD_ram:14c6::caseD_1e = 0;
  do {
    *puVar2 = *puVar3;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  puVar3 = &switchD_ram:14c6::caseD_1e;
  puVar2 = &DAT_ram_b00c;
  sVar1 = 0x2b;
  do {
    *puVar2 = *puVar3;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  puVar3 = &switchD_ram:14c6::caseD_1a;
  puVar2 = &DAT_ram_8101;
  sVar1 = 0x62;
  switchD_ram:14c6::caseD_1a = 0;
  do {
    *puVar2 = *puVar3;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  return;
}

