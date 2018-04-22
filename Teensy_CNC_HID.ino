#include "CNCAxis.h"

#include "MoveCommand.h"

#include "src/AccelStepper/AccelStepper.h"

#include "src/AccelStepper/MultiStepper.h"

#include "TeensyCNCCore.h"

#include "enum.h"

#include "pin_definitions.h"

//#define useEncoders
//#define useButtons

#define DEBUG
//#define DEBUGReports
#define DEBUGProcessing

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

unsigned int msBetweenReports = 500;
unsigned int msBetweenReportsMoving = 50;

// RawHID packets are always 64 bytes
byte buffer[64];
elapsedMillis msUntilPositionReport;
elapsedMillis msUntilStatusReport;

void setup() {
  Serial.begin(9600);
  cncore.ExecuteCallBack = ExecuteCode;
  cncore.AxisX = CNCAxis( xEncA, xEncB, xDirr, xStep, xEnds, xEnbl);
  cncore.AxisY = CNCAxis( yEncA, yEncB, yDirr, yStep, yEnds, yEnbl);
  cncore.AxisZ = CNCAxis( zEncA, zEncB, zDirr, zStep, zEnds, zEnbl);

#ifdef useButtons
  pinMode(s1_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(s1_pin), emergency_stop, FALLING );
  pinMode(s2_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(s2_pin), pause, FALLING );
  pinMode(s3_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(s3_pin), start_continue, FALLING );
#endif

  stepperX.setMaxSpeed(cncore.global_state.cnc_speeds.movement_speed);
  stepperY.setMaxSpeed(cncore.global_state.cnc_speeds.movement_speed);
  stepperZ.setMaxSpeed(cncore.global_state.cnc_speeds.movement_speed);

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
    cncore.global_state.USBCMDqueueSTR.concat((char*)buffer);
    cncore.global_state.USBCMDqueueSTR.concat('|');

    immediate_report_state();

    Serial.println("Queue contents - ");
    Serial.println(cncore.global_state.USBCMDqueueSTR);


#ifdef DEBUG
    Serial.print("Incoming cmd - ");
    Serial.println((char*)buffer);
    /*Serial.print("Queue count - ");
      Serial.println(cncore.global_state.USBCMDqueue.count());*/
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
#ifdef DEBUG
  Serial.print("Preprocessing frame - ");
  Serial.println((char*)buf);

  Serial.println("Queue contents - ");
  Serial.println(cncore.global_state.USBCMDqueueSTR);
  //for (int i = 0; i < cncore.global_state.USBCMDqueue.count(); i++)
  //{
  //  Serial.print(i);
  //  Serial.print(" - ");
  //  Serial.println((char*)cncore.global_state.USBCMDqueue.contents[i]);
  //}
#endif
  cncore.ProcessGCodeFrame((char*)buf);
}

void immediate_report_state()
{
  msUntilStatusReport = 0;
  cncore.report_state();
}

void loop()
{
  if (((/*cncore.global_state.USBCMDqueue.isEmpty() ||*/ cncore.global_state.cnc_status.engine_state != Running) &&  msUntilStatusReport > msBetweenReports) || msUntilStatusReport > msBetweenReports) {
    immediate_report_state();
  }

  long positionsArray[] = {XPos(), YPos(), ZPos()};

  if (cncore.global_state.cnc_status.engine_state == EmergencyStop) {
    //cncore.global_state.USBCMDqueue = QueueArray <byte*>();
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

  if (!cncore.global_state.ImmediateUSBCMDqueue.isEmpty())
  {
    float speedPerSec = cncore.global_state.cnc_speeds.movement_speed / 60;

    cncore.global_state.cnc_position.getXPathRelation();

    stepperX.setMaxSpeed(cncore.xToSteps(speedPerSec) * cncore.global_state.cnc_position.getXPathRelation());
    stepperY.setMaxSpeed(cncore.yToSteps(speedPerSec) * cncore.global_state.cnc_position.getYPathRelation());
    stepperZ.setMaxSpeed(cncore.zToSteps(speedPerSec) * cncore.global_state.cnc_position.getZPathRelation());

    long positionsArray[] = {cncore.global_state.cnc_position.x_destination_steps, cncore.global_state.cnc_position.y_destination_steps, cncore.global_state.cnc_position.z_destination_steps};
    steppers.moveTo(positionsArray);
  }
  else if (cncore.global_state.USBCMDqueueSTR.length() > 0 && AllDestinationsReached() && cncore.global_state.cnc_status.engine_state == Running)
  {
    byte buf[64];
    GetCommandFromBuffer(buf);
    processBuffer(buf);

    float speedPerSec = cncore.global_state.cnc_speeds.movement_speed / 60;

    stepperX.setMaxSpeed(cncore.xToSteps(speedPerSec) * cncore.global_state.cnc_position.getXPathRelation());
    stepperY.setMaxSpeed(cncore.yToSteps(speedPerSec) * cncore.global_state.cnc_position.getYPathRelation());
    stepperZ.setMaxSpeed(cncore.zToSteps(speedPerSec) * cncore.global_state.cnc_position.getZPathRelation());

    long positionsArray[] = {cncore.global_state.cnc_position.x_destination_steps, cncore.global_state.cnc_position.y_destination_steps, cncore.global_state.cnc_position.z_destination_steps};
    steppers.moveTo(positionsArray);
  }

  steppers.run();
}

void GetCommandFromBuffer(byte *buf)
{
  int comlen = cncore.global_state.USBCMDqueueSTR.indexOf('|');
  if (comlen != -1) {
    cncore.global_state.USBCMDqueueSTR.getBytes(buf, cncore.global_state.USBCMDqueueSTR.indexOf('|') + 1);
    cncore.global_state.USBCMDqueueSTR.remove(0, cncore.global_state.USBCMDqueueSTR.indexOf('|') + 1);
  }
  else
  {
    cncore.global_state.USBCMDqueueSTR.getBytes(buf, 64);
    cncore.global_state.USBCMDqueueSTR.remove(0);
  }
}

bool AllDestinationsReached()
{
  return ( stepperX.distanceToGo() + stepperY.distanceToGo() + stepperZ.distanceToGo() == 0 );
}
