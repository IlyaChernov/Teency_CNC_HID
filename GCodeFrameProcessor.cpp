#include "Arduino.h"

#include "GCodeFrameProcessor.h"
#include "Enum.h"

void GCodeProcessor::Process(char* frame, CNCGlobalState &gstate) 
{
  gstate.curPosType = G00;
//#ifdef DEBUGProcessing
    Serial.print("Processing frame - ");
    Serial.println(frame);    
//#endif  
  }

