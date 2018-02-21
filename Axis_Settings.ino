CNCAxis AxisX( 0,  1,  xDirr,  xStep,  4);
CNCAxis AxisY( 5,  6,  yDirr,  yStep,  9);
CNCAxis AxisZ(10, 11, zDirr, zStep, 25);
CNCAxis AxisA(26, 27, aDirr, aStep, 30);
CNCAxis AxisB(23, 22, bDirr, bStep, 19);
CNCAxis AxisC(18, 17, cDirr, cStep, 14);

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
long APos()
{
  return AxisA.GetPosition(&stepperA);
}
long BPos()
{
  return AxisB.GetPosition(&stepperB);
}
long CPos()
{
  return AxisC.GetPosition(&stepperC);
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

void ForwardA()
{
  AxisA.StepForward(&stepperA);
}

void BackwardA()
{
  AxisA.StepBackward(&stepperA);
}

void ForwardB()
{
  AxisB.StepForward(&stepperB);
}

void BackwardB()
{
  AxisB.StepBackward(&stepperB);
}

void ForwardC()
{
  AxisC.StepForward(&stepperC);
}

void BackwardC()
{
  AxisC.StepBackward(&stepperC);
}
