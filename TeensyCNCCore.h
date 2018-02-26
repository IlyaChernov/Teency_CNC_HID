/*
  tcnccore.h - Library for TEENSY USB CNC.
  Created by Ilya Chernov, December 9, 2016.
*/
#ifndef TeensyCNCCore_h
#define TeensyCNCCore_h

#include "Arduino.h"
#include "CNCGlobalState.h"
#include "src/Regexp/Regexp.h"

class TeensyCNCCore
{
  private:
    double xToSteps(double distance);
    double yToSteps(double distance);
    double zToSteps(double distance);
  public:
    void report_state();
    void report_speeds();
    void report_positions();
    CNCGlobalState global_state;
    void ProcessGCodeFrame(char* frame);
    void ExecuteCode (String code);
    GlobalReplaceCallback ExecuteCallBack;
};

#endif
