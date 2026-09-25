
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void runIntroTimerThenInitGame(void)

{
  undefined2 uVar1;
  short sVar2;
  undefined1 *puVar3;
  undefined2 *puVar4;
  undefined1 *puVar5;
  undefined *puVar6;
  
  blitGameOverLine();
  enqueueSoundCommand(0xc);
  enqueueSoundCommand(0xd);
  do {
    DAT_ram_83c5 = DAT_ram_83c5 + -1;
  } while (DAT_ram_83c5 != 0);
  if (DAT_ram_83fe == '\x01') {
    DAT_ram_825c = 0;
    puVar5 = &DAT_ram_825e;
    puVar3 = &DAT_ram_825f;
    sVar2 = 4;
    DAT_ram_825e = 0;
    DAT_ram_83c5 = 0;
    do {
      *puVar3 = *puVar5;
      puVar3 = puVar3 + 1;
      puVar5 = puVar5 + 1;
      sVar2 = sVar2 + -1;
      uVar1 = DAT_ram_83c5;
    } while (sVar2 != 0);
  }
  else {
    if (DAT_ram_83fd == '\x01') {
      DAT_ram_83c9 = 1;
      if (DAT_ram_83ca != '\0') {
        DAT_ram_825c = 0;
        puVar5 = &DAT_ram_825e;
        puVar3 = &DAT_ram_825f;
        sVar2 = 4;
        DAT_ram_825e = 0;
        DAT_ram_83c5 = 0;
        do {
          *puVar3 = *puVar5;
          puVar3 = puVar3 + 1;
          puVar5 = puVar5 + 1;
          sVar2 = sVar2 + -1;
        } while (sVar2 != 0);
        coldStartClearPlayRamAndSetMode();
        return;
      }
      clearTilemapToTile16();
      handOffToOtherPlayer();
      DAT_ram_83fe = 1;
      DAT_ram_825c = 1;
      puVar5 = &DAT_ram_825e;
      puVar3 = &DAT_ram_825f;
      sVar2 = 4;
      DAT_ram_825e = 0;
      do {
        *puVar3 = *puVar5;
        puVar3 = puVar3 + 1;
        puVar5 = puVar5 + 1;
        sVar2 = sVar2 + -1;
      } while (sVar2 != 0);
      puVar6 = &UNK_ram_8600;
      puVar3 = &DAT_ram_80ff;
      sVar2 = 0xb7;
      do {
        *puVar3 = *puVar6;
        puVar3 = puVar3 + 1;
        puVar6 = puVar6 + 1;
        sVar2 = sVar2 + -1;
      } while (sVar2 != 0);
      puVar5 = &DAT_ram_85c0;
      puVar3 = &switchD_ram:14c6::caseD_1e;
      sVar2 = 0x2b;
      do {
        *puVar3 = *puVar5;
        puVar3 = puVar3 + 1;
        puVar5 = puVar5 + 1;
        sVar2 = sVar2 + -1;
      } while (sVar2 != 0);
      DAT_ram_803f = 1;
      endForegroundPassAtPaceTail();
      return;
    }
    DAT_ram_83ca = '\x01';
    uVar1 = 0;
    if (DAT_ram_83c9 == '\0') {
      clearTilemapToTile16();
      handOffToOtherPlayer();
      DAT_ram_83fe = 1;
      DAT_ram_825d = 1;
      puVar5 = &DAT_ram_8263;
      puVar3 = &DAT_ram_8264;
      sVar2 = 4;
      DAT_ram_8263 = 0;
      do {
        *puVar3 = *puVar5;
        puVar3 = puVar3 + 1;
        puVar5 = puVar5 + 1;
        sVar2 = sVar2 + -1;
      } while (sVar2 != 0);
      puVar6 = &UNK_ram_86c0;
      puVar3 = &switchD_ram:14c6::caseD_1e;
      sVar2 = 0x2b;
      do {
        *puVar3 = *puVar6;
        puVar3 = puVar3 + 1;
        puVar6 = puVar6 + 1;
        sVar2 = sVar2 + -1;
      } while (sVar2 != 0);
      DAT_ram_803f = 1;
      puVar5 = &DAT_ram_8500;
      puVar3 = &DAT_ram_80ff;
      sVar2 = 0xb7;
      do {
        *puVar3 = *puVar5;
        puVar3 = puVar3 + 1;
        puVar5 = puVar5 + 1;
        sVar2 = sVar2 + -1;
      } while (sVar2 != 0);
      endForegroundPassAtPaceTail();
      return;
    }
  }
  DAT_ram_83c5 = uVar1;
  DAT_ram_825d = 0;
  puVar5 = &DAT_ram_8263;
  puVar3 = &DAT_ram_8264;
  sVar2 = 4;
  DAT_ram_8263 = 0;
  do {
    *puVar3 = *puVar5;
    puVar3 = puVar3 + 1;
    puVar5 = puVar5 + 1;
    sVar2 = sVar2 + -1;
  } while (sVar2 != 0);
  clearTilemapToTile16();
  clearActivePlayerWorkRam();
  renderCreditLine();
  packScoreRankPair();
  renderScoreHeader();
  puVar5 = &switchD_ram:14c6::caseD_1a;
  puVar3 = &DAT_ram_8101;
  sVar2 = 0x15f;
  switchD_ram:14c6::caseD_1a = 0;
  do {
    *puVar3 = *puVar5;
    puVar3 = puVar3 + 1;
    puVar5 = puVar5 + 1;
    sVar2 = sVar2 + -1;
  } while (sVar2 != 0);
  puVar3 = &switchD_ram:0fbd::caseD_36;
  puVar4 = &switchD_ram:0fbd::caseD_40;
  sVar2 = 4;
  switchD_ram:0fbd::caseD_36 = 0;
  do {
    *(undefined1 *)puVar4 = *puVar3;
    puVar4 = (undefined2 *)((short)puVar4 + 1);
    puVar3 = puVar3 + 1;
    sVar2 = sVar2 + -1;
  } while (sVar2 != 0);
  puVar5 = &switchD_ram:14c6::caseD_1e;
  puVar3 = &DAT_ram_800d;
  sVar2 = 0x2e;
  switchD_ram:14c6::caseD_1e = 0;
  do {
    *puVar3 = *puVar5;
    puVar3 = puVar3 + 1;
    puVar5 = puVar5 + 1;
    sVar2 = sVar2 + -1;
  } while (sVar2 != 0);
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

