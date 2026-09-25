
void issueSoundCommand(undefined1 param_1)

{
  DAT_ram_d000 = param_1;
  DAT_ram_d002 = DAT_ram_83d9 | 8;
  return;
}

