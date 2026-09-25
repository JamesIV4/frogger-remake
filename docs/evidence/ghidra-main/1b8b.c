
void animateFrogHop(short param_1,char *param_2)

{
  if (0xef < DAT_ram_8047) {
    return;
  }
  if (DAT_ram_8250 == '\0') {
    enqueueSoundCommand(4);
    if (*(char *)(param_1 + 1) == -0x22) goto LAB_ram_1bb4;
    DAT_ram_8045 = 0xde;
  }
  DAT_ram_8250 = DAT_ram_8250 + '\x01';
  if (DAT_ram_8250 == '\0') {
    return;
  }
LAB_ram_1bb4:
  DAT_ram_8250 = DAT_ram_8256;
  if (DAT_ram_824c != '\0') {
    return;
  }
  DAT_ram_8248 = 1;
  DAT_ram_8250 = DAT_ram_8256 + -1;
  if (DAT_ram_8250 != '\0') {
    *param_2 = DAT_ram_8254 + *param_2;
    *(undefined1 *)(param_1 + 1) = 0xdc;
    return;
  }
  DAT_ram_824c = DAT_ram_8256;
  DAT_ram_8248 = DAT_ram_8250;
  *(undefined1 *)(param_1 + 1) = 0xde;
  return;
}

