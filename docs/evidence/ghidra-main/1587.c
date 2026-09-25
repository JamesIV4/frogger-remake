
void dispatch_1587(void)

{
  char cVar1;
  byte bVar2;
  char *pcVar3;
  byte *pbVar4;
  char *pcVar5;
  byte *pbVar6;
  
code_r0x1587:
  pbVar4 = &UNK_ram_81a5;
  pcVar3 = &switchD_ram:14c6::caseD_c4;
  pcVar5 = &switchD_ram:14c6::caseD_c8;
  pbVar6 = &switchD_ram:14c6::caseD_cc;
switchD_ram_14c6_caseD_36:
  bVar2 = *pbVar6;
  if (bVar2 == 0) {
    bVar2 = *pbVar4 & 0xf;
    if ((*pbVar4 & 0x10) != 0) goto LAB_ram_16e6;
  }
  else {
LAB_ram_16e6:
    if (bVar2 != 1) {
      *pbVar6 = bVar2 - 1;
      goto LAB_ram_167c;
    }
    bVar2 = 1;
  }
  cVar1 = *pcVar3;
  do {
    pcVar3 = pcVar3 + 1;
    *pcVar3 = *pcVar3 - bVar2;
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  cVar1 = *pcVar5;
  *pcVar5 = cVar1 - bVar2;
  pcVar5[2] = cVar1 - bVar2;
  if (DAT_ram_8047 < 0x73) {
    if ((DAT_ram_8047 & 0xf) < 3) {
      if (DAT_ram_80ff == (byte)((DAT_ram_8047 & 0xf0) - 0x30) >> 4) {
        DAT_ram_8044 = DAT_ram_8044 - bVar2;
        if ((DAT_ram_8044 < 8) || (0xe6 < DAT_ram_8044)) {
          DAT_ram_8004 = 1;
        }
      }
    }
    else if ((0xb < (DAT_ram_8047 & 0xf)) &&
            (DAT_ram_80ff == (byte)((DAT_ram_8047 & 0xf0) - 0x20) >> 4)) {
      DAT_ram_8044 = DAT_ram_8044 - bVar2;
    }
  }
  *pbVar6 = 0;
LAB_ram_167c:
  DAT_ram_80ff = DAT_ram_80ff + 1;
  if (10 < DAT_ram_80ff) {
    DAT_ram_80ff = 0;
    return;
  }
  do {
    switch(DAT_ram_80ff) {
    case 0:
      pbVar4 = &DAT_ram_819b;
      pcVar3 = &switchD_ram:14c6::caseD_1a;
      pcVar5 = &switchD_ram:14c6::caseD_1e;
      pbVar6 = &switchD_ram:14c6::caseD_22;
      break;
    case 1:
      goto dispatch_14ee;
    case 2:
      pbVar4 = &UNK_ram_819d;
      pcVar3 = &switchD_ram:14c6::caseD_3c;
      pcVar5 = &switchD_ram:14c6::caseD_40;
      pbVar6 = &switchD_ram:14c6::caseD_44;
      break;
    case 3:
      pbVar4 = &switchD_ram:14c6::caseD_4a;
      pcVar3 = &UNK_ram_811b;
      pcVar5 = &UNK_ram_8018;
      pbVar6 = &UNK_ram_81a9;
      break;
    case 4:
      pbVar4 = &UNK_ram_819f;
      pcVar3 = &switchD_ram:14c6::caseD_5e;
      pcVar5 = &switchD_ram:14c6::caseD_62;
      pbVar6 = &switchD_ram:14c6::caseD_66;
      goto switchD_ram_14c6_caseD_36;
    case 5:
      goto code_r0x15de;
    case 6:
      pbVar4 = &UNK_ram_81a1;
      pcVar3 = &switchD_ram:14c6::caseD_80;
      pcVar5 = &switchD_ram:14c6::caseD_84;
      pbVar6 = &switchD_ram:14c6::caseD_88;
      goto switchD_ram_14c6_caseD_36;
    case 7:
      pbVar4 = &switchD_ram:14c6::caseD_8e;
      pcVar3 = &switchD_ram:0fbd::caseD_b5;
      pcVar5 = &UNK_ram_8028;
      pbVar6 = &UNK_ram_81ad;
      break;
    case 8:
      pbVar4 = &UNK_ram_81a3;
      pcVar3 = &switchD_ram:14c6::caseD_a2;
      pcVar5 = &switchD_ram:14c6::caseD_a6;
      pbVar6 = &switchD_ram:14c6::caseD_aa;
      goto switchD_ram_14c6_caseD_36;
    case 9:
      pbVar4 = &switchD_ram:14c6::caseD_b0;
      pcVar3 = &switchD_ram:0fbd::caseD_d5;
      pcVar5 = &UNK_ram_8030;
      pbVar6 = &UNK_ram_81af;
      break;
    case 10:
      goto code_r0x1587;
    }
    bVar2 = *pbVar6;
    if (bVar2 == 0) {
      bVar2 = *pbVar4 & 0xf;
      if ((*pbVar4 & 0x10) != 0) goto switchD_ram_14c6_caseD_e2;
    }
    else {
switchD_ram_14c6_caseD_e2:
      if (bVar2 != 1) {
        *pbVar6 = bVar2 - 1;
        switchD_ram:14c6::caseD_6c();
        return;
      }
      bVar2 = 1;
    }
    cVar1 = *pcVar3;
    do {
      pcVar3 = pcVar3 + 1;
      *pcVar3 = *pcVar3 + bVar2;
      cVar1 = cVar1 + -1;
    } while (cVar1 != '\0');
    cVar1 = *pcVar5;
    *pcVar5 = cVar1 + bVar2;
    pcVar5[2] = cVar1 + bVar2;
    if ((0x2f < DAT_ram_8047) && (DAT_ram_8047 < 0x73)) {
      if ((DAT_ram_8047 & 0xf) < 3) {
        if ((DAT_ram_80ff == (byte)((DAT_ram_8047 & 0xf0) - 0x30) >> 4) && (0x2f < DAT_ram_8047)) {
          DAT_ram_8044 = DAT_ram_8044 + bVar2;
          if ((DAT_ram_8044 < 8) || (0xe6 < DAT_ram_8044)) {
            DAT_ram_8004 = 1;
          }
        }
      }
      else if ((0xb < (DAT_ram_8047 & 0xf)) &&
              (DAT_ram_80ff == (byte)((DAT_ram_8047 & 0xf0) - 0x20) >> 4)) {
        DAT_ram_8044 = DAT_ram_8044 + bVar2;
      }
    }
    *pbVar6 = 0;
code_r0x15de:
    DAT_ram_80ff = DAT_ram_80ff + 1;
    if (10 < DAT_ram_80ff) {
      DAT_ram_80ff = 0;
      return;
    }
  } while( true );
dispatch_14ee:
  pbVar4 = &switchD_ram:14c6::caseD_28;
  pcVar3 = &DAT_ram_8109;
  pcVar5 = &UNK_ram_8010;
  pbVar6 = &UNK_ram_81a7;
  goto switchD_ram_14c6_caseD_36;
}

