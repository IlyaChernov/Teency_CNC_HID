#include "Arduino.h"
#include "CNCGlobalState.h"

#define defaultG0Speed 200
#define defaultG1Speed 150
#define defaultG2Speed 150
#define defaultG3Speed 150

CNCGlobalState::CNCGlobalState()
{
  cnc_status.engine_state = Running;

  curPosType = G00;

  curCoordSysType = Absolute;

  curMetricType = Metric;

  PositioningSpeeds[G00] = defaultG0Speed;
  PositioningSpeeds[G01] = defaultG1Speed;
  PositioningSpeeds[G02] = defaultG2Speed;
  PositioningSpeeds[G03] = defaultG3Speed;
}
