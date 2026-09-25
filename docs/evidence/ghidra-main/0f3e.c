
undefined1 FUN_ram_0f3e(void)

{
  undefined2 in_stack_00000000;
  
  DAT_ram_83bd = DAT_ram_83bd + -1;
  if (DAT_ram_83bd == '\0') {
    DAT_ram_83bd = 8;
    DAT_ram_83be = DAT_ram_83be + -1;
    if (DAT_ram_83be == '\0') {
      DAT_ram_83be = '\x04';
    }
    return *(undefined1 *)CONCAT11(0x2e,DAT_ram_83be + '\x1b');
  }
  return (char)((ushort)in_stack_00000000 >> 8);
}

