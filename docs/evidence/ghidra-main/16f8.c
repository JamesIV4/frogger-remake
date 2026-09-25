
char driveFrogDeathAnimation(void)

{
  char cVar1;
  short sVar2;
  undefined1 *puVar3;
  undefined1 *puVar4;
  
  if (DAT_ram_8004 == '\0') {
    return '\0';
  }
  if ((DAT_ram_8150 & 1) != 0) {
    DAT_ram_8118 = 1;
  }
  if (DAT_ram_8120 != '\0') {
    DAT_ram_8121 = DAT_ram_8120;
  }
  stampHomeBaySlot();
  clearLatchedCollision();
  cVar1 = DAT_ram_8247;
  DAT_ram_8247 = DAT_ram_8247 + '\x01';
  cVar1 = cVar1 + -0xf;
  if (cVar1 != '\0') {
    return cVar1;
  }
  DAT_ram_8247 = cVar1;
  DAT_ram_8046 = 7;
  DAT_ram_81b2 = DAT_ram_81b2 + '\x01';
  if (DAT_ram_829c == '\0') {
    if (DAT_ram_81b2 != '\x06') {
LAB_ram_1785:
      if (DAT_ram_829c != '\0') {
        if (DAT_ram_81b2 == '\x01') {
          DAT_ram_8045 = 0x22;
          DAT_ram_8382 = 0;
          enqueueSoundCommand();
          cVar1 = enqueueSoundCommand(2);
          return cVar1;
        }
        if (DAT_ram_81b2 == '\x02') {
          DAT_ram_8045 = 0x23;
          return '\0';
        }
        if (DAT_ram_81b2 == '\x03') {
          DAT_ram_8045 = 0x24;
          return '\0';
        }
        DAT_ram_8045 = 0x3c;
        DAT_ram_83ae = 0;
        DAT_ram_8110 = 0;
        DAT_ram_8107 = 0;
        DAT_ram_811a = 0;
        DAT_ram_8119 = 0;
        cVar1 = clearTwoPlayerFrameCells();
        DAT_ram_8382 = 0xd8;
        return cVar1;
      }
      if (DAT_ram_81b2 == '\x01') {
        DAT_ram_8045 = 0x39;
        DAT_ram_8382 = 0;
        enqueueSoundCommand();
        cVar1 = enqueueSoundCommand(3);
        return cVar1;
      }
      if (DAT_ram_81b2 == '\x02') {
        DAT_ram_8045 = 0x39;
        return '\0';
      }
      if (DAT_ram_81b2 == '\x03') {
        DAT_ram_8045 = 0x3a;
        return '\0';
      }
      if (DAT_ram_81b2 == '\x04') {
        DAT_ram_8045 = 0x3b;
        return '\0';
      }
      DAT_ram_8045 = 0x3c;
      DAT_ram_83ae = 0;
      cVar1 = clearTwoPlayerFrameCells();
      DAT_ram_8382 = 0xd8;
      return cVar1;
    }
  }
  else if (DAT_ram_81b2 != '\x05') goto LAB_ram_1785;
  activateFrogObject();
  DAT_ram_81b2 = 0;
  DAT_ram_8004 = 0;
  DAT_ram_8247 = 0;
  DAT_ram_8269 = 0;
  DAT_ram_829c = 0;
  puVar4 = &DAT_ram_8248;
  puVar3 = &DAT_ram_8249;
  sVar2 = 0xb;
  DAT_ram_8248 = 0;
  do {
    *puVar3 = *puVar4;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
    sVar2 = sVar2 + -1;
  } while (sVar2 != 0);
  DAT_ram_83ce = 1;
  cVar1 = DAT_ram_83d6 + -1;
  if (((char)(DAT_ram_83d6 + -1) == '\0') && (cVar1 = DAT_ram_83fe, DAT_ram_83fe == '\0')) {
    DAT_ram_83d6 = DAT_ram_83fe;
    DAT_ram_8299 = DAT_ram_83fe;
    DAT_ram_829a = DAT_ram_83fe;
    switchD_ram:0fbd::caseD_1d = DAT_ram_83fe;
  }
  return cVar1;
}

