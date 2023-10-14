#include "PosLogic.h"
#include "JSONArray.h"

void PosLogic::Init (Calibrator *CA, Display *DI, SDWebServer *WS) {
	this->MyCalibrator = CA;
	this->MyDisplay = DI;
	this->MyWebServer = WS;
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
	Serial.println("PL Init Done");
}

bool PosLogic::Home () {
	Serial.println("PosLog::Home");
	if (this->Blocked()) {
		Serial.println("ELEVATOR_BLOCKED");
		return false;
	}
	this->MyStatus = STATUS_HOMING_1;
	this->MyDisplay->Homing();
	this->MyWebServer->sendMessage(this->GetStatus());
	return true;
}
	
bool PosLogic::MoveTo (int Level, int AdditionalSteps) {
	Serial.println("PosLogic::MoveTo");
	if (this->Blocked()) {
		Serial.println("ELEVATOR_BLOCKED");
		return false;
	}
	if (this->Locked) {
		Serial.println("ELEVATOR_LOCKED");
		return false;
	}
	if (this->MyStatus != STATUS_IDLE) {
		Serial.print("ELEVATOR NOT IDLE"); Serial.println(this->GetStatus());
		return false;
	}
	this->MyStatus  = STATUS_MOVING;
	this->NextLevel = Level;
	this->LHStepper->moveTo(this->MyCalibrator->GetOffset(true,  Level)+AdditionalSteps);
	this->RHStepper->moveTo(this->MyCalibrator->GetOffset(false, Level)+AdditionalSteps);
	this->MyDisplay->NewLevel(Level);
	this->MyWebServer->sendMessage(this->GetStatus());
	return true;
}

bool PosLogic::MoveToSteps (int Level, int StepsLeft, int StepsRight) {
	Serial.println("PosLogic::MoveToSteps");
	if (this->Blocked()) {
		Serial.println("ELEVATOR_BLOCKED");
		return false;
	}
	if (this->Locked) {
		Serial.println("ELEVATOR_LOCKED");
		return false;
	}
	if (this->MyStatus != STATUS_IDLE) {
		Serial.print("ELEVATOR NOT IDLE"); Serial.println(this->GetStatus());
		return false;
	}
	this->MyStatus  = STATUS_MOVING;
	this->NextLevel = Level;
	this->LHStepper->moveTo(StepsLeft);
	this->RHStepper->moveTo(StepsRight);
	this->MyDisplay->NewLevel(Level);
	this->MyWebServer->sendMessage(this->GetStatus());
	return true;
}

void PosLogic::Lock () {
	Serial.println("PosLogic::Lock");
	this->Locked = true;
}

void PosLogic::Unlock () {
	Serial.println("PosLogic::Unlock");
	this->Locked = false;
}
	
String PosLogic::GetStatus () {
	Serial.println("PosLogic::GetStatus");
	String retval = JSON_ArrayStart();
	if (this->Blocked()) {
		retval += JSON_item("STATUS", "BLOCKED");
	}
	switch (this->MyStatus) {
		case STATUS_HOMING_1:
			retval += JSON_item("STATUS", "HOMING1");
			break;
		case STATUS_HOMING_2:
			retval += JSON_item("STATUS", "HOMING2");
			break;
		case STATUS_HOMING_3:
			retval += JSON_item("STATUS", "HOMING3");
			break;
		case STATUS_HOMING_4:
			retval += JSON_item("STATUS", "HOMING4");
			break;
		case STATUS_MOVING:
			retval += JSON_item("STATUS", "MOVING");
			retval += JSON_ArrayDivider()+JSON_item("FROM", String(this->CurrentLevel));
			retval += JSON_ArrayDivider()+JSON_item("TO", String(this->NextLevel));
			break;
		case STATUS_IDLE:
			retval += JSON_item("STATUS", "IDLE");
			retval += JSON_ArrayDivider()+JSON_item("LEVEL", String(this->CurrentLevel));
			break;
	}
	retval += JSON_ArrayEnd();
	Serial.print("STATUS IS "); Serial.println(retval);
	return retval;
}

bool PosLogic::Blocked () {
	Serial.println("PosLogic::Blocked");
return false;
	return (!digitalRead(LH_BACK_SECURITY_PIN) || !digitalRead(LH_FRONT_SECURITY_PIN) || !digitalRead(RH_BACK_SECURITY_PIN) || !digitalRead(RH_FRONT_SECURITY_PIN));
}

int PosLogic::GetCurrentLevel () {
	Serial.println("PosLogic::GetCurrentLevel");
	if (this->Blocked() || this->MyStatus != STATUS_IDLE) {
		return 0;
	}
	return this->CurrentLevel;
}

bool PosLogic::isRunning() {
	return this->LHStepper->isRunning() || this->RHStepper->isRunning();
}

void PosLogic::Loop () {
	this->LHStepper->run();
	this->RHStepper->run();
	switch (this->MyStatus) {
		case STATUS_HOMING_1: // Moving downwards searching for end-stop
			if (!digitalRead(LH_ENDSTOP_PIN) && !digitalRead(LH_ENDSTOP_PIN)) {
				this->MyStatus = STATUS_HOMING_2;
				this->MyWebServer->sendMessage(this->GetStatus());
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
					this->MyWebServer->sendMessage(this->GetStatus());
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
				this->MyWebServer->sendMessage(this->GetStatus());
			}
			break;
		case STATUS_HOMING_4: // Moving to level 1 and then resetting stepper positions to zero
			if (!this->LHStepper->isRunning() && !this->LHStepper->isRunning()) {
				this->LHStepper->setCurrentPosition(0);
				this->RHStepper->setCurrentPosition(0);
				this->CurrentLevel = 1;
				this->MyStatus = STATUS_IDLE;
				this->MyDisplay->AtLevel(this->CurrentLevel);
				this->MyWebServer->sendMessage(this->GetStatus());
			}
			break;
		case STATUS_MOVING:
			if (!this->LHStepper->isRunning() && !this->LHStepper->isRunning()) {
				this->CurrentLevel = this->NextLevel;
				this->MyStatus = STATUS_IDLE;
				this->MyDisplay->AtLevel(this->CurrentLevel);
				this->MyWebServer->sendMessage(this->GetStatus());
			}
			break;
		default:;
			// idling - do nothing.
	}
}
