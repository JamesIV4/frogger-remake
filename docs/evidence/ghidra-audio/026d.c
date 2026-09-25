
void audio_026d(undefined2 param_1)

{
  DAT_io_0080 = 7;
  DAT_ram_404c = DAT_ram_404c & (byte)((ushort)param_1 >> 8) | (byte)param_1;
  DAT_io_0040 = DAT_ram_404c;
  return;
}

