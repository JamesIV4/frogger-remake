
void audio_019d(char param_1,short param_2)

{
  undefined2 *puVar1;
  
  puVar1 = (undefined2 *)(param_2 + (ushort)(byte)(param_1 * '\x02'));
                    /* WARNING: Could not recover jumptable at 0x01a6. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)*puVar1)((short)puVar1 + 1);
  return;
}

