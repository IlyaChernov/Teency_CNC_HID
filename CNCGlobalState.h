/*
  cnc_global_state.h - Library for TEENSY USB CNC.
  Created by Ilya Chernov, December 9, 2016.
  Not released into the public domain.
*/
#ifndef CNCGlobalState_h
#define CNCGlobalState_h

#include "Arduino.h"
#include <QueueArray.h>
//#include "Position.h"
//#include "Status.h"

struct Speeds
{
  public:
    int x_speed;
    int y_speed;
    int z_speed;
    int a_speed;
    int b_speed;
    int c_speed;
    int spindle_speed;
};

struct Position
{
  public:
    int x_steps;
    int y_steps;
    int z_steps;
    int a_steps;
    int b_steps;
    int c_steps;

    int x_destination_steps;
    int y_destination_steps;
    int z_destination_steps;
    int a_destination_steps;
    int b_destination_steps;
    int c_destination_steps;
};

struct Status
{
  public:
    int buffer_count;    
    int line_number;
    int engine_state;
    //EngineState engine_state;
    //bool pause;
    
};

/*enum EngineState {
  running,
  paused,
  emergency_stopped
};*/


class CNCGlobalState
{
  public:
    CNCGlobalState();
    Position cnc_position;
    Status cnc_status;   
    Speeds cnc_speeds;
    QueueArray <byte*> USBCMDqueue;
    QueueArray <byte*> ImmediateUSBCMDqueue;
};

#endif
