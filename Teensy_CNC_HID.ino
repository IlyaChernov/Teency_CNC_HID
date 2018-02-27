#include "CNCAxis.h"

#include "MoveCommand.h"

#include "src/AccelStepper/AccelStepper.h"

#include "src/AccelStepper/MultiStepper.h"

#include "TeensyCNCCore.h"

#include "enum.h"

//#define useEncoders
//#define useButtons

#define DEBUG
//#define DEBUGReports
#define DEBUGProcessing

//pin definitions for all 3 axis
#define xEnbl 0 //Enable pin
#define xDirr 1 //Direction pin
#define xStep 2 //Step pin
#define xEnds 3 //Endstop pin
#define xEncA 4 //Encoder pin A
#define xEncB 5 //Encoder pin B

#define yEnbl 7
#define yDirr 8
#define yStep 9
#define yEnds 10
#define yEncA 11
#define yEncB 12

#define zEnbl 24
#define zDirr 25
#define zStep 26
#define zEnds 27
#define zEncA 28
#define zEncB 29

//pin definitions for buttons start/pause/emergency
#define s1_pin 33
#define s2_pin 36
#define s3_pin 39

#define axisMaxSteps 2000000000

TeensyCNCCore cncore;

#ifdef useEncoders
AccelStepper stepperX(&ForwardX, &BackwardX);
AccelStepper stepperY(&ForwardY, &BackwardY);
AccelStepper stepperZ(&ForwardZ, &BackwardZ);
#else
AccelStepper stepperX(AccelStepper::DRIVER, xStep, xDirr);
AccelStepper stepperY(AccelStepper::DRIVER, yStep, yDirr);
AccelStepper stepperZ(AccelStepper::DRIVER, zStep, zDirr);
#endif

MultiStepper steppers;

unsigned int msBetweenReports = 1000;
unsigned int msBetweenReportsMoving = 50;

// RawHID packets are always 64 bytes
byte buffer[64];
elapsedMillis msUntilPositionReport;
elapsedMillis msUntilStatusReport;

void setup() {
  Serial.begin(9600);
  cncore.ExecuteCallBack = ExecuteCode;

#ifdef useButtons
  pinMode(s1_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(s1_pin), emergency_stop, FALLING );
  pinMode(s2_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(s2_pin), pause, FALLING );
  pinMode(s3_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(s3_pin), start_continue, FALLING );
#endif

  stepperX.setMaxSpeed(2000);
  stepperY.setMaxSpeed(2000);
  stepperZ.setMaxSpeed(2000);

  steppers.addStepper(stepperX);
  steppers.addStepper(stepperY);
  steppers.addStepper(stepperZ);
}

void emergency_stop() {
  cncore.global_state.cnc_status.engine_state = EmergencyStop;
}

void pause() {
  if ( cncore.global_state.cnc_status.engine_state == Running )
    cncore.global_state.cnc_status.engine_state = Paused;
}

void start_continue() {
  cncore.global_state.cnc_status.engine_state = Running;
}

void receiveCommand()
{
  int n = RawHID.recv(buffer, 0); // 0 timeout = do not wait

  if (n > 0) {
   // Serial.println(n);
    cncore.global_state.USBCMDqueue.enqueue(buffer);

#ifdef DEBUG
    Serial.print("Incoming cmd - ");
    Serial.println((char*)buffer);
    Serial.print("Queue count - ");
    Serial.println(cncore.global_state.USBCMDqueue.count());
    Serial.print("CNC Engine State - ");
    Serial.println(cncore.global_state.cnc_status.engine_state);
#endif
  }
}

void ExecuteCode (const char * match, const unsigned int length, char * & replacement, unsigned int & replacement_length, const MatchState & ms)
{
  String code = (((String)match).substring(0, length)).toLowerCase();

  cncore.ExecuteCode(code);

  replacement = (char *)"";
  replacement_length = 0;
}

void processBuffer(byte* buf)
{
  cncore.ProcessGCodeFrame((char*)buf);
}

void loop()
{
  if (((cncore.global_state.USBCMDqueue.isEmpty() || cncore.global_state.cnc_status.engine_state != Running) &&  msUntilStatusReport > msBetweenReports) || msUntilStatusReport > msBetweenReports) {
    msUntilStatusReport = 0;
    cncore.report_state();
  }

  long positionsArray[] = {XPos(), YPos(), ZPos()};

  if (cncore.global_state.cnc_status.engine_state == EmergencyStop) {
    cncore.global_state.USBCMDqueue = QueueArray <byte*>();
    cncore.global_state.ImmediateUSBCMDqueue = QueueArray <byte*>();

    cncore.global_state.cnc_position.setDestinations(positionsArray);

    steppers.moveTo(positionsArray);
    //Serial.println("Ret");
    return;
  }

  cncore.global_state.cnc_position.setPositions(positionsArray);

  receiveCommand();

  // every msBetweenReports, send a packet over usb
  if ((AllDestinationsReached() && msUntilPositionReport > msBetweenReportsMoving) || msUntilPositionReport > msBetweenReports) {
    msUntilPositionReport = 0;
    cncore.report_positions();
  }

  //Serial.println(stepperX.distanceToGo() + stepperY.distanceToGo() + stepperZ.distanceToGo());
  if (!cncore.global_state.ImmediateUSBCMDqueue.isEmpty())
  {
    //Serial.println("Process 1");
    processBuffer(cncore.global_state.ImmediateUSBCMDqueue.dequeue());
    stepperX.setMaxSpeed(cncore.global_state.cnc_speeds.movement_speed);
    stepperY.setMaxSpeed(cncore.global_state.cnc_speeds.movement_speed);
    stepperZ.setMaxSpeed(cncore.global_state.cnc_speeds.movement_speed);
    long positionsArray[] = {cncore.global_state.cnc_position.x_destination_steps, cncore.global_state.cnc_position.y_destination_steps, cncore.global_state.cnc_position.z_destination_steps};
    steppers.moveTo(positionsArray);
  }
  else if (!cncore.global_state.USBCMDqueue.isEmpty() && AllDestinationsReached() && cncore.global_state.cnc_status.engine_state == Running)
  {
    //Serial.println("Process 2");
    processBuffer(cncore.global_state.USBCMDqueue.dequeue());
    stepperX.setMaxSpeed(cncore.global_state.cnc_speeds.movement_speed);
    stepperY.setMaxSpeed(cncore.global_state.cnc_speeds.movement_speed);
    stepperZ.setMaxSpeed(cncore.global_state.cnc_speeds.movement_speed);
    long positionsArray[] = {cncore.global_state.cnc_position.x_destination_steps, cncore.global_state.cnc_position.y_destination_steps, cncore.global_state.cnc_position.z_destination_steps};
    steppers.moveTo(positionsArray);
  }

  steppers.run();
}

bool AllDestinationsReached()
{
  return ( stepperX.distanceToGo() + stepperY.distanceToGo() + stepperZ.distanceToGo() == 0 );
}
