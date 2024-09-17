// The Relays class is giving power only to the track that is currently at portal level

#include "Relays.h"
#include <Arduino.h>

void Relays::Init () {
  Wire.begin(); // wake up I2C bus
  // set I/O pins to outputs
  Wire.beginTransmission(0x20);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // set all of port A to outputs
  Wire.endTransmission();
  Wire.beginTransmission(0x20);
  Wire.write(0x01); // IODIRB register
  Wire.write(0x00); // set all of port B to outputs
  Wire.endTransmission();
  this->AtLevel(0);
}

void Relays::AtLevel (int Level) {
  Serial.println("DEBUG Relays::AtLevel");
  Serial.println(Level);
  unsigned int PortA = 0xFF;
  unsigned int PortB = 0xFF;
  if (Level > 0 && Level <= 6) {
    PortA -= 1 << (Level-1);
  } else if (Level > 6) {
    PortB -= 1 << (Level-7);
  }
  Serial.println(PortA);
  Serial.println(PortB);
  Wire.beginTransmission(0x20);
  Wire.write(0x12); // GPIOA
  Wire.write(PortA); // port A
  Wire.endTransmission();
  Wire.beginTransmission(0x20);
  Wire.write(0x13); // GPIOB
  Wire.write(PortB); // port B
  Wire.endTransmission();
}
