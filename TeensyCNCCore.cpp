/*
  tcnccore.cpp -Library for TEENSY USB CNC.
  Created by Ilya Chernov, December 9, 2016.
*/

#include "Arduino.h"
#include "TeensyCNCCore.h"
#include "Enum.h"

#define regexSimple "(%a%-?%d+)"
#define regexDecimal "(%a%-?%d+([.,]%d+))"

void TeensyCNCCore::ProcessGCodeFrame(char* frame)
{
  Serial.print("Processing frame - ");
  Serial.println(frame);

  MatchState ms;
  ms.Target(frame);
  unsigned long count;

  count = ms.GlobalReplace (regexDecimal, ExecuteCallBack);
  count += ms.GlobalReplace (regexSimple, ExecuteCallBack);

  Serial.print ("Found ");
  Serial.print (count);
  Serial.println (" codes in GCode frame");
}

void TeensyCNCCore::ExecuteCode(String code)
{
  if (code == "g0" || code == "g00")
  {
    global_state.curPosType = G00;
  }
  else  if (code == "g1" || code == "g01")
  {
    global_state.curPosType = G01;
  }
  else if (code == "g2" || code == "g02")
  {
    global_state.curPosType = G02;
  }
  else if (code == "g3" || code == "g03")
  {
    global_state.curPosType = G03;
  }

  Serial.print ("Executing ");
  Serial.print (code);
  Serial.println (" code");  
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

  buffer[startingByte++] = (global_state.USBCMDqueue.count() >> 0) & 0xFF;
  buffer[startingByte++] = (global_state.USBCMDqueue.count() >> 8) & 0xFF;
  buffer[startingByte++] = (global_state.USBCMDqueue.count() >> 16) & 0xFF;
  buffer[startingByte++] = (global_state.USBCMDqueue.count() >> 24) & 0xFF;

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
  byte buffer[64];
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

  RawHID.send(buffer, 100);
}

void TeensyCNCCore::report_positions()
{
  byte buffer[64];
  int CommandCode = 65282;

  int startingByte = 0;
  buffer[startingByte++] = (CommandCode >> 0) & 0xFF;
  buffer[startingByte++] = (CommandCode >> 8) & 0xFF;
  buffer[startingByte++] = (CommandCode >> 16) & 0xFF;
  buffer[startingByte++] = (CommandCode >> 24) & 0xFF;

  buffer[startingByte++] = (global_state.cnc_position.x_steps >> 0) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.x_steps >> 8) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.x_steps >> 16) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.x_steps >> 24) & 0xFF;

  buffer[startingByte++] = (global_state.cnc_position.y_steps >> 0) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.y_steps >> 8) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.y_steps >> 16) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.y_steps >> 24) & 0xFF;

  buffer[startingByte++] = (global_state.cnc_position.z_steps >> 0) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.z_steps >> 8) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.z_steps >> 16) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.z_steps >> 24) & 0xFF;

  buffer[startingByte++] = (global_state.cnc_position.x_destination_steps >> 0) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.x_destination_steps >> 8) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.x_destination_steps >> 16) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.x_destination_steps >> 24) & 0xFF;

  buffer[startingByte++] = (global_state.cnc_position.y_destination_steps >> 0) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.y_destination_steps >> 8) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.y_destination_steps >> 16) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.y_destination_steps >> 24) & 0xFF;

  buffer[startingByte++] = (global_state.cnc_position.z_destination_steps >> 0) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.z_destination_steps >> 8) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.z_destination_steps >> 16) & 0xFF;
  buffer[startingByte++] = (global_state.cnc_position.z_destination_steps >> 24) & 0xFF;

  RawHID.send(buffer, 100);
}
