#ifndef POS_LOGIC_H
#define POS_LOGIC_H

#include <AccelStepper.h>
#include "Calibrator.h"
#include "Display.h"
#include "SDWebServer.h"

#define LH_STEPPER_DIR_PIN 12
#define LH_STEPPER_STEP_PIN 13
#define RH_STEPPER_DIR_PIN 32
#define RH_STEPPER_STEP_PIN 33
#define LH_ENDSTOP_PIN 17
#define RH_ENDSTOP_PIN 16
#define LH_BACK_SECURITY_PIN 4
#define LH_FRONT_SECURITY_PIN 25
#define RH_BACK_SECURITY_PIN 2
#define RH_FRONT_SECURITY_PIN 26

#define STATUS_IDLE     0
#define STATUS_HOMING_1 1
#define STATUS_HOMING_2 2
#define STATUS_HOMING_3 3
#define STATUS_HOMING_4 4
#define STATUS_MOVING   5

class PosLogic {
	public:
		void   Init (Calibrator *CA, Display *DI, SDWebServer *WS);
		bool   Home ();
		void   Lock ();
		void   Unlock ();
		bool   MoveTo (int Level, int AdditionalSteps);
		bool   MoveToSteps (int Level, int StepsLeft, int StepsRight);
		String GetStatus ();
		int    GetCurrentLevel ();
		bool   isRunning();
		void   Loop ();
	private:
		AccelStepper *LHStepper;
		AccelStepper *RHStepper;
		Calibrator   *MyCalibrator;
		Display      *MyDisplay;
		SDWebServer  *MyWebServer;
		int          MyStatus = STATUS_IDLE;
		int          CurrentLevel = 0;
		int          NextLevel    = 0;
		bool         Locked = false;
		bool         Blocked();
};

#endif
