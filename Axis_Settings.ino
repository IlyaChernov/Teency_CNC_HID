

//CNCAxis AxisX( xEncA, xEncB, xDirr, xStep, xEnds, xEnbl);
//CNCAxis AxisY( yEncA, yEncB, yDirr, yStep, yEnds, yEnbl);
//CNCAxis AxisZ( zEncA, zEncB, zDirr, zStep, zEnds, zEnbl);

long XPos()
{
  return cncore.AxisX.GetPosition(&stepperX);
}
long YPos()
{
  return cncore.AxisY.GetPosition(&stepperY);
}
long ZPos()
{
  return cncore.AxisZ.GetPosition(&stepperZ);
}

void ForwardX()
{
  cncore.AxisX.StepForward(&stepperX);
}

void BackwardX()
{
  cncore.AxisX.StepBackward(&stepperX);
}

void ForwardY()
{
  cncore.AxisY.StepForward(&stepperY);
}

void BackwardY()
{
  cncore.AxisY.StepBackward(&stepperY);
}

void ForwardZ()
{
  cncore.AxisZ.StepForward(&stepperZ);
}

void BackwardZ()
{
  cncore.AxisZ.StepBackward(&stepperZ);
}
