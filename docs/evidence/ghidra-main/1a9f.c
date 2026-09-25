
void FUN_ram_1a9f(char param_1)

{
  DAT_ram_8340 = param_1 + -1;
  if (DAT_ram_8340 != '\x01') {
    return;
  }
  clearFourByteCounterBlock();
  clearFlySpriteBlock();
  return;
}

