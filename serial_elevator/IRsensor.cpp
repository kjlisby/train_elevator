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
  if (analogRead(this->IR_pin) < 2000) {
    if (millis() - this->trainDetectionMillis > 20000) {
      this->trainDetectionMillis = 0;
    }
    this->loopCounter = 0;
  } else {
    if (this->loopCounter++ > 100) {
      this->trainDetectionMillis = millis();
      this->loopCounter = 0;
    }
  }
}

void IRsensor::Init (uint8_t Pin) {
  this->trainDetectionMillis = 0;
  this->IR_pin = Pin;
}