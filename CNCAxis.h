#define ENCODER_USE_INTERRUPTS

#include <Encoder.h>
#include <AccelStepper.h>

#ifndef CNCAxis_h
#define CNCAxis_h

class CNCAxis
{
  public:
    CNCAxis(int encoderPinA, int encoderPinB, int pinDir, int pinStep, int pinEndstop);
    void StepForward(AccelStepper *);
    void StepBackward(AccelStepper *);
    long GetPosition(AccelStepper *);
  private:
    int _pinDir, _pinStep, _pinEndstop;
    Encoder * _encoder;
};
#endif

