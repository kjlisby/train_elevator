#include "PosLogic.h"

void PosLogic::Init (Calibrator *CA, Display *DI, Relays *RE, IRsensor *FLIR, IRsensor *RLIR, IRsensor *FRIR, IRsensor *RRIR) {
  this->MyCalibrator = CA;
  this->MyDisplay = DI;
  this->MyRelays = RE;
  this->FrontLeft_IR  = FLIR;
  this->RearLeft_IR   = RLIR;
  this->FrontRight_IR = FRIR;
  this->RearRight_IR  = RRIR;
  pinMode(LH_ENDSTOP_PIN, INPUT_PULLUP);
  pinMode(RH_ENDSTOP_PIN, INPUT_PULLUP);
  
  this->LHStepper = new AccelStepper(AccelStepper::DRIVER, LH_STEPPER_STEP_PIN, LH_STEPPER_DIR_PIN);
  // 500 rotation/min == 100.000 steps/min == 1667 steps/second
  this->LHStepper->setMaxSpeed(600.0);
  // acceleration is in steps per second per second i.e. to accelerate to max speed of 1500 steps/s in 3 seconds it needs to be 500
  this->LHStepper->setAcceleration(50000.0);
  this->LHStepper->setPinsInverted (true,false,false);

  this->RHStepper = new AccelStepper(AccelStepper::DRIVER, RH_STEPPER_STEP_PIN, RH_STEPPER_DIR_PIN);
  // 500 rotation/min == 100.000 steps/min == 1667 steps/second
  this->RHStepper->setMaxSpeed(600.0);
  // acceleration is in steps per second per second i.e. to accelerate to max speed of 1500 steps/s in 3 seconds it needs to be 500
  this->RHStepper->setAcceleration(50000.0);

  Serial.println("DEBUG PL Init Done");
}

bool PosLogic::isBlocked () {
//  Serial.println("DEBUG PosLogic::isBlocked");
  return (this->FrontLeft_IR->TrainSeen()  || this->RearLeft_IR->TrainSeen() ||
          this->FrontRight_IR->TrainSeen() || this->RearRight_IR->TrainSeen());
}

bool PosLogic::MoveOneStepper (bool RIGHT, bool RELATIVE, int Steps) {
  Serial.print("MoveOneStepper "); Serial.print(String(RIGHT)); Serial.print(" "); Serial.print(String(RELATIVE)); Serial.print(" "); Serial.println(String(Steps));
  if (RIGHT) {
    if (RELATIVE) {
      this->RHStepper->move(Steps);
    } else {
      this->RHStepper->moveTo(Steps);
    }
  } else {
    if (RELATIVE) {
      this->LHStepper->move(Steps);
    } else {
      this->LHStepper->moveTo(Steps);
    }
  }
  Serial.println("End of MoveOneStepper");
  return true;
}
  
bool PosLogic::MoveTo (int Level, int AdditionalSteps) {
  Serial.println("DEBUG PosLogic::MoveTo");
  if (Level < 1 || Level > 12) {
    Serial.println("Illegal level");
    return false;
  }
  Serial.println("DEBUG PosLogic::MoveTo checking blocked");
  if (this->isBlocked()) {
    Serial.println("STATUS BLOCKED");
    return false;
  }
  Serial.println("DEBUG PosLogic::MoveTo checking locked");
  if (this->Locked) {
    Serial.println("STATUS LOCKED");
    return false;
  }
  if (this->MyStatus != STATUS_IDLE) {
    Serial.println(this->GetStatus());
    return false;
  }
  this->NextLevel = Level;
  if (!this->HomingDone) {
    this->HomingDone = true;
    this->MyStatus = STATUS_HOMING_1;
    this->MyDisplay->Homing(1);
    Serial.println(this->GetStatus());
  } else {
    this->MyStatus  = STATUS_MOVING;
    this->LHStepper->moveTo(this->MyCalibrator->GetOffset(true,  Level)+AdditionalSteps);
    this->RHStepper->moveTo(this->MyCalibrator->GetOffset(false, Level)+AdditionalSteps);
    this->MyDisplay->NewLevel(Level);
    this->MyRelays->AtLevel(0);
    Serial.println(this->GetStatus());
  }
  return true;
}

bool PosLogic::MoveToSteps (int Level, int StepsLeft, int StepsRight) {
  Serial.println("DEBUG PosLogic::MoveToSteps");
  if (this->isBlocked()) {
    Serial.println("STATUS BLOCKED");
    return false;
  }
  if (this->Locked) {
    Serial.println("STATUS LOCKED");
    return false;
  }
  if (this->MyStatus != STATUS_IDLE) {
    Serial.println(this->GetStatus());
    return false;
  }
  if (!this->HomingDone) {
    Serial.println("STATUS NOT_HOMED");
    return false;
  }
  this->MyStatus  = STATUS_MOVING;
  this->NextLevel = Level;
  this->LHStepper->moveTo(StepsLeft);
  this->RHStepper->moveTo(StepsRight);
  this->MyDisplay->NewLevel(Level);
  Serial.println(this->GetStatus());
  return true;
}

void PosLogic::Lock () {
  Serial.println("LOCKED YES");
  this->Locked = true;
}

void PosLogic::Unlock () {
  Serial.println("LOCKED NO");
  this->Locked = false;
}

bool PosLogic::isLocked() {
  return this->Locked;
}
  
