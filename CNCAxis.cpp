#include "CNCAxis.h"
CNCAxis::CNCAxis(int encoderPinA, int encoderPinB, int pinDir, int pinStep, int pinEndstop, int pinEnable)
{
  _pinDir = pinDir;
  _pinStep = pinStep;
  _pinEndstop = pinEndstop;
  _pinEnable = pinEnable;
  _encoder = new Encoder(encoderPinA, encoderPinB);

  pinMode(_pinDir, OUTPUT);
  pinMode(_pinStep, OUTPUT);
  pinMode(_pinEndstop, INPUT);
  pinMode(_pinEnable, OUTPUT);
}

void CNCAxis::Enable()
{
  digitalWrite(_pinEnable, HIGH);
}
void CNCAxis::Disable()
{
  digitalWrite(_pinEnable, LOW);
}

long CNCAxis::GetPosition(AccelStepper * _stepper)
{
#ifdef useEncoders
  return  _encoder->read();
#else
  return _stepper->currentPosition();
#endif
}

void CNCAxis::StepForward(AccelStepper * _stepper)
{
  Enable();
  long pos = _stepper->currentPosition();
  digitalWrite(_pinDir, HIGH);
  while (pos == _stepper->currentPosition()) {
    digitalWrite(_pinStep, HIGH);
    delayMicroseconds(_stepper->getMinPulseWidth());
    digitalWrite(_pinStep, LOW);
#ifndef useEncoders
    _stepper->setCurrentPosition(_stepper->currentPosition() + 1);
#endif
  }
}

void CNCAxis::StepBackward(AccelStepper * _stepper)
{
  Enable();
  long pos = _stepper->currentPosition();
  digitalWrite(_pinDir, LOW);
  while (pos == _stepper->currentPosition()) {
    digitalWrite(_pinStep, HIGH);
    delayMicroseconds(_stepper->getMinPulseWidth());
    digitalWrite(_pinStep, LOW);
#ifndef useEncoders
    _stepper->setCurrentPosition(_stepper->currentPosition() - 1);
#endif
  }
}
