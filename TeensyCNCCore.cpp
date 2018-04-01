/*
  tcnccore.cpp -Library for TEENSY USB CNC.
  Created by Ilya Chernov, December 9, 2016.
*/

#include "Arduino.h"
#include "TeensyCNCCore.h"
#include "Enum.h"

#define regexSimpleGM "([GgMm]%d+)"
#define regexSimple "(%a%-?%d+)"
#define regexDecimal "(%a%-?%d+([.,]%d+))"

#define xRevolutionSteps 400
#define yRevolutionSteps 400
#define zRevolutionSteps 2000

//distance in mm
#define xRevolutionDistance 4
#define yRevolutionDistance 4
#define zRevolutionDistance 4

long long TeensyCNCCore::xToSteps(float  distance)
{
  return distance / ((float )xRevolutionDistance / (float )xRevolutionSteps);
}

long long TeensyCNCCore::yToSteps(float  distance)
{
  return distance / ((float )yRevolutionDistance / (float )yRevolutionSteps);
}

long long TeensyCNCCore::zToSteps(float  distance)
{
  return distance / ((float )zRevolutionDistance / (float )zRevolutionSteps);
}

float  TeensyCNCCore::xToUnits(long long distance)
{
  return distance * ((float )xRevolutionDistance / (float )xRevolutionSteps);
}

float  TeensyCNCCore::yToUnits(long long distance)
{
  return distance * ((float )yRevolutionDistance / (float )yRevolutionSteps);
}

float  TeensyCNCCore::zToUnits(long long distance)
{
  return distance * ((float )zRevolutionDistance / (float )zRevolutionSteps);
}

void TeensyCNCCore::ProcessGCodeFrame(char* frame)
{
  Serial.print("Processing frame - ");
  Serial.println(frame);

  MatchState ms;
  ms.Target(frame);
  unsigned long count;

  count = ms.GlobalReplace (regexSimpleGM, ExecuteCallBack); //First get base commands
  count += ms.GlobalReplace (regexDecimal, ExecuteCallBack);
  count += ms.GlobalReplace (regexSimple, ExecuteCallBack);

  Serial.print ("Found ");
  Serial.print (count);
  Serial.println (" codes in GCode frame");
}

void TeensyCNCCore::ExecuteCode(String code)
{
  Serial.print ("Executing ");
  Serial.print (code);
  Serial.println (" code");

  if (code.startsWith("g"))
  {
    switch (code.remove(0, 1).toInt())
    {
      case 0:
        global_state.curPosType = G00;
        break;
      case 1:
        global_state.curPosType = G01;
        break;
      case 2:
        global_state.curPosType = G02;
        break;
      case 3:
        global_state.curPosType = G03;
        break;
      case 90:
        global_state.curCoordSysType = Absolute;
        break;
      case 91:
        global_state.curCoordSysType = Incremental;
        break;
    }
  }
  else if (code.startsWith("m"))
  {
    switch (code.remove(0, 1).toInt())
    {
      case 0:
        global_state.cnc_status.engine_state = Paused;
        break;
    }
  }
  else if (code.startsWith("f"))
  {
    global_state.cnc_speeds.movement_speed = code.remove(0, 1).toInt();
    global_state.PositioningSpeeds[global_state.curPosType] = global_state.cnc_speeds.movement_speed;

  }
  else if (code.startsWith("x"))
  {
    float value = xToSteps(code.remove(0, 1).replace(",", ".").toFloat());
    global_state.cnc_position.x_destination_steps = global_state.curCoordSysType == Absolute ? value : (global_state.cnc_position.x_destination_steps + value) ;
  }
  else if (code.startsWith("y"))
  {
    float value = yToSteps(code.remove(0, 1).replace(",", ".").toFloat());
    global_state.cnc_position.y_destination_steps = global_state.curCoordSysType == Absolute ? value : (global_state.cnc_position.y_destination_steps + value) ;
  }
  else if (code.startsWith("z"))
  {
    float value = zToSteps(code.remove(0, 1).replace(",", ".").toFloat());
    global_state.cnc_position.z_destination_steps = global_state.curCoordSysType == Absolute ? value : (global_state.cnc_position.z_destination_steps + value) ;
  }
}

void TeensyCNCCore::report_state()
{
  byte buffer[64];
  int CommandCode = 65280;

  int startingByte = 0;
  buffer[startingByte++] = (CommandCode >> 0) & 0xFF;
  buffer[startingByte++] = (CommandCode >> 8) & 0xFF;
  buffer[startingByte++] = (CommandCode >> 16) & 0xFF;
  buffer[startingByte++] = (CommandCode >> 24) & 0xFF;

  buffer[startingByte++] = (global_state.cnc_status.line_number >> 0) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_status.line_number >> 8) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_status.line_number >> 16) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_status.line_number >> 24) & 0xFF;

  /*buffer[startingByte++] = (global_state.USBCMDqueue.count() >> 0) & 0xFF;
    buffer[startingByte++] = (global_state.USBCMDqueue.count() >> 8) & 0xFF;
    buffer[startingByte++] = (global_state.USBCMDqueue.count() >> 16) & 0xFF;
    buffer[startingByte++] = (global_state.USBCMDqueue.count() >> 24) & 0xFF;*/

  buffer[startingByte++] = (global_state.USBCMDqueueSTR.length() >> 0) & 0xFF;
  buffer[startingByte++] = (global_state.USBCMDqueueSTR.length() >> 8) & 0xFF;
  buffer[startingByte++] = (global_state.USBCMDqueueSTR.length() >> 16) & 0xFF;
  buffer[startingByte++] = (global_state.USBCMDqueueSTR.length() >> 24) & 0xFF;

  buffer[startingByte++] = (global_state.cnc_status.engine_state >> 0) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_status.engine_state >> 8) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_status.engine_state >> 16) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_status.engine_state >> 24) & 0xFF;

