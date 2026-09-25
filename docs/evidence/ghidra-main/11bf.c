
char dispatchFrogMoveAgainstLanes(void)

{
  char cVar1;
  
  if (DAT_ram_83cd != '\0') {
    return DAT_ram_83cd;
  }
  if (DAT_ram_8004 != '\0') {
    return DAT_ram_8004;
  }
  if ((DAT_ram_8047 & 0xf) < 9) {
                    /* WARNING: Could not recover jumptable at 0x11e8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    cVar1 = (**(code **)(&DAT_ram_11e9 + (ushort)(DAT_ram_8047 >> 4) * 2))();
    return cVar1;
  }
  cVar1 = resolveFrogMoveAgainstLanes();
  return cVar1;
}

