/*
  tcnccore.h - Library for TEENSY USB CNC.
  Created by Ilya Chernov, December 9, 2016.
*/
#ifndef TeensyCNCCore_h
#define TeensyCNCCore_h

#include "Arduino.h"
#include "CNCGlobalState.h"

class TeensyCNCCore
{
  public:
    void report_state();
    void report_speeds();
    void report_positions();
    CNCGlobalState global_state;
};

#endif
