#define ENCODER_USE_INTERRUPTS

#include <Encoder.h>
#include "src/AccelStepper/AccelStepper.h"

#ifndef CNCAxis_h
#define CNCAxis_h

class CNCAxis
{
  public:
    CNCAxis(int encoderPinA, int encoderPinB, int pinDir, int pinStep, int pinEndstop, int pinEnable);
    void StepForward(AccelStepper *);
    void StepBackward(AccelStepper *);
    long GetPosition(AccelStepper *);
    void Enable();
    void Disable();
  private:
    int _pinDir, _pinStep, _pinEndstop, _pinEnable;
    Encoder * _encoder;
};
#endif

