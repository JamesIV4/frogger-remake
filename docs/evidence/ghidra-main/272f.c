
void driveFlyPatrol(void)

{
  char cVar1;
  char *pcVar2;
  
  if (DAT_ram_833e == '\0') {
    if ((char)DAT_ram_833d < '\0') {
      DAT_ram_833d = DAT_ram_833d - 2;
    }
    DAT_ram_833d = DAT_ram_833d + 1;
    pcVar2 = "";
    FUN_ram_279a(DAT_ram_833d & 0x7f);
    cVar1 = *pcVar2;
    if (cVar1 != '\0') {
      if (cVar1 != '\x01') {
        DAT_ram_8040 = cVar1 + DAT_ram_811c;
        return;
      }
      DAT_ram_833e = 0x3c;
      return;
    }
    DAT_ram_833d = DAT_ram_833d ^ 0x80;
    DAT_ram_833e = 0x3c;
    DAT_ram_8041 = 0x1e;
    return;
  }
  DAT_ram_833e = DAT_ram_833e + -1;
  if (DAT_ram_833e != '\x1e') {
    pcVar2 = "";
    FUN_ram_279a((DAT_ram_833d & 0x7f) + 1);
    DAT_ram_8040 = *pcVar2 + DAT_ram_811c;
    return;
  }
  DAT_ram_8041 = 0x21;
  if (-1 < (char)DAT_ram_833d) {
    return;
  }
  DAT_ram_8041 = 0xa1;
  return;
}

