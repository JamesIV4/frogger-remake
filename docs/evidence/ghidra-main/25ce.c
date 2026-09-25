
char stampHomeBaySlot(void)

{
  undefined1 *puVar1;
  
  if (DAT_ram_8121 == '\x01') {
    if (DAT_ram_83fd == '\x01') {
      if (DAT_ram_825e != '\0') {
        return DAT_ram_825e;
      }
    }
    else if (DAT_ram_8263 != '\0') {
      return DAT_ram_8263;
    }
    puVar1 = &DAT_ram_ab64;
  }
  else if (DAT_ram_8121 == '\x02') {
    if (DAT_ram_83fd == '\x01') {
      if (DAT_ram_825f != '\0') {
        return DAT_ram_825f;
      }
    }
    else if (DAT_ram_8264 != '\0') {
      return DAT_ram_8264;
    }
    puVar1 = &DAT_ram_aaa4;
  }
  else if (DAT_ram_8121 == '\x03') {
    if (DAT_ram_83fd == '\x01') {
      if (DAT_ram_8260 != '\0') {
        return DAT_ram_8260;
      }
    }
    else if (DAT_ram_8265 != '\0') {
      return DAT_ram_8265;
    }
    puVar1 = &DAT_ram_a9e4;
  }
  else if (DAT_ram_8121 == '\x04') {
    if (DAT_ram_83fd == '\x01') {
      if (DAT_ram_8261 != '\0') {
        return DAT_ram_8261;
      }
    }
    else if (DAT_ram_8266 != '\0') {
      return DAT_ram_8266;
    }
    puVar1 = &DAT_ram_a924;
  }
  else {
    if (DAT_ram_8121 != '\x05') {
      return DAT_ram_8121;
    }
    if (DAT_ram_83fd == '\x01') {
      if (DAT_ram_8262 != '\0') {
        return DAT_ram_8262;
      }
    }
    else if (DAT_ram_8267 != '\0') {
      return DAT_ram_8267;
    }
    puVar1 = &DAT_ram_a864;
  }
  *puVar1 = 0x10;
  puVar1[1] = 0x10;
  puVar1[0x20] = 0x10;
  puVar1[0x21] = 0x10;
  if (DAT_ram_8004 == '\0') {
    DAT_ram_8121 = 0;
    DAT_ram_8120 = 0;
    return '\0';
  }
  return DAT_ram_8004;
}

