#include "TVRemote.h"
#include <IRremote.hpp>
#include "PosLogic.h"

extern PosLogic *PL;

void TVRemote::Init () {
  IrReceiver.begin(18);
}

void TVRemote::Loop () {
  this->newLevel = 0;
  if (!IrReceiver.decode()) {
    if (this->button1_PushedMillis > 0) {
      if (millis()-this->button1_PushedMillis > 2000) {
        this->newLevel = 1;
        this->button1_PushedMillis = 0;
      }
    }
  } else {
    if (millis()-this->anyButtonPushedMillis < 500) {
      IrReceiver.resume(); // Enable receiving of the next value
      return;
    }
    this->anyButtonPushedMillis = millis();
    if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x04) {
      Serial.println("TV remote 1 pressed");
      if (this->button1_PushedMillis > 0) {
        this->newLevel = 11;
        this->button1_PushedMillis = 0;
      } else {
        this->button1_PushedMillis = millis();
      }
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x05) {
      Serial.println("TV remote 2 pressed");
      if (this->button1_PushedMillis > 0) {
        this->newLevel = 12;
      } else {
        this->newLevel = 2;
      }
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x06) {
      Serial.println("TV remote 3 pressed");
      this->newLevel = 3;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x08) {
      Serial.println("TV remote 4 pressed");
      this->newLevel = 4;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x09) {
      Serial.println("TV remote 5 pressed");
      this->newLevel = 5;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x0A) {
      Serial.println("TV remote 6 pressed");
      this->newLevel = 6;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x0C) {
      Serial.println("TV remote 7 pressed");
      this->newLevel = 7;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x0D) {
      Serial.println("TV remote 8 pressed");
      this->newLevel = 8;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x0E) {
      Serial.println("TV remote 9 pressed");
      this->newLevel = 9;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x11) {
      Serial.println("TV remote 0 pressed");
      if (this->button1_PushedMillis > 0) {
        this->newLevel = 10;
      }
      this->button1_PushedMillis = 0;
    }
    IrReceiver.resume(); // Enable receiving of the next value
  }
  if (this->newLevel > 0) {
    Serial.print("TVRemote new level: "); Serial.println(this->newLevel);
    PL->MoveTo(this->newLevel,0);
  }
}