#ifdef DEBUGReports
  Serial.print("Report Buffer Count " + String(global_state.USBCMDqueue.count()) + " | ");
  for (int i = 0; i < 64; i++) {
    if (i % 4 == 0 && i > 0)
      Serial.print("  ");
    Serial.print(buffer[i]);
    Serial.print(" ");
  }
  Serial.println("");
#endif
  RawHID.send(buffer, 100);
}

void TeensyCNCCore::report_speeds()
{
  /*byte buffer[64];
    int CommandCode = 65281;

    int startingByte = 0;
    buffer[startingByte++] = (CommandCode >> 0) & 0xFF;
    buffer[startingByte++] = (CommandCode >> 8) & 0xFF;
    buffer[startingByte++] = (CommandCode >> 16) & 0xFF;
    buffer[startingByte++] = (CommandCode >> 24) & 0xFF;

    buffer[startingByte++] = (global_state.cnc_speeds.x_speed >> 0) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.x_speed >> 8) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.x_speed >> 16) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.x_speed >> 24) & 0xFF;

    buffer[startingByte++] = (global_state.cnc_speeds.y_speed >> 0) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.y_speed >> 8) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.y_speed >> 16) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.y_speed >> 24) & 0xFF;

    buffer[startingByte++] = (global_state.cnc_speeds.z_speed >> 0) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.z_speed >> 8) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.z_speed >> 16) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.z_speed >> 24) & 0xFF;

    buffer[startingByte++] = (global_state.cnc_speeds.spindle_speed >> 0) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.spindle_speed >> 8) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.spindle_speed >> 16) & 0xFF;
    buffer[startingByte++] = (global_state.cnc_speeds.spindle_speed >> 24) & 0xFF;

    RawHID.send(buffer, 100);*/
}

void TeensyCNCCore::report_positions()
{
  /*if(global_state.cnc_position.x_steps > 0 || global_state.cnc_position.y_steps > 0 || global_state.cnc_position.z_steps > 0){
    Serial.print(global_state.cnc_position.x_steps);
    Serial.print(" ");
    Serial.print(global_state.cnc_position.y_steps);
    Serial.print(" ");
    Serial.print(global_state.cnc_position.z_steps);
    Serial.println();
    }*/
  byte buffer[64];

  int startingByte = 0;

  int CommandCode = 65282;

  byte *b = (byte *)&CommandCode;

  buffer[startingByte++] = (b[0]) & 0xFF;
  buffer[startingByte++] = (b[1]) & 0xFF;
  buffer[startingByte++] = (b[2]) & 0xFF;
  buffer[startingByte++] = (b[3]) & 0xFF;

  float xunits = xToUnits(global_state.cnc_position.x_steps);
  float yunits = yToUnits(global_state.cnc_position.y_steps);
  float zunits = zToUnits(global_state.cnc_position.z_steps);

  b = (byte *)&xunits;

  buffer[startingByte++] = (b[0]) & 0xFF;
  buffer[startingByte++] = (b[1]) & 0xFF;
  buffer[startingByte++] = (b[2]) & 0xFF;
  buffer[startingByte++] = (b[3]) & 0xFF;

  b = (byte *)&yunits;

  buffer[startingByte++] = (b[0]) & 0xFF;
  buffer[startingByte++] = (b[1]) & 0xFF;
  buffer[startingByte++] = (b[2]) & 0xFF;
  buffer[startingByte++] = (b[3]) & 0xFF;

  b = (byte *)&zunits;

  buffer[startingByte++] = (b[0]) & 0xFF;
  buffer[startingByte++] = (b[1]) & 0xFF;
  buffer[startingByte++] = (b[2]) & 0xFF;
  buffer[startingByte++] = (b[3]) & 0xFF;

  xunits = xToUnits(global_state.cnc_position.x_destination_steps);
  yunits = yToUnits(global_state.cnc_position.y_destination_steps);
  zunits = zToUnits(global_state.cnc_position.z_destination_steps);

  b = (byte *)&xunits;

  buffer[startingByte++] = (b[0]) & 0xFF;
  buffer[startingByte++] = (b[1]) & 0xFF;
  buffer[startingByte++] = (b[2]) & 0xFF;
  buffer[startingByte++] = (b[3]) & 0xFF;

  b = (byte *)&yunits;

  buffer[startingByte++] = (b[0]) & 0xFF;
  buffer[startingByte++] = (b[1]) & 0xFF;
  buffer[startingByte++] = (b[2]) & 0xFF;
  buffer[startingByte++] = (b[3]) & 0xFF;

  b = (byte *)&zunits;

  buffer[startingByte++] = (b[0]) & 0xFF;
  buffer[startingByte++] = (b[1]) & 0xFF;
  buffer[startingByte++] = (b[2]) & 0xFF;
  buffer[startingByte++] = (b[3]) & 0xFF;

  RawHID.send(buffer, 100);
}
