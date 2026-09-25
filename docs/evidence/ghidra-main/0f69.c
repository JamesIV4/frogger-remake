
void packScoreRankPair(void)

{
  undefined1 uVar1;
  undefined1 uVar2;
  ushort uVar3;
  undefined2 uStack_2;
  
  if (DAT_ram_83eb < DAT_ram_83ed) {
    uStack_2 = DAT_ram_83eb;
    uVar3 = DAT_ram_83ed;
  }
  else {
    uStack_2 = DAT_ram_83ed;
    uVar3 = DAT_ram_83eb;
  }
  uVar1 = insertHighScoreEntry(DAT_ram_83eb - DAT_ram_83ed,uVar3);
  uVar2 = insertHighScoreEntry(uStack_2);
  DAT_ram_83fb = CONCAT11(uVar2,uVar1);
  return;
}

