CNCAxis AxisX( 0,  1,  xDirr,  xStep,  4);
CNCAxis AxisY( 5,  6,  yDirr,  yStep,  9);
CNCAxis AxisZ(10, 11, zDirr, zStep, 25);

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
