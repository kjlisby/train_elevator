#include "PosLogic.h"

void PosLogic::Init (Calibrator *CA, Display *DI, Relays *RE) {
	this->MyCalibrator = CA;
	this->MyDisplay = DI;
  this->MyRelays = RE;
	this->LHStepper = new AccelStepper(AccelStepper::DRIVER, LH_STEPPER_STEP_PIN, LH_STEPPER_DIR_PIN);
	// 500 rotation/min == 100.000 steps/min == 1667 steps/second
	this->LHStepper->setMaxSpeed(1500.0);
	// acceleration is in steps per second per second i.e. to accelerate to max speed of 1500 steps/s in 3 seconds it needs to be 500
	this->LHStepper->setAcceleration(500.0);
	this->RHStepper = new AccelStepper(AccelStepper::DRIVER, RH_STEPPER_STEP_PIN, RH_STEPPER_DIR_PIN);
	// 500 rotation/min == 100.000 steps/min == 1667 steps/second
	this->RHStepper->setMaxSpeed(1500.0);
	// acceleration is in steps per second per second i.e. to accelerate to max speed of 1500 steps/s in 3 seconds it needs to be 500
	this->RHStepper->setAcceleration(500.0);
	pinMode(LH_ENDSTOP_PIN, INPUT_PULLUP);
	pinMode(RH_ENDSTOP_PIN, INPUT_PULLUP);
	pinMode(LH_BACK_SECURITY_PIN,  INPUT_PULLUP);
	pinMode(LH_FRONT_SECURITY_PIN, INPUT_PULLUP);
	pinMode(RH_BACK_SECURITY_PIN,  INPUT_PULLUP);
	pinMode(RH_FRONT_SECURITY_PIN, INPUT_PULLUP);
	Serial.println("DEBUG PL Init Done");
}
	
bool PosLogic::MoveTo (int Level, int AdditionalSteps) {
	Serial.println("DEBUG PosLogic::MoveTo");
  if (Level < 1 || Level > 12) {
    Serial.println("Illegal level");
    return false;
  }
	if (this->Blocked()) {
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
	this->NextLevel = Level;
	if (!this->HomingDone) {
		this->HomingDone = true;
		this->MyStatus = STATUS_HOMING_1;
		this->MyDisplay->Homing();
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
	if (this->Blocked()) {
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
	if (this->Blocked()) {
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

bool PosLogic::Blocked () {
//	Serial.println("DEBUG PosLogic::Blocked");
return false;
	return (!digitalRead(LH_BACK_SECURITY_PIN) || !digitalRead(LH_FRONT_SECURITY_PIN) || !digitalRead(RH_BACK_SECURITY_PIN) || !digitalRead(RH_FRONT_SECURITY_PIN));
}

int PosLogic::GetCurrentLevel () {
//	Serial.println("DEBUG PosLogic::GetCurrentLevel");
	if (this->Blocked() || this->MyStatus != STATUS_IDLE) {
		return -1;
	}
	return this->CurrentLevel;
}

bool PosLogic::isRunning() {
	return this->LHStepper->isRunning() || this->RHStepper->isRunning();
}

void PosLogic::Loop () {
	if (this->Blocked () == false) {
    // Do not advance if we risk to crush a train
		this->LHStepper->run();
		this->RHStepper->run();
  }
	switch (this->MyStatus) {
		case STATUS_HOMING_1: // Moving downwards searching for end-stop
			if (!digitalRead(LH_ENDSTOP_PIN) && !digitalRead(LH_ENDSTOP_PIN)) {
				this->MyStatus = STATUS_HOMING_2;
				Serial.println(this->GetStatus());
				this->LHStepper->move(20);
				this->RHStepper->move(20);
			} else {
				this->LHStepper->move(-1000);
				this->RHStepper->move(-1000);
			}
			break;
		case STATUS_HOMING_2: // Moving upwards (each side separately) until end-stop is exactly not activated
			if (!this->LHStepper->isRunning() && !this->LHStepper->isRunning()) {
				if (!digitalRead(LH_ENDSTOP_PIN)) {
					this->LHStepper->move(20);
				}
				if (!digitalRead(RH_ENDSTOP_PIN)) {
					this->RHStepper->move(20);
				}
				if (!this->LHStepper->isRunning() && !this->LHStepper->isRunning()) {
					this->MoveTo(1,1000);
					this->MyStatus = STATUS_HOMING_3;
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
		case STATUS_HOMING_3: // Moving upwards until e few millimeters aboove level 1
			if (!this->LHStepper->isRunning() && !this->LHStepper->isRunning()) {
				this->MoveTo(1,0);
				this->MyStatus = STATUS_HOMING_4;
				Serial.println(this->GetStatus());
			}
			break;
		case STATUS_HOMING_4: // Moving to level 1 and then resetting stepper positions to zero
			if (!this->LHStepper->isRunning() && !this->LHStepper->isRunning()) {
				this->LHStepper->setCurrentPosition(0);
				this->RHStepper->setCurrentPosition(0);
				this->CurrentLevel = 1;
				this->MyStatus = STATUS_IDLE;
				this->MoveTo(this->NextLevel,0);
			}
			break;
		case STATUS_MOVING:
			if (!this->LHStepper->isRunning() && !this->LHStepper->isRunning()) {
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
