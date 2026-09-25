
void renderFrogSceneAndTickTimer(void)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  
  cVar2 = DAT_ram_83cd;
  DAT_ram_83ce = 0;
  if (DAT_ram_83cd == '\0') {
    DAT_ram_83cf = 0;
  }
  if (DAT_ram_83fe != '\0') {
    if (DAT_ram_83fd == '\x01') {
      pcVar3 = &DAT_ram_83e5;
    }
    else {
      pcVar3 = &DAT_ram_83e6;
    }
    if ((DAT_ram_83cd == '\0') &&
       (cVar1 = *pcVar3, *pcVar3 = cVar1 + -1, (char)(cVar1 + -1) == '\0')) {
      cVar2 = '\x01';
      DAT_ram_83cf = 1;
    }
    renderFrogAndArmObjects(cVar2);
    if (DAT_ram_83cd == '\0') {
      DAT_ram_83b5 = 1;
      blitFourTileGroupColumn(&UNK_ram_a850);
    }
    DAT_ram_83b5 = DAT_ram_826c ^ 1;
    if ((char)(DAT_ram_83fd + -1) == '\0') {
      renderFilledHomeSlots(0,&DAT_ram_825e);
    }
    else {
      renderFilledHomeSlots(DAT_ram_83fd + -1,&DAT_ram_8263);
    }
    if (DAT_ram_825a != '\0') {
      loadActivePlayerLaneParams();
      dispatchFrogAnimationArm();
      DAT_ram_825a = '\0';
    }
    DAT_ram_8044 = 0x80;
    DAT_ram_8045 = 0x1e;
    DAT_ram_8046 = 3;
    DAT_ram_8047 = 0xe0;
    DAT_ram_83cd = 0;
    DAT_ram_842d = 0;
    DAT_ram_842c = 0;
    DAT_ram_8269 = 0;
    DAT_ram_83c3 = 1;
    return;
  }
  if (DAT_ram_83cd == '\0') {
    return;
  }
  resetFrogObject(DAT_ram_83cd);
  return;
}

