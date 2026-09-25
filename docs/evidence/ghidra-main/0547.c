
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void coldStartClearSlotGates(void)

{
  short sVar1;
  undefined1 *puVar2;
  undefined2 *puVar3;
  undefined1 *puVar4;
  
  DAT_ram_825c = 0;
  puVar4 = &DAT_ram_825e;
  puVar2 = &DAT_ram_825f;
  sVar1 = 4;
  DAT_ram_825e = 0;
  do {
    *puVar2 = *puVar4;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  DAT_ram_825d = 0;
  puVar4 = &DAT_ram_8263;
  puVar2 = &DAT_ram_8264;
  sVar1 = 4;
  DAT_ram_8263 = 0;
  do {
    *puVar2 = *puVar4;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  clearTilemapToTile16();
  clearActivePlayerWorkRam();
  renderCreditLine();
  packScoreRankPair();
  renderScoreHeader();
  puVar4 = &switchD_ram:14c6::caseD_1a;
  puVar2 = &DAT_ram_8101;
  sVar1 = 0x15f;
  switchD_ram:14c6::caseD_1a = 0;
  do {
    *puVar2 = *puVar4;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  puVar2 = &switchD_ram:0fbd::caseD_36;
  puVar3 = &switchD_ram:0fbd::caseD_40;
  sVar1 = 4;
  switchD_ram:0fbd::caseD_36 = 0;
  do {
    *(undefined1 *)puVar3 = *puVar2;
    puVar3 = (undefined2 *)((short)puVar3 + 1);
    puVar2 = puVar2 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  puVar4 = &switchD_ram:14c6::caseD_1e;
  puVar2 = &DAT_ram_800d;
  sVar1 = 0x2e;
  switchD_ram:14c6::caseD_1e = 0;
  do {
    *puVar2 = *puVar4;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
    sVar1 = sVar1 + -1;
  } while (sVar1 != 0);
  DAT_ram_83c3 = 0;
  DAT_ram_83fe = 0;
  DAT_ram_83bf = 0;
  DAT_ram_83c9 = 0;
  DAT_ram_83ca = 0;
  DAT_ram_b810 = 0;
  DAT_ram_b80c = 0;
  _DAT_ram_8293 = 0;
  DAT_ram_83bb = 0;
  DAT_ram_83cb = 0;
  DAT_ram_83d8 = 0;
  DAT_ram_83c4 = 0;
  DAT_ram_83ba = 0;
  DAT_ram_8295 = 0;
  switchD_ram:0fbd::caseD_1d = 0;
  DAT_ram_83d6 = 3;
  forceClearPlayerWorkRam();
  endForegroundPassAtPaceTail();
  return;
}

