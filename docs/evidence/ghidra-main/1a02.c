
void seedObjectAnimationState(void)

{
  char cVar1;
  undefined1 *puVar2;
  
  DAT_ram_8025 = 5;
  DAT_ram_8027 = 5;
  DAT_ram_802d = 4;
  DAT_ram_802f = 4;
  DAT_ram_8035 = 7;
  DAT_ram_8037 = 7;
  DAT_ram_8021 = 6;
  DAT_ram_8023 = 6;
  DAT_ram_8039 = 6;
  DAT_ram_803b = 6;
  cVar1 = '\n';
  puVar2 = &DAT_ram_800d;
  do {
    *puVar2 = 5;
    puVar2 = puVar2 + 2;
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  DAT_ram_8029 = 5;
  DAT_ram_802b = 5;
  DAT_ram_8031 = 5;
  DAT_ram_8033 = 5;
  DAT_ram_800d = 2;
  DAT_ram_800f = 2;
  DAT_ram_8015 = 2;
  DAT_ram_8017 = 2;
  DAT_ram_8019 = 2;
  DAT_ram_801b = 2;
  return;
}

