
void audio_023c(undefined2 param_1)

{
  char cVar1;
  undefined1 uVar2;
  
  uVar2 = (undefined1)((ushort)param_1 >> 8);
  cVar1 = (DAT_ram_404b + -1) * '\x02';
  audio_0309();
  DAT_io_0080 = cVar1 + '\x01';
  DAT_io_0040 = uVar2;
  return;
}

