#include "Arduino.h"

#include "GCodeFrameProcessor.h"
#include "Enum.h"
#include "src/Regexp/Regexp.h"

void GCodeProcessor::Process(char* frame, CNCGlobalState &gstate)
{
  Serial.print("Processing frame - ");
  Serial.println(frame);

  MatchState ms;
  char buf [64];
  ms.Target(frame);

}

