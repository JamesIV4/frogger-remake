
undefined1 dispatch_0235(undefined2 param_1)

{
  driveSpriteObjectCluster();
  tickGatedCountdown();
  tickFrogRespawnDelay();
  if (DAT_ram_8297 != '\0') {
    stampHomeBayFrogByColumn();
  }
  DAT_ram_b808 = 1;
  return (char)((ushort)param_1 >> 8);
}

