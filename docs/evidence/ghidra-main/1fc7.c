
void tickGatedCountdown(void)

{
  if (DAT_ram_826c == '\0') {
    return;
  }
  DAT_ram_826a = DAT_ram_826a + -1;
  if (DAT_ram_826a != '\0') {
    return;
  }
  DAT_ram_826c = 0;
  return;
}

