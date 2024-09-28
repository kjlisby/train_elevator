#ifndef IRSENSOR_H
#define IRSENSOR_H

#include <Arduino.h>

class IRsensor {
	public:
		void   Init (uint8_t Pin);
		bool   TrainSeen ();
		void   Loop();
	private:
		unsigned long trainDetectionMillis;
		uint8_t IR_pin;
};

#endif