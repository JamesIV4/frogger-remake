
void armDiveHighPhase(void)

{
  if (DAT_ram_8101 == '\0') {
    armTwoPairFigureFrame(0);
  }
  stepDiveSurfaceTimer();
  return;
}

