
void scanCoinInputAndCredit(void)

{
  byte bVar1;
  short sVar2;
  byte *pbVar3;
  
  pbVar3 = &DAT_ram_83e2;
  if (DAT_ram_83e2 == '\0') {
    DAT_ram_83e2 = ~DAT_ram_e000 & 0xc4;
    return;
  }
  if ((~DAT_ram_e000 & 0xc4) != 0) {
    return;
  }
  issueSoundCommand(1);
  sVar2 = DAT_ram_83d4;
  if ((*pbVar3 & 0x40) == 0) {
    bVar1 = *pbVar3;
    *pbVar3 = 0;
    if ((bVar1 & 4) == 0) {
      DAT_ram_b818 = 1;
      DAT_ram_837e = 4;
    }
                    /* WARNING: Could not recover jumptable at 0x2d22. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*(dispatch_2d23 + sVar2))();
    return;
  }
  *pbVar3 = 0;
  DAT_ram_b81c = 1;
  DAT_ram_837f = 4;
                    /* WARNING: Could not recover jumptable at 0x2d39. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(dispatch_2d3a + sVar2))();
  return;
}

