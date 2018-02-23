

CNCAxis AxisX( xEncA, xEncB, xDirr, xStep, xEnds);
CNCAxis AxisY( yEncA, yEncB, yDirr, yStep, yEnds);
CNCAxis AxisZ( zEncA, zEncB, zDirr, zStep, zEnds );

long XPos()
{
  return AxisX.GetPosition(&stepperX);
}
long YPos()
{
  return AxisY.GetPosition(&stepperY);
}
long ZPos()
{
  return AxisZ.GetPosition(&stepperZ);
}

void ForwardX()
{
  AxisX.StepForward(&stepperX);
}

void BackwardX()
{
  AxisX.StepBackward(&stepperX);
}

void ForwardY()
{
  AxisY.StepForward(&stepperY);
}

void BackwardY()
{
  AxisY.StepBackward(&stepperY);
}

void ForwardZ()
{
  AxisZ.StepForward(&stepperZ);
}

void BackwardZ()
{
  AxisZ.StepBackward(&stepperZ);
}
