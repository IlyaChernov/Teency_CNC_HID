#ifndef GCodeFrameProcessor_h
#define GCodeFrameProcessor_h

#include "CNCGlobalState.h"

class GCodeProcessor {
  public:
    static void Process(char* frame, CNCGlobalState &gstate);
};

#endif
