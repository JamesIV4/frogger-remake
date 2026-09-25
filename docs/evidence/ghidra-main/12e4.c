
byte resolveFrogMoveAgainstLanes(void)

{
  byte bVar1;
  
  if (DAT_ram_8004 != 0) {
    return DAT_ram_8004;
  }
  bVar1 = DAT_ram_8047 + 0xfU & 0xf;
  if (4 < bVar1) {
                    /* WARNING: Could not recover jumptable at 0x130a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    bVar1 = (**(code **)(&DAT_ram_130b + (ushort)((byte)(DAT_ram_8047 + 0xfU) >> 4) * 2))();
    return bVar1;
  }
  return bVar1;
}

