/*
  cnc_global_state.h - Library for TEENSY USB CNC.
  Created by Ilya Chernov, December 9, 2016.
*/
#ifndef CNCGlobalState_h
#define CNCGlobalState_h

#include "Arduino.h"
#include "src/QueueArray/QueueArray.h"
#include "Enum.h"

struct Speeds
{
  public:
    int x_speed;
    int y_speed;
    int z_speed;
    int spindle_speed;
};

struct Position
{
  public:
    int x_steps;
    int y_steps;
    int z_steps;

    int x_destination_steps;
    int y_destination_steps;
    int z_destination_steps;

    void setPositions(long absolute[])
    {
      int i = 0;
      x_steps = absolute[i++];
      y_steps = absolute[i++];
      z_steps = absolute[i++];
    }

    void setDestinations(long absolute[])
    {
      int i = 0;
      x_destination_steps = absolute[i++];
      y_destination_steps = absolute[i++];
      z_destination_steps = absolute[i++];
    }
};

struct Status
{
  public:
    int buffer_count;
    int line_number;
    int engine_state;
};

class CNCGlobalState
{
  public:
    PositioningType curPosType;
    CoordinateSystemType curCoordSysType;
    MetricSystemType curMetricType;
    
    double PositioningSpeeds[PositioningTypeCount];
    
    CNCGlobalState();
    Position cnc_position;
    Status cnc_status;
    Speeds cnc_speeds;
    QueueArray <byte*> USBCMDqueue;
    QueueArray <byte*> ImmediateUSBCMDqueue;
};

#endif
