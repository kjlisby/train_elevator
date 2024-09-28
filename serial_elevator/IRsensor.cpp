#include "IRsensor.h"
#include <Arduino.h>

bool IRsensor::TrainSeen () {
	if (this->trainDetectionMillis == 0) {
		return false;
	} else {
		return true;
	}
}

void IRsensor::Loop () {
	if (analogRead(this->IR_pin) < 3000) {
    if (millis() - this->trainDetectionMillis > 20000) {
      this->trainDetectionMillis = 0;
    }
  } else {
    this->trainDetectionMillis = millis();
  }
}

void IRsensor::Init (uint8_t Pin) {
	this->trainDetectionMillis = 0;
	this->IR_pin = Pin;
}