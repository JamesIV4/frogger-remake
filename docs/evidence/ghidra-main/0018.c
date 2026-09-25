
char enqueueSoundCommand(undefined1 param_1)

{
  char cVar1;
  
  if (DAT_ram_83fe == '\0') {
    return '\0';
  }
  DAT_ram_8300 = DAT_ram_8300 + '\x01';
  cVar1 = DAT_ram_8300;
  *(undefined1 *)CONCAT11(0x83,DAT_ram_8300) = param_1;
  return cVar1;
}

