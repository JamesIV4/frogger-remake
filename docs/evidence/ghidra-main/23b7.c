
void advanceAttractDemoFrogHop(void)

{
  char cVar1;
  char cVar2;
  char cVar3;
  char *pcVar4;
  undefined1 *puVar5;
  
  cVar3 = DAT_ram_8253;
  cVar2 = DAT_ram_8252;
  cVar1 = DAT_ram_8250;
  puVar5 = &DAT_ram_8044;
  pcVar4 = &DAT_ram_8047;
  if (DAT_ram_8248 != '\0') {
    if (DAT_ram_824c != '\0') {
      return;
    }
    DAT_ram_8248 = 1;
    DAT_ram_8250 = DAT_ram_8250 + -1;
    if (DAT_ram_8250 == '\0') {
      DAT_ram_8248 = DAT_ram_8250;
      DAT_ram_824c = cVar1;
      DAT_ram_8045 = 0xde;
      return;
    }
    DAT_ram_8047 = DAT_ram_8254 + DAT_ram_8047;
    DAT_ram_8045 = 0xdc;
    return;
  }
  DAT_ram_824c = DAT_ram_8248;
  if (DAT_ram_8249 != '\0') {
    DAT_ram_824c = DAT_ram_8248;
    advanceHomeBaySlotCursor();
    if (DAT_ram_824d != '\0') {
      return;
    }
    DAT_ram_8249 = 1;
    cVar1 = DAT_ram_8251 + -1;
    if (cVar1 == '\0') {
      DAT_ram_824d = DAT_ram_8251;
      DAT_ram_8249 = cVar1;
      DAT_ram_8251 = cVar1;
      puVar5[1] = 0x1e;
      scoreFrogRowProgress(pcVar4);
      return;
    }
    DAT_ram_8251 = cVar1;
    *pcVar4 = *pcVar4 - DAT_ram_8254;
    puVar5[1] = 0x1c;
    return;
  }
  DAT_ram_824d = DAT_ram_8249;
  if (DAT_ram_824a != '\0') {
    if (DAT_ram_824e != '\0') {
      return;
    }
    DAT_ram_824a = 1;
    DAT_ram_8252 = DAT_ram_8252 + -1;
    if (DAT_ram_8252 == '\0') {
      DAT_ram_824a = DAT_ram_8252;
      DAT_ram_824e = cVar2;
      DAT_ram_8045 = 0xa1;
      return;
    }
    DAT_ram_8044 = DAT_ram_8044 + DAT_ram_8255;
    DAT_ram_8045 = 0x9f;
    return;
  }
  DAT_ram_824e = DAT_ram_824a;
  if (DAT_ram_824b == '\0') {
    DAT_ram_824f = DAT_ram_824b;
    return;
  }
  if (DAT_ram_824f != '\0') {
    return;
  }
  DAT_ram_824b = 1;
  DAT_ram_8253 = DAT_ram_8253 + -1;
  if (DAT_ram_8253 == '\0') {
    DAT_ram_824b = DAT_ram_8253;
    DAT_ram_824f = cVar3;
    DAT_ram_8045 = 0x21;
    return;
  }
  DAT_ram_8044 = DAT_ram_8044 - DAT_ram_8255;
  DAT_ram_8045 = 0x1f;
  return;
}

