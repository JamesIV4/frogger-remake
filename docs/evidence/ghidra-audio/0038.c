
/* WARNING: Removing unreachable block (ram,0x009d) */
/* WARNING: Removing unreachable block (ram,0x0084) */

void audio_0038(void)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  byte bVar5;
  char cVar6;
  ushort uVar7;
  undefined1 *puVar8;
  byte *pbVar9;
  undefined1 unaff_E_;
  
  bVar3 = audio_02c1(0xe,0x6d);
  if (bVar3 == 0) {
    cVar6 = '\x06';
    puVar8 = &DAT_ram_4040;
    do {
      *puVar8 = 0;
      puVar8 = puVar8 + 1;
      cVar6 = cVar6 + -1;
    } while (cVar6 != '\0');
    audio_0008(7);
    return;
  }
  uVar7 = CONCAT11(bVar3,unaff_E_);
  if (bVar3 == 0xff) {
    bVar3 = audio_0000(uVar7);
  }
  bVar4 = (byte)(uVar7 >> 8);
  if ((bVar4 != (bVar3 & 0xf)) && ((bVar3 & 0xf) != 0)) {
    FUN_ram_0097(bVar4);
    return;
  }
  bVar3 = bVar4 & 0xf;
  if ((uVar7 & 0xf00) == 0) {
    bVar4 = bVar4 + 0x12;
    bVar2 = bVar4 * '\x02';
    bVar5 = (bVar4 & 0x7f) >> 6;
    bVar1 = (bVar2 & 0x7f) >> 6;
    bVar3 = ((bVar2 | bVar4 >> 7) << 1 | bVar5) << 1 | bVar1;
    if ((bVar2 & 0x20) != 0) {
      audio_00e6(((bVar2 & 0x1f | bVar4 >> 7) << 1 | bVar5) << 1 | bVar1);
      return;
    }
  }
  DAT_ram_4046 = bVar3;
  audio_00e6();
  audio_00e6();
  bVar3 = audio_0102(DAT_ram_4040);
  bVar4 = audio_0102(DAT_ram_4042);
  DAT_ram_4049 = audio_0102(DAT_ram_4044);
  bVar5 = audio_0102(DAT_ram_4046);
  pbVar9 = &DAT_ram_4049;
  if (bVar4 <= bVar3) {
    bVar3 = bVar4;
  }
  if (DAT_ram_4049 <= bVar3) {
    bVar3 = DAT_ram_4049;
  }
  if (bVar3 < bVar5) {
    audio_008c();
    *pbVar9 = DAT_ram_4046;
    pbVar9[1] = 0;
    return;
  }
  return;
}

