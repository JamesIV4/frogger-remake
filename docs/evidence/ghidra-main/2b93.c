
char writeSpriteObjectSlotX(char *param_1,short param_2)

{
  char cVar1;
  
  if (*(char *)(param_2 + 6) == '\0') {
    return '\0';
  }
  *param_1 = *(char *)CONCAT11(0x80,*(undefined1 *)(param_2 + 0xb)) - *(char *)(param_2 + 2);
  cVar1 = *(char *)(param_2 + 4);
  param_1[3] = cVar1;
  return cVar1;
}