String PosLogic::GetStatus () {
  Serial.println("DEBUG PosLogic::GetStatus");
  if (this->isBlocked()) {
    return "STATUS BLOCKED";
  }
  switch (this->MyStatus) {
    case STATUS_HOMING_1:
      return "STATUS HOMING 1";
    case STATUS_HOMING_2:
      return "STATUS HOMING 2";
    case STATUS_HOMING_3:
      return "STATUS HOMING 3";
    case STATUS_HOMING_4:
      return "STATUS HOMING 4";
    case STATUS_MOVING:
      return "STATUS MOVING "+String(this->CurrentLevel)+" "+String(this->NextLevel);
    case STATUS_IDLE:
      return "STATUS IDLE "+String(this->CurrentLevel);
  }
  return "STATUS UNKNOWN";
}

int PosLogic::GetCurrentLevel () {
//	Serial.println("DEBUG PosLogic::GetCurrentLevel");
  if (this->isBlocked() || this->MyStatus != STATUS_IDLE) {
    return -1;
  }
  return this->CurrentLevel;
}

String PosLogic::GetStepperPositions () {
  String L_Status;
  String R_Status;
  String L_EndStop;
  String R_EndStop;
  if (this->LHStepper->isRunning()) {
    L_Status = "RUNNING";
  } else {
    L_Status = "STOPPED";
  }
  if (this->RHStepper->isRunning()) {
    R_Status = "RUNNING";
  } else {
    R_Status = "STOPPED";
  }
  if (!digitalRead(LH_ENDSTOP_PIN)) {
    L_EndStop = " ENDSTOP SWITCH ACTIVE ";
  } else {
    L_EndStop = " not at end-stop ";
  }
  if (!digitalRead(RH_ENDSTOP_PIN)) {
    R_EndStop = " ENDSTOP SWITCH ACTIVE ";
  } else {
    R_EndStop = " not at end-stop ";
  }
  return "STEPPER POSITION Left: "+L_Status+" at "+String(this->LHStepper->currentPosition())+L_EndStop+"  Right: "+R_Status+" at "+String(this->RHStepper->currentPosition())+R_EndStop;
}

bool PosLogic::isRunning() {
  return this->LHStepper->isRunning() || this->RHStepper->isRunning();
}

void PosLogic::Loop () {
  if (this->isBlocked () == false) {
    // Do not advance if we risk to crush a train
    this->LHStepper->run();
    this->RHStepper->run();
  }
  switch (this->MyStatus) {
    case STATUS_HOMING_1: // Moving downwards searching for end-stop
      if (!digitalRead(LH_ENDSTOP_PIN) && !digitalRead(RH_ENDSTOP_PIN)) {
        this->MyStatus = STATUS_HOMING_2;
        this->MyDisplay->Homing(2);
        Serial.println(this->GetStatus());
//        this->LHStepper->move(20); No need this is just duplicating the behavior of STATUS_HOMING_2
//        this->RHStepper->move(20); No need this is just duplicating the behavior of STATUS_HOMING_2
      } else {
        this->LHStepper->move(-500);
        this->RHStepper->move(-500);
      }
      break;
    case STATUS_HOMING_2: // Moving upwards (each side separately) until end-stop is exactly not activated.
      if (!this->LHStepper->isRunning() && !this->RHStepper->isRunning()) {
        if (!digitalRead(LH_ENDSTOP_PIN)) {
          this->LHStepper->move(20);
        }
        if (!digitalRead(RH_ENDSTOP_PIN)) {
          this->RHStepper->move(20);
        }
        if (!this->LHStepper->isRunning() && !this->RHStepper->isRunning()) {
          this->LHStepper->setCurrentPosition(0);
          this->RHStepper->setCurrentPosition(0);
//          this->MoveTo(1,500); The original purpose of STATUS_HOMING_3 was to move a bit beyond level 1. But why bother?
          this->MyStatus = STATUS_HOMING_3;
          this->MyDisplay->Homing(3);
          Serial.println(this->GetStatus());
        }
      } else {
        if (digitalRead(LH_ENDSTOP_PIN)) {
          this->LHStepper->stop();
        }
        if (digitalRead(RH_ENDSTOP_PIN)) {
          this->RHStepper->stop();
        }
      }
    case STATUS_HOMING_3: // Moving upwards until e few millimeters above level 1
      if (!this->LHStepper->isRunning() && !this->RHStepper->isRunning()) {
//        this->MoveTo(1,0); The original purpose of STATUS_HOMING_4 was to move to level 1. But why bother?
        this->MyStatus = STATUS_HOMING_4;
        this->MyDisplay->Homing(4);
        Serial.println(this->GetStatus());
      }
      break;
    case STATUS_HOMING_4: // Moving to level 1
      if (!this->LHStepper->isRunning() && !this->RHStepper->isRunning()) {
        this->CurrentLevel = 1;
        this->MyStatus = STATUS_IDLE;
        this->MoveTo(this->NextLevel,0);
      }
      break;
    case STATUS_MOVING:
      if (!this->LHStepper->isRunning() && !this->RHStepper->isRunning()) {
        this->CurrentLevel = this->NextLevel;
        this->MyStatus = STATUS_IDLE;
        this->MyDisplay->AtLevel(this->CurrentLevel);
        this->MyRelays->AtLevel(this->CurrentLevel);
        Serial.println(this->GetStatus());
      }
      break;
    default:;
      // idling - do nothing.
  }
}
