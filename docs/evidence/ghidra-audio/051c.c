
undefined1 audio_051c(void)

{
  byte bVar1;
  undefined1 uVar2;
  
  if (DAT_ram_4178 == '\x01') {
    if ((char)(DAT_ram_4175 + -1) != '\0') {
      DAT_ram_4175 = DAT_ram_4175 + -1;
      return 0;
    }
    DAT_ram_4175 = 5;
    DAT_ram_4178 = 3;
  }
  else {
    if (DAT_ram_4178 == '\x02') {
      if ((char)(DAT_ram_4175 + -1) != '\0') {
        DAT_ram_4175 = DAT_ram_4175 + -1;
        return 0;
      }
      DAT_ram_4175 = 6;
      DAT_ram_4176 = DAT_ram_4176 + -4;
      bVar1 = (byte)((ushort)DAT_ram_4176 >> 8);
      if ((bVar1 != 0) || (bVar1 = (byte)DAT_ram_4176, 0x2f < bVar1)) goto LAB_ram_054a;
      DAT_ram_4175 = 0x30;
      DAT_ram_4178 = 3;
      goto LAB_ram_0569;
    }
    if (DAT_ram_4178 != '\x03') {
      if (DAT_ram_4178 == '\x04') {
        if ((char)(DAT_ram_4175 + -1) != '\0') {
          DAT_ram_4175 = DAT_ram_4175 + -1;
          return 0;
        }
        DAT_ram_4175 = 4;
        DAT_ram_4176 = DAT_ram_4176 + 0x10;
        bVar1 = 0;
        if (((char)((ushort)DAT_ram_4176 >> 8) != '\0') &&
           (bVar1 = (byte)DAT_ram_4176, 0x7f < bVar1)) {
          uVar2 = audio_0505();
          return uVar2;
        }
      }
      else {
        if ((char)(DAT_ram_4175 + -1) != '\0') {
          DAT_ram_4175 = DAT_ram_4175 + -1;
          return 0;
        }
        DAT_ram_4175 = 8;
        DAT_ram_4176 = DAT_ram_4176 + -0x10;
        bVar1 = (byte)((ushort)DAT_ram_4176 >> 8);
        if ((bVar1 == 0) && (bVar1 = (byte)DAT_ram_4176, bVar1 < 0x38)) {
          DAT_ram_4175 = 0x20;
          DAT_ram_4178 = 1;
          goto LAB_ram_0569;
        }
      }
LAB_ram_054a:
      audio_0028(bVar1);
      return 0;
    }
    if ((char)(DAT_ram_4175 + -1) != '\0') {
      DAT_ram_4175 = DAT_ram_4175 + -1;
      return 0;
    }
    DAT_ram_4175 = 4;
    DAT_ram_4178 = 4;
  }
  DAT_ram_4176 = 0x60;
LAB_ram_0569:
  audio_0018();
  return 0;
}

