
byte awardHomeBayGoal(void)

{
  byte bVar1;
  byte bVar2;
  char cVar3;
  char *pcVar4;
  char *pcVar5;
  
  bVar2 = DAT_ram_8266;
  if (DAT_ram_83fd == '\x01') {
    bVar2 = DAT_ram_8261;
  }
  if (bVar2 != 0) {
    return bVar2;
  }
  if (DAT_ram_8047 < 0x2a) {
    if (DAT_ram_8121 == '\x04') {
      FUN_ram_2673(0);
    }
    stampHomeGoalAndResetFrog(&DAT_ram_a924);
    if (DAT_ram_8134 != '\0') {
      armHomeGoalSprite(DAT_ram_8134);
      DAT_ram_8134 = '\0';
    }
    if (DAT_ram_83fd != '\x01') {
      DAT_ram_8266 = 1;
      DAT_ram_825d = DAT_ram_825d + '\x01';
      return 1;
    }
    DAT_ram_8261 = 1;
    DAT_ram_825c = DAT_ram_825c + '\x01';
    return 1;
  }
  if (DAT_ram_826c != 0) {
    return DAT_ram_826c;
  }
  if (DAT_ram_8268 != '\0') {
    DAT_ram_8268 = DAT_ram_8268 + -1;
    bVar2 = advanceHomeBaySlotCursor();
    return bVar2;
  }
  if (DAT_ram_8004 != 0) {
    return DAT_ram_8004;
  }
  pcVar5 = (char *)&DAT_ram_8044;
  pcVar4 = (char *)&DAT_ram_8047;
  bVar2 = DAT_ram_e000;
  if (((DAT_ram_e004 & 8) != 0) && (DAT_ram_83fd != '\x01')) {
    bVar2 = DAT_ram_e002;
  }
  if (DAT_ram_8248 != '\0') goto animateFrogHop;
  if (((DAT_ram_e004 & 8) == 0) || (DAT_ram_83fd == '\x01')) {
    bVar1 = DAT_ram_e004 & 0x40;
  }
  else {
    bVar1 = DAT_ram_e004 & 1;
  }
  if (bVar1 != 0) {
    DAT_ram_824c = 0;
    DAT_ram_8250 = 0;
    cVar3 = DAT_ram_8249;
    if (DAT_ram_8249 != '\0') goto animateFrogHop;
    if ((char)(DAT_ram_824b + DAT_ram_824a) == '\0') {
      if (((DAT_ram_e004 & 8) == 0) || (DAT_ram_83fd == '\x01')) {
        bVar1 = DAT_ram_e004 & 0x10;
      }
      else {
        bVar1 = DAT_ram_e000 & 1;
      }
      if (bVar1 == 0) {
        if (DAT_ram_8251 == '\0') {
          enqueueSoundCommand(4);
          if (pcVar5[1] != '\x1e') {
            DAT_ram_8045 = 0x1e;
            goto LAB_ram_1bfa;
          }
        }
        else {
LAB_ram_1bfa:
          DAT_ram_8251 = DAT_ram_8251 + '\x01';
          if (DAT_ram_8251 == '\0') {
            return 0;
          }
        }
        DAT_ram_8251 = DAT_ram_8257;
        cVar3 = DAT_ram_8257;
animateFrogHop:
        advanceHomeBaySlotCursor(cVar3);
        if (DAT_ram_824d != 0) {
          return DAT_ram_824d;
        }
        DAT_ram_8249 = 1;
        cVar3 = DAT_ram_8251 + -1;
        if (cVar3 != '\0') {
          DAT_ram_8251 = cVar3;
          *pcVar4 = *pcVar4 - DAT_ram_8254;
          pcVar5[1] = '\x1c';
          return 0x1c;
        }
        DAT_ram_824d = DAT_ram_8251;
        DAT_ram_8249 = cVar3;
        DAT_ram_8251 = cVar3;
        pcVar5[1] = '\x1e';
        bVar2 = scoreFrogRowProgress(pcVar4);
        return bVar2;
      }
      DAT_ram_824d = 0;
      DAT_ram_8251 = '\0';
    }
    if (DAT_ram_824a != '\0') goto animateFrogHop;
    if ((bVar2 & 0x10) != 0) {
      DAT_ram_824e = 0;
      DAT_ram_8252 = 0;
      if (DAT_ram_824b != '\0') goto animateFrogHop;
      if ((bVar2 & 0x20) != 0) {
        DAT_ram_824f = 0;
        DAT_ram_8253 = 0;
        return 0;
      }
      if (DAT_ram_8047 < 0x30) {
        return DAT_ram_8047;
      }
      if (DAT_ram_8044 < 0x20) {
        return DAT_ram_8044;
      }
      if (DAT_ram_8253 == 0) {
        enqueueSoundCommand(4);
        if (pcVar5[1] != '!') {
          DAT_ram_8045 = 0x21;
          goto LAB_ram_1cc2;
        }
      }
      else {
LAB_ram_1cc2:
        DAT_ram_8253 = DAT_ram_8253 + 1;
        if (DAT_ram_8253 == '\0') {
          return 0;
        }
      }
      DAT_ram_8253 = DAT_ram_8259;
animateFrogHop:
      if (DAT_ram_824f != 0) {
        return DAT_ram_824f;
      }
      DAT_ram_824b = 1;
      cVar3 = DAT_ram_8253 - 1;
      if (cVar3 != '\0') {
        DAT_ram_8253 = cVar3;
        *pcVar5 = *pcVar5 - DAT_ram_8255;
        pcVar5[1] = '\x1f';
        return 0x1f;
      }
      bVar2 = DAT_ram_8253;
      DAT_ram_824f = DAT_ram_8253;
      DAT_ram_824b = cVar3;
      DAT_ram_8253 = cVar3;
      pcVar5[1] = '!';
      return bVar2;
    }
    if (DAT_ram_8047 < 0x30) {
      return DAT_ram_8047;
    }
    if (0xdf < DAT_ram_8044) {
      return DAT_ram_8044;
    }
    if (DAT_ram_8252 == 0) {
      enqueueSoundCommand(4);
      if (pcVar5[1] != -0x5f) {
        DAT_ram_8045 = 0xa1;
        goto LAB_ram_1c63;
      }
    }
    else {
LAB_ram_1c63:
      DAT_ram_8252 = DAT_ram_8252 + 1;
      if (DAT_ram_8252 == '\0') {
        return 0;
      }
    }
    DAT_ram_8252 = DAT_ram_8258;
animateFrogHop:
    if (DAT_ram_824e != 0) {
      return DAT_ram_824e;
    }
    DAT_ram_824a = 1;
    cVar3 = DAT_ram_8252 - 1;
    if (cVar3 != '\0') {
      DAT_ram_8252 = cVar3;
      *pcVar5 = *pcVar5 + DAT_ram_8255;
      pcVar5[1] = -0x61;
      return 0x9f;
    }
    bVar2 = DAT_ram_8252;
    DAT_ram_824e = DAT_ram_8252;
    DAT_ram_824a = cVar3;
    DAT_ram_8252 = cVar3;
    pcVar5[1] = -0x5f;
    return bVar2;
  }
  if (0xef < DAT_ram_8047) {
    return DAT_ram_8047;
  }
  if (DAT_ram_8250 == 0) {
    enqueueSoundCommand(4);
    if (pcVar5[1] != -0x22) {
      DAT_ram_8045 = 0xde;
      goto LAB_ram_1ba7;
    }
  }
  else {
LAB_ram_1ba7:
    DAT_ram_8250 = DAT_ram_8250 + 1;
    if (DAT_ram_8250 == '\0') {
      return 0;
    }
  }
  DAT_ram_8250 = DAT_ram_8256;
animateFrogHop:
  if (DAT_ram_824c != 0) {
    return DAT_ram_824c;
  }
  DAT_ram_8248 = 1;
  cVar3 = DAT_ram_8250 - 1;
  if (cVar3 != '\0') {
    DAT_ram_8250 = cVar3;
    *pcVar4 = DAT_ram_8254 + *pcVar4;
    pcVar5[1] = -0x24;
    return 0xdc;
  }
  bVar2 = DAT_ram_8250;
  DAT_ram_824c = DAT_ram_8250;
  DAT_ram_8248 = cVar3;
  DAT_ram_8250 = cVar3;
  pcVar5[1] = -0x22;
  return bVar2;
}

