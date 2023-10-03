#include "PosLogic.h"

void PosLogic::Init () {
	this->LHStepper = new AccelStepper(AccelStepper::DRIVER, LH_STEPPER_STEP_PIN, LH_STEPPER_DIR_PIN);
	this->RHStepper = new AccelStepper(AccelStepper::DRIVER, RH_STEPPER_STEP_PIN, RH_STEPPER_DIR_PIN);
	pinMode(this->LHEndStopPin, INPUT_PULLUP);
	pinMode(this->RHEndStopPin, INPUT_PULLUP);
	pinMode(this->LBSecurityPin, INPUT_PULLUP);
	pinMode(this->LFSecurityPin, INPUT_PULLUP);
	pinMode(this->RBSecurityPin, INPUT_PULLUP);
	pinMode(this->RFSecurityPin, INPUT_PULLUP);
}


void PosLogic::Home () {
}
	
void PosLogic::MoveTo (int Level) {
}
	
String PosLogic::GetStatus () {
}
