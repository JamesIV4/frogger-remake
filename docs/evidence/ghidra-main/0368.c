
/* WARNING: Removing unreachable block (ram,0x0393) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void endForegroundPassAtPaceTail(void)

{
  bool bVar1;
  bool bVar2;
  ushort uVar3;
  undefined2 uVar4;
  byte bVar5;
  short sVar6;
  undefined1 *puVar7;
  undefined *puVar8;
  undefined2 *puVar9;
  char cVar10;
  undefined1 *puVar11;
  undefined *puVar12;
  
  sVar6 = DAT_ram_83c7;
  do {
    while( true ) {
      do {
        uVar3 = (ushort)sVar6 >> 8;
        cVar10 = (char)sVar6;
        sVar6 = sVar6 + -1;
      } while ((char)uVar3 != '\0' || cVar10 != '\0');
      if (DAT_ram_83fe == 0) break;
setUpBoardOrContinueLife:
      if (DAT_ram_83ea == '\0') {
        if (DAT_ram_83cd == '\0') {
          if ((byte)(DAT_ram_83fe - 1) != '\0') {
            clearTilemapToTile16(DAT_ram_83fe - 1);
            swapInActivePlayerPages();
          }
          renderScoreHeader();
        }
        if (DAT_ram_826d != '\0') {
          advanceBoardForeground(DAT_ram_826d);
        }
        DAT_ram_83ea = renderFrogSceneAndTickTimer();
        renderTimeBar();
        DAT_ram_839e = 0x20;
        DAT_ram_839d = 0x10;
        DAT_ram_839c = 0x20;
        if (DAT_ram_83fe != 1) {
          raiseActivePlayerStartFlag();
        }
        DAT_ram_826d = 0;
        DAT_ram_83b6 = DAT_ram_83cd;
        renderLivesRow();
        endForegroundPassAtPaceTail();
        return;
      }
      renderScoreHeader(DAT_ram_83ea);
      sVar6 = DAT_ram_83c7;
      if (DAT_ram_83ce != '\0') {
        activateFrogObject(DAT_ram_83ce);
        clearActivePlayerWorkRam();
        DAT_ram_839a = 0;
        DAT_ram_839b = 0;
        DAT_ram_83cc = 0;
        DAT_ram_83ea = 0;
        puVar11 = &DAT_ram_83a0;
        puVar7 = &DAT_ram_83a1;
        sVar6 = 0xd;
        DAT_ram_83a0 = 0;
        do {
          *puVar7 = *puVar11;
          puVar7 = puVar7 + 1;
          puVar11 = puVar11 + 1;
          sVar6 = sVar6 + -1;
        } while (sVar6 != 0);
        enqueueSoundCommand(0x80);
        if (DAT_ram_83cf == '\0') {
          handOffToOtherPlayer();
          endForegroundPassAtPaceTail();
          return;
        }
        blitGameOverLine(DAT_ram_83cf);
        enqueueSoundCommand(0xc);
        enqueueSoundCommand(0xd);
        do {
          DAT_ram_83c5 = DAT_ram_83c5 + -1;
        } while (DAT_ram_83c5 != 0);
        if (DAT_ram_83fe == 1) {
          DAT_ram_825c = 0;
          puVar11 = &DAT_ram_825e;
          puVar7 = &DAT_ram_825f;
          sVar6 = 4;
          DAT_ram_825e = 0;
          DAT_ram_83c5 = 0;
          do {
            *puVar7 = *puVar11;
            puVar7 = puVar7 + 1;
            puVar11 = puVar11 + 1;
            sVar6 = sVar6 + -1;
            uVar4 = DAT_ram_83c5;
          } while (sVar6 != 0);
        }
        else {
          if (DAT_ram_83fd == '\x01') {
            DAT_ram_83c9 = 1;
            if (DAT_ram_83ca == '\0') {
              clearTilemapToTile16();
              handOffToOtherPlayer();
              DAT_ram_83fe = 1;
              DAT_ram_825c = 1;
              puVar11 = &DAT_ram_825e;
              puVar7 = &DAT_ram_825f;
              sVar6 = 4;
              DAT_ram_825e = 0;
              do {
                *puVar7 = *puVar11;
                puVar7 = puVar7 + 1;
                puVar11 = puVar11 + 1;
                sVar6 = sVar6 + -1;
              } while (sVar6 != 0);
              puVar8 = &UNK_ram_8600;
              puVar7 = &DAT_ram_80ff;
              sVar6 = 0xb7;
              do {
                *puVar7 = *puVar8;
                puVar7 = puVar7 + 1;
                puVar8 = puVar8 + 1;
                sVar6 = sVar6 + -1;
              } while (sVar6 != 0);
              puVar11 = &DAT_ram_85c0;
              puVar7 = &switchD_ram:14c6::caseD_1e;
              sVar6 = 0x2b;
              do {
                *puVar7 = *puVar11;
                puVar7 = puVar7 + 1;
                puVar11 = puVar11 + 1;
                sVar6 = sVar6 + -1;
              } while (sVar6 != 0);
              DAT_ram_803f = 1;
              endForegroundPassAtPaceTail();
              return;
            }
            DAT_ram_825c = 0;
            puVar11 = &DAT_ram_825e;
            puVar7 = &DAT_ram_825f;
            sVar6 = 4;
            DAT_ram_825e = 0;
            DAT_ram_83c5 = 0;
            do {
              *puVar7 = *puVar11;
              puVar7 = puVar7 + 1;
              puVar11 = puVar11 + 1;
              sVar6 = sVar6 + -1;
            } while (sVar6 != 0);
            coldStartClearPlayRamAndSetMode();
            return;
          }
          DAT_ram_83ca = '\x01';
          uVar4 = 0;
          if (DAT_ram_83c9 == '\0') {
            clearTilemapToTile16();
            handOffToOtherPlayer();
            DAT_ram_83fe = 1;
            DAT_ram_825d = 1;
            puVar11 = &DAT_ram_8263;
            puVar7 = &DAT_ram_8264;
            sVar6 = 4;
            DAT_ram_8263 = 0;
            do {
              *puVar7 = *puVar11;
              puVar7 = puVar7 + 1;
              puVar11 = puVar11 + 1;
              sVar6 = sVar6 + -1;
            } while (sVar6 != 0);
            puVar8 = &UNK_ram_86c0;
            puVar7 = &switchD_ram:14c6::caseD_1e;
            sVar6 = 0x2b;
            do {
              *puVar7 = *puVar8;
              puVar7 = puVar7 + 1;
              puVar8 = puVar8 + 1;
              sVar6 = sVar6 + -1;
            } while (sVar6 != 0);
            DAT_ram_803f = 1;
            puVar11 = &DAT_ram_8500;
            puVar7 = &DAT_ram_80ff;
            sVar6 = 0xb7;
            do {
              *puVar7 = *puVar11;
              puVar7 = puVar7 + 1;
              puVar11 = puVar11 + 1;
              sVar6 = sVar6 + -1;
            } while (sVar6 != 0);
            endForegroundPassAtPaceTail();
            return;
          }
        }
        DAT_ram_83c5 = uVar4;
        DAT_ram_825d = 0;
        puVar11 = &DAT_ram_8263;
        puVar7 = &DAT_ram_8264;
        sVar6 = 4;
        DAT_ram_8263 = 0;
        do {
          *puVar7 = *puVar11;
          puVar7 = puVar7 + 1;
          puVar11 = puVar11 + 1;
          sVar6 = sVar6 + -1;
        } while (sVar6 != 0);
        clearTilemapToTile16();
        clearActivePlayerWorkRam();
        renderCreditLine();
        packScoreRankPair();
        renderScoreHeader();
        puVar11 = &switchD_ram:14c6::caseD_1a;
        puVar7 = &DAT_ram_8101;
        sVar6 = 0x15f;
        switchD_ram:14c6::caseD_1a = 0;
        do {
          *puVar7 = *puVar11;
          puVar7 = puVar7 + 1;
          puVar11 = puVar11 + 1;
          sVar6 = sVar6 + -1;
        } while (sVar6 != 0);
        puVar7 = &switchD_ram:0fbd::caseD_36;
        puVar9 = &switchD_ram:0fbd::caseD_40;
        sVar6 = 4;
        switchD_ram:0fbd::caseD_36 = 0;
        do {
          *(undefined1 *)puVar9 = *puVar7;
          puVar9 = (undefined2 *)((short)puVar9 + 1);
          puVar7 = puVar7 + 1;
          sVar6 = sVar6 + -1;
        } while (sVar6 != 0);
        puVar11 = &switchD_ram:14c6::caseD_1e;
        puVar7 = &DAT_ram_800d;
        sVar6 = 0x2e;
        switchD_ram:14c6::caseD_1e = 0;
        do {
          *puVar7 = *puVar11;
          puVar7 = puVar7 + 1;
          puVar11 = puVar11 + 1;
          sVar6 = sVar6 + -1;
        } while (sVar6 != 0);
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
    }
    if (DAT_ram_83b3 == '\0') {
      if ((char)DAT_ram_e002 < '\0') {
        if ((bool)((DAT_ram_e002 & 0x7f) >> 6)) goto drainForegroundThenYieldEachVblank;
        bVar5 = 2;
      }
      else {
        bVar5 = 1;
      }
      if (bVar5 <= DAT_ram_83e1) {
        bVar1 = ((DAT_ram_83e1 & 0xf) - bVar5 & 0x10) != 0;
        bVar2 = DAT_ram_83e1 < bVar5;
        DAT_ram_83e1 = BCDadjust(DAT_ram_83e1 - bVar5,bVar2,bVar1);
        BCDadjustCarry(DAT_ram_83e1,bVar2,bVar1);
        hasEvenParity(DAT_ram_83e1);
        puVar11 = &DAT_ram_8500;
        puVar7 = &DAT_ram_8501;
        sVar6 = 0x1ff;
        DAT_ram_8500 = 0;
        DAT_ram_8370 = bVar5;
        do {
          *puVar7 = *puVar11;
          puVar7 = puVar7 + 1;
          puVar11 = puVar11 + 1;
          sVar6 = sVar6 + -1;
        } while (sVar6 != 0);
        DAT_ram_83fd = '\x01';
        DAT_ram_83b3 = '\x01';
        DAT_ram_83b7 = 1;
        _DAT_ram_83b8 = 0x101;
        DAT_ram_83fe = bVar5;
        initNewGameScoreAndTimers();
        DAT_ram_803d = 3;
        clearSoundQueue();
        DAT_ram_8071 = 0;
        enqueueSoundCommand();
        enqueueSoundCommand(9);
        enqueueSoundCommand(10);
        enqueueSoundCommand(0xb);
        DAT_ram_829d = 0x20;
        DAT_ram_8382 = 0x1a0;
        DAT_ram_83d2 = 0;
        clearActivePlayerWorkRam();
        clearTilemapToTile16();
        loadActivePlayerLaneParams();
        DAT_ram_842f = 0;
        DAT_ram_842d = 0;
        _DAT_ram_8293 = 0;
        puVar12 = &UNK_ram_8440;
        puVar8 = &UNK_ram_8441;
        sVar6 = 0x4f;
        UNK_ram_8440 = 0;
        do {
          *puVar8 = *puVar12;
          puVar8 = puVar8 + 1;
          puVar12 = puVar12 + 1;
          sVar6 = sVar6 + -1;
        } while (sVar6 != 0);
        DAT_ram_8004 = 0;
        DAT_ram_825a = 1;
        goto setUpBoardOrContinueLife;
      }
    }
drainForegroundThenYieldEachVblank:
    if (1 < DAT_ram_83d6) {
      dispatchGameModeFrame();
    }
    renderScoreHeader();
    if ((byte)(DAT_ram_83d6 - 1) != '\0') {
      renderCreditLine(DAT_ram_83d6 - 1);
    }
    setUpPlayStartOnce();
    DAT_ram_8254 = 2;
    DAT_ram_8255 = 2;
    DAT_ram_8256 = 9;
    DAT_ram_8257 = 9;
    DAT_ram_8258 = 9;
    DAT_ram_8259 = 9;
    sVar6 = DAT_ram_83c7;
  } while( true );
}

