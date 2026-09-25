
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined1
serviceVblankNmi(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
                undefined2 param_5)

{
  byte bVar1;
  byte bVar2;
  undefined2 in_AF;
  char cVar4;
  char cVar5;
  short sVar3;
  byte *pbVar6;
  undefined1 uVar8;
  undefined1 *puVar7;
  byte *pbVar9;
  undefined1 uVar11;
  undefined1 *puVar10;
  undefined1 uStack_1;
  
  DAT_ram_b808 = 0;
  scanCoinInputAndCredit(param_4,param_5,param_1,param_3,param_2);
  DAT_ram_b007 = DAT_ram_8007;
  pbVar9 = &DAT_ram_8008;
  pbVar6 = &DAT_ram_b008;
  cVar4 = '\x1c';
  do {
    bVar1 = *pbVar9 >> 1;
    bVar2 = (byte)(bVar1 | *pbVar9 << 7) >> 1;
    bVar1 = (byte)(bVar2 | bVar1 << 7) >> 1;
    *pbVar6 = (byte)(bVar1 | bVar2 << 7) >> 1 | bVar1 << 7;
    uVar11 = (undefined1)((ushort)pbVar9 >> 8);
    uVar8 = (undefined1)((ushort)pbVar6 >> 8);
    *(undefined1 *)CONCAT11(uVar8,(char)pbVar6 + '\x01') =
         *(undefined1 *)CONCAT11(uVar11,(char)pbVar9 + '\x01');
    pbVar9 = (byte *)CONCAT11(uVar11,(char)pbVar9 + '\x02');
    pbVar6 = (byte *)CONCAT11(uVar8,(char)pbVar6 + '\x02');
    cVar4 = cVar4 + -1;
  } while (cVar4 != '\0');
  cVar4 = '\b';
  if (DAT_ram_842f != '\0') {
    cVar4 = '\x06';
    pbVar6 = (byte *)CONCAT11(uVar8,0x48);
    pbVar9 = (byte *)CONCAT11(uVar11,0x48);
  }
  do {
    bVar1 = *pbVar9 >> 1;
    bVar2 = (byte)(bVar1 | *pbVar9 << 7) >> 1;
    bVar1 = (byte)(bVar2 | bVar1 << 7) >> 1;
    *pbVar6 = (byte)(bVar1 | bVar2 << 7) >> 1 | bVar1 << 7;
    pbVar9 = (byte *)CONCAT11((char)((ushort)pbVar9 >> 8),(char)pbVar9 + '\x01');
    pbVar6 = (byte *)CONCAT11((char)((ushort)pbVar6 >> 8),(char)pbVar6 + '\x01');
    cVar5 = '\x03';
    do {
      *pbVar6 = *pbVar9;
      pbVar9 = (byte *)CONCAT11((char)((ushort)pbVar9 >> 8),(char)pbVar9 + '\x01');
      pbVar6 = (byte *)CONCAT11((char)((ushort)pbVar6 >> 8),(char)pbVar6 + '\x01');
      cVar5 = cVar5 + -1;
    } while (cVar5 != '\0');
    cVar4 = cVar4 + -1;
  } while (cVar4 != '\0');
  if ((DAT_ram_837f != '\0') && (DAT_ram_837f = DAT_ram_837f + -1, DAT_ram_837f == '\0')) {
    DAT_ram_b81c = 0;
  }
  if ((DAT_ram_837e != '\0') && (DAT_ram_837e = DAT_ram_837e + -1, DAT_ram_837e == '\0')) {
    DAT_ram_b818 = 0;
  }
  if (((((DAT_ram_e004 & 8) != 0) && (DAT_ram_83fe != '\0')) && (DAT_ram_83fd != '\0')) &&
     (DAT_ram_83fd != '\x01')) {
    DAT_ram_b043 = DAT_ram_8043 + '\x02';
    DAT_ram_b047 = DAT_ram_8047 + '\x02';
  }
  if (DAT_ram_83fe == '\0') {
    if (DAT_ram_83d6 < 2) {
      if (DAT_ram_83d6 == 0) {
        driveAttractDemoSequencer();
      }
      driveInPlayFrameUpdate();
      DAT_ram_83cd = 0;
      DAT_ram_83cf = 0;
      DAT_ram_83b5 = '\0';
      _DAT_ram_8293 = 0;
      puVar10 = &DAT_ram_825c;
      puVar7 = &DAT_ram_825d;
      sVar3 = 0xb;
      DAT_ram_825c = '\0';
      do {
        *puVar7 = *puVar10;
        puVar7 = puVar7 + 1;
        puVar10 = puVar10 + 1;
        sVar3 = sVar3 + -1;
      } while (sVar3 != 0);
      UNK_ram_83af = 0x80;
      UNK_ram_83b0 = 0;
      UNK_ram_83b1 = 0;
    }
    else if (((DAT_ram_83d8 != '\0') && (DAT_ram_83d8 = DAT_ram_83d8 + -1, DAT_ram_83d8 == '\0')) &&
            (DAT_ram_83d7 == '\0')) {
      DAT_ram_83d6 = DAT_ram_83d6 - 1;
    }
  }
  else {
    dequeueSoundCommand();
    if (DAT_ram_83ea != '\0') {
      if ((char)((ushort)DAT_ram_83d2 >> 8) == '\0' && (char)DAT_ram_83d2 == '\0') {
        if ((char)((ushort)DAT_ram_8382 >> 8) != '\0' || (char)DAT_ram_8382 != '\0') {
          DAT_ram_8382 = DAT_ram_8382 + -1;
          if (DAT_ram_8382 == 0) {
            enqueueSoundCommand(0xf);
            enqueueSoundCommand(0xb0);
            DAT_ram_8371 = 0;
          }
        }
        if (DAT_ram_83fd == '\x01') {
          if (DAT_ram_825c == '\x05') {
            puVar10 = &DAT_ram_825e;
            puVar7 = &DAT_ram_825f;
            sVar3 = 4;
            DAT_ram_825e = 0;
            do {
              *puVar7 = *puVar10;
              puVar7 = puVar7 + 1;
              puVar10 = puVar10 + 1;
              sVar3 = sVar3 + -1;
            } while (sVar3 != 0);
            DAT_ram_825c = '\0';
            armBoardCompleteReveal();
            goto LAB_ram_0245;
          }
        }
        else if (DAT_ram_825d == '\x05') {
          puVar10 = &DAT_ram_8263;
          puVar7 = &DAT_ram_8264;
          sVar3 = 4;
          DAT_ram_8263 = 0;
          do {
            *puVar7 = *puVar10;
            puVar7 = puVar7 + 1;
            puVar10 = puVar10 + 1;
            sVar3 = sVar3 + -1;
          } while (sVar3 != 0);
          DAT_ram_825d = '\0';
          armBoardCompleteReveal();
          goto LAB_ram_0245;
        }
        if (DAT_ram_8298 == '\0') {
          if (DAT_ram_8297 == '\0') {
            if ((char)((ushort)DAT_ram_829d >> 8) == '\0' && (char)DAT_ram_829d == '\0') {
              driveScoreDisplayCountdown();
              orchestrateCollisionsAndFrogInput();
              if (DAT_ram_83b5 == '\0') {
                DAT_ram_83b5 = '\x01';
                DAT_ram_8384 = -1;
                if (DAT_ram_8380 != '\0') {
                  DAT_ram_8380 = '\0';
                  DAT_ram_8382 = 0x40;
                  copyRunUpTileColumn(&UNK_ram_aa51,&UNK_ram_2f7b);
                }
              }
            }
          }
          else {
            DAT_ram_8297 = DAT_ram_8297 + -1;
          }
        }
        else {
          DAT_ram_8298 = DAT_ram_8298 + -1;
        }
        if (DAT_ram_8384 != '\0') {
          DAT_ram_8384 = DAT_ram_8384 + -1;
          if (DAT_ram_8384 == '\0') {
            blitFourTileGroupColumn(&UNK_ram_a850);
          }
        }
        advanceScrollLaneObjects();
        advanceAnimationFrameBuffer();
        if (DAT_ram_8107 != '\0') {
          DAT_ram_8109 = DAT_ram_8109 + -1;
        }
        if (DAT_ram_8108 != '\0') {
          switchD_ram:14c6::caseD_5e = switchD_ram:14c6::caseD_5e + -1;
        }
        dispatchFrogMoveAgainstLanes();
        if (DAT_ram_8107 != '\0') {
          DAT_ram_8109 = DAT_ram_8109 + '\x01';
        }
        if (DAT_ram_8108 != '\0') {
          switchD_ram:14c6::caseD_5e = switchD_ram:14c6::caseD_5e + '\x01';
        }
        driveFrogDeathAnimation();
        moveLaneObjectsAndCarryFrog();
        driveSpriteObjectCluster();
        tickGatedCountdown();
        tickFrogRespawnDelay();
        if (DAT_ram_8297 != '\0') {
          stampHomeBayFrogByColumn();
        }
      }
      else {
        DAT_ram_83d2 = DAT_ram_83d2 + -1;
        moveLaneObjectsAndCarryFrog();
        advanceAnimationFrameBuffer();
      }
    }
  }
LAB_ram_0245:
  uStack_1 = (undefined1)((ushort)in_AF >> 8);
  DAT_ram_b808 = 1;
  return uStack_1;
}

