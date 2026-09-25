
void switchD_ram:0ec2::dispatch_0ec3(void)

{
  undefined1 uVar1;
  byte bVar2;
  byte bVar3;
  byte *pbVar4;
  undefined1 *puVar5;
  
  pbVar4 = &DAT_ram_8058;
  bVar3 = 0xc1;
  uVar1 = FUN_ram_0f3e();
  *pbVar4 = *pbVar4 - 1;
  *pbVar4 = *pbVar4 - 1;
  *pbVar4 = *pbVar4 - 1;
  *pbVar4 = *pbVar4 - 1;
  bVar2 = *pbVar4;
  puVar5 = (undefined1 *)CONCAT11((char)((ushort)pbVar4 >> 8),(char)pbVar4 + '\x01');
  *puVar5 = uVar1;
  if (bVar3 <= bVar2) {
    return;
  }
  *puVar5 = 0x1e;
  DAT_ram_83d7 = DAT_ram_83d7 + -1;
  if (DAT_ram_83d7 != '\0') {
    return;
  }
  DAT_ram_83d7 = 0x14;
  DAT_ram_83bf = DAT_ram_83bf + '\x01';
  return;
}

