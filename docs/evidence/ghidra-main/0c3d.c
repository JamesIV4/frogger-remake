
void placeScoreRankMarkers(void)

{
  char cVar1;
  undefined2 uVar2;
  undefined *puVar3;
  char cVar4;
  undefined1 uVar5;
  
  uVar5 = 0x80;
  puVar3 = &UNK_ram_3004;
  uVar2 = DAT_ram_83fb;
  stampRankMarkerIfPlaced();
  cVar4 = (char)((ushort)puVar3 >> 8);
  cVar1 = cVar4 - (char)((ushort)uVar2 >> 8);
  if (cVar1 == cVar4) {
    return;
  }
  *(char *)CONCAT11(uVar5,cVar1) = (char)puVar3;
  return;
}

