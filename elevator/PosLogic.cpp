#include "PosLogic.h"

void PosLogic::Init (Calibrator *CA, Display *DI) {
	this->MyCalibrator = CA;
	this->MyDisplay = DI;
	this->LHStepper = new AccelStepper(AccelStepper::DRIVER, LH_STEPPER_STEP_PIN, LH_STEPPER_DIR_PIN);
	this->LHStepper->setMaxSpeed(200.0);
	this->LHStepper->setAcceleration(100.0);
	this->RHStepper = new AccelStepper(AccelStepper::DRIVER, RH_STEPPER_STEP_PIN, RH_STEPPER_DIR_PIN);
	this->RHStepper->setMaxSpeed(200.0);
	this->RHStepper->setAcceleration(100.0);
	pinMode(LH_ENDSTOP_PIN, INPUT_PULLUP);
	pinMode(RH_ENDSTOP_PIN, INPUT_PULLUP);
	pinMode(LH_BACK_SECURITY_PIN,  INPUT_PULLUP);
	pinMode(LH_FRONT_SECURITY_PIN, INPUT_PULLUP);
	pinMode(RH_BACK_SECURITY_PIN,  INPUT_PULLUP);
	pinMode(RH_FRONT_SECURITY_PIN, INPUT_PULLUP);
}

bool PosLogic::Home () {
	if (this->Blocked()) {
		Serial.println("ELEVATOR_BLOCKED");
		return false;
	}
	this->MyStatus = STATUS_HOMING_1;
	return true;
}
	
bool PosLogic::MoveTo (int Level, int AdditionalSteps) {
	if (this->Blocked()) {
		Serial.println("ELEVATOR_BLOCKED");
		return false;
	}
	if (this->Locked) {
		Serial.println("ELEVATOR_LOCKED");
		return false;
	}
	this->MyStatus  = STATUS_MOVING;
	this->NextLevel = Level;
	this->LHStepper->moveTo(this->MyCalibrator->GetOffset(true,  Level)+AdditionalSteps);
	this->RHStepper->moveTo(this->MyCalibrator->GetOffset(false, Level)+AdditionalSteps);
	this->MyDisplay->NewLevel(Level);
	return true;
}

bool PosLogic::Lock () {
this->Locked = true;
}

bool PosLogic::Unlock () {
this->Locked = false;
}
	
String PosLogic::GetStatus () {
	if (this->Blocked()) {
		return ("BLOCKED");
	}
	if (this->Locked) {
		return ("LOCKED");
	}
	switch (this->MyStatus) {
		case STATUS_HOMING_1:
			return ("HOMING phase 1");
		case STATUS_HOMING_2:
			return ("HOMING phase 2");
		case STATUS_HOMING_3:
			return ("HOMING phase 3");
		case STATUS_HOMING_4:
			return ("HOMING phase 4");
		case STATUS_MOVING:
			return ("MOVING from "+String(this->CurrentLevel)+" to "+String(this->NextLevel));
		case STATUS_IDLE:
			return ("STEADY at "+String(this->CurrentLevel));
	}
	return ("UNKNOWN STATE");
}

bool PosLogic::Blocked () {
	return (!digitalRead(LH_BACK_SECURITY_PIN) || !digitalRead(LH_FRONT_SECURITY_PIN) || !digitalRead(RH_BACK_SECURITY_PIN) || !digitalRead(RH_FRONT_SECURITY_PIN));
}

int PosLogic::GetCurrentLevel () {
	if (this->Blocked() || this->MyStatus != STATUS_IDLE) {
		return 0;
	}
	return this->CurrentLevel;
}

void PosLogic::Loop () {
	this->LHStepper->run();
	this->RHStepper->run();
	switch (this->MyStatus) {
		case STATUS_HOMING_1: // Moving downwards searching for end-stop
			if (!digitalRead(LH_ENDSTOP_PIN) && !digitalRead(LH_ENDSTOP_PIN)) {
				this->MyStatus = STATUS_HOMING_2;
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
			}
			break;
		case STATUS_HOMING_4: // Moving to level 1 and then resetting stepper positions to zero
			if (!this->LHStepper->isRunning() && !this->LHStepper->isRunning()) {
				this->LHStepper->setCurrentPosition(0);
				this->RHStepper->setCurrentPosition(0);
				this->MyStatus = STATUS_IDLE;
			}
			break;
		case STATUS_MOVING:
			if (!this->LHStepper->isRunning() && !this->LHStepper->isRunning()) {
				this->CurrentLevel = this->NextLevel;
				this->MyStatus = STATUS_IDLE;
				this->MyDisplay->AtLevel(this->CurrentLevel);
			}
			break;
		default:;
			// idling - do nothing.
	}
}
