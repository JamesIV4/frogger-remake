
void animateFrogHop(char *param_1)

{
  if (DAT_ram_8047 < 0x30) {
    return;
  }
  if (0xdf < DAT_ram_8044) {
    return;
  }
  if (DAT_ram_8252 == '\0') {
    enqueueSoundCommand(4);
    if (param_1[1] == -0x5f) goto LAB_ram_1c70;
    DAT_ram_8045 = 0xa1;
  }
  DAT_ram_8252 = DAT_ram_8252 + '\x01';
  if (DAT_ram_8252 == '\0') {
    return;
  }
LAB_ram_1c70:
  DAT_ram_8252 = DAT_ram_8258;
  if (DAT_ram_824e != '\0') {
    return;
  }
  DAT_ram_824a = 1;
  DAT_ram_8252 = DAT_ram_8258 + -1;
  if (DAT_ram_8252 != '\0') {
    *param_1 = *param_1 + DAT_ram_8255;
    param_1[1] = -0x61;
    return;
  }
  DAT_ram_824e = DAT_ram_8258;
  DAT_ram_824a = DAT_ram_8252;
  param_1[1] = -0x5f;
  return;
}

