#ifndef POS_LOGIC_H
#define POS_LOGIC_H

#include <AccelStepper.h>

#define LH_STEPPER_DIR_PIN 12
#define LH_STEPPER_STEP_PIN 13
#define RH_STEPPER_DIR_PIN 32
#define RH_STEPPER_STEP_PIN 33
#define LH_ENDSTOP_PIN 17
#define RH_ENDSTOP_PIN 16
#define LH_BACK_SECURITY_PIN 4
#define LH_FRONT_SECURITY_PIN 9
#define RH_BACK_SECURITY_PIN 2
#define RH_FRONT_SECURITY_PIN 15

class PosLogic {
	public:
		void Init ();
		void Home ();
		void MoveTo (int Level);
		String GetStatus ();
	private:
		AccelStepper *LHStepper;
		AccelStepper *RHStepper;
		uint8_t LHEndStopPin   = LH_ENDSTOP_PIN;
		uint8_t RHEndStopPin   = RH_ENDSTOP_PIN;
		uint8_t LBSecurityPin  = LH_BACK_SECURITY_PIN;
		uint8_t LFSecurityPin  = LH_FRONT_SECURITY_PIN;
		uint8_t RBSecurityPin  = RH_BACK_SECURITY_PIN;
		uint8_t RFSecurityPin  = RH_FRONT_SECURITY_PIN;
};

#endif