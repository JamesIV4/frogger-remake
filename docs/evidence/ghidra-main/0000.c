
/* WARNING: Control flow encountered bad instruction data */
/* WARNING: This function may have set the stack pointer */

void seatStackAndEnterColdBoot(void)

{
  if (DAT_ram_4000 != 'U') {
    initColdBootAndEnterMainLoop(switchD_ram:0fbd::caseD_47);
    return;
  }
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}

