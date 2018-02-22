#ifndef ExtAccelStepper_h
#define ExtAccelStepper_h

#include <AccelStepper.h>

class ExtAccelStepper : public AccelStepper
{
  public:    
    ExtAccelStepper(void (*forward)(), void (*backward)())
    : AccelStepper(forward, backward) {}
    
};

class Derived : public ExtAccelStepper
{
  public:
    void fnDerived (void)  {}
    Derived(void (*forward)(), void (*backward)())
    : ExtAccelStepper(forward, backward) {}
    
};

#endif
