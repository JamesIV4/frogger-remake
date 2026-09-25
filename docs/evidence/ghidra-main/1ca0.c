
void animateFrogHop(char *param_1)

{
  if (DAT_ram_8047 < 0x30) {
    return;
  }
  if (DAT_ram_8044 < 0x20) {
    return;
  }
  if (DAT_ram_8253 == '\0') {
    enqueueSoundCommand(4);
    if (param_1[1] == '!') goto LAB_ram_1ccf;
    DAT_ram_8045 = 0x21;
  }
  DAT_ram_8253 = DAT_ram_8253 + '\x01';
  if (DAT_ram_8253 == '\0') {
    return;
  }
LAB_ram_1ccf:
  DAT_ram_8253 = DAT_ram_8259;
  if (DAT_ram_824f != '\0') {
    return;
  }
  DAT_ram_824b = 1;
  DAT_ram_8253 = DAT_ram_8259 + -1;
  if (DAT_ram_8253 != '\0') {
    *param_1 = *param_1 - DAT_ram_8255;
    param_1[1] = '\x1f';
    return;
  }
  DAT_ram_824f = DAT_ram_8259;
  DAT_ram_824b = DAT_ram_8253;
  param_1[1] = '!';
  return;
}

