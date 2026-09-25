
void renderMode3ScoreRankingScreen(undefined1 param_1)

{
  byte bVar1;
  char cVar3;
  undefined2 uVar2;
  char cVar4;
  char cVar5;
  undefined1 *puVar6;
  undefined2 *puVar7;
  undefined1 uVar8;
  char cVar9;
  short unaff_AF_;
  
  DAT_ram_83d8 = DAT_ram_83d8 + -1;
  DAT_ram_83b3 = 0;
  DAT_ram_83d7 = DAT_ram_83b3;
  fillTilemapBlock22x32();
  DAT_ram_8019 = 3;
  puVar6 = &DAT_ram_801f;
  cVar3 = '\x05';
  do {
    *puVar6 = 0;
    puVar6 = (undefined1 *)CONCAT11((char)((ushort)puVar6 >> 8),(char)puVar6 + '\x04');
    cVar3 = cVar3 + -1;
  } while (cVar3 != '\0');
  placeScoreRankMarkers();
  uVar2 = CONCAT11(0xd,param_1);
  copyRunUpTileColumn(&UNK_ram_aaac,&UNK_ram_2ee5);
  cVar3 = '\x01';
  do {
    uVar8 = (undefined1)uVar2;
    writeScoreDigitStepUp(CONCAT11(0xaa,cVar3 * '\x02' + -0x33));
    uVar2 = CONCAT11(3,uVar8);
    cVar9 = cVar3;
    bVar1 = copyRunUpTileColumn((char)((ushort)unaff_AF_ >> 8),uVar2);
    unaff_AF_ = (ushort)bVar1 << 8;
    puVar7 = &DAT_ram_83ef;
    cVar4 = cVar3;
    do {
      cVar5 = (char)puVar7;
      uVar8 = (undefined1)((ushort)puVar7 >> 8);
      puVar7 = (undefined2 *)CONCAT11(uVar8,cVar5 + '\x02');
      cVar4 = cVar4 + -1;
    } while (cVar4 != '\0');
    writeScoreField(CONCAT11(0xa9,cVar3 * '\x02' + -0x13),
                    CONCAT11(*(undefined1 *)CONCAT11(uVar8,cVar5 + '\x03'),*(undefined1 *)puVar7));
    copyRunUpTileColumn(&UNK_ram_2fba);
    cVar3 = cVar9 + '\x01';
  } while (cVar3 != '\x06');
  DAT_ram_8039 = 0;
  copyRunUpTileColumn(CONCAT11(0xf,(char)uVar2),&UNK_ram_aafc,&UNK_ram_2f4d);
  return;
}

