#include "TVRemote.h"
#include <IRremote.hpp>
#include "PosLogic.h"

extern PosLogic *PL;

void TVRemote::Init (Display *DI) {
  IrReceiver.begin(18);
  this->MyDisplay = DI;
}

void TVRemote::Loop (bool ThrowKeysAway) {
  bool AtLeastaSecond = millis()-this->anyButtonPushedMillis >= 1000;
  this->newLevel = 0;
  if (!IrReceiver.decode()) {
    if (this->button1_PushedMillis > 0) {
      if (millis()-this->button1_PushedMillis > 3000) {
        this->MyDisplay->TVRemote(" 1");
        this->newLevel = 1;
        this->button1_PushedMillis = 0;
      }
    }
  } else {
    if (ThrowKeysAway) {
      IrReceiver.resume(); // Enable receiving of the next value
      return;
    }
    if (millis()-this->anyButtonPushedMillis < 500) {
      IrReceiver.resume(); // Enable receiving of the next value
      return;
    }
    this->anyButtonPushedMillis = millis();
    IrReceiver.printIRSendUsage(&Serial);
    if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x04) {
      Serial.println("TV remote 1 pressed");
      if (this->button1_PushedMillis > 0) {
        this->MyDisplay->TVRemote("11");
        this->newLevel = 11;
        this->button1_PushedMillis = 0;
      } else {
        this->MyDisplay->TVRemote("1-");
        this->button1_PushedMillis = millis();
      }
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x05) {
      Serial.println("TV remote 2 pressed");
      if (this->button1_PushedMillis > 0) {
        this->MyDisplay->TVRemote("12");
        this->newLevel = 12;
      } else {
        this->MyDisplay->TVRemote(" 2");
        this->newLevel = 2;
      }
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x06) {
      Serial.println("TV remote 3 pressed");
      this->MyDisplay->TVRemote(" 3");
      this->newLevel = 3;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x08) {
      Serial.println("TV remote 4 pressed");
      this->MyDisplay->TVRemote(" 4");
      this->newLevel = 4;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x09) {
      Serial.println("TV remote 5 pressed");
      this->MyDisplay->TVRemote(" 5");
      this->newLevel = 5;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x0A) {
      Serial.println("TV remote 6 pressed");
      this->MyDisplay->TVRemote(" 6");
      this->newLevel = 6;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x0C) {
      Serial.println("TV remote 7 pressed");
      this->MyDisplay->TVRemote(" 7");
      this->newLevel = 7;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x0D) {
      Serial.println("TV remote 8 pressed");
      this->MyDisplay->TVRemote(" 8");
      this->newLevel = 8;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x0E) {
      Serial.println("TV remote 9 pressed");
      this->MyDisplay->TVRemote(" 9");
      this->newLevel = 9;
      this->button1_PushedMillis = 0;
    } else if (IrReceiver.decodedIRData.address == 0x07 && IrReceiver.decodedIRData.command == 0x11) {
      Serial.println("TV remote 0 pressed");
      if (this->button1_PushedMillis > 0) {
        this->MyDisplay->TVRemote("10");
        this->newLevel = 10;
      }
      this->button1_PushedMillis = 0;
    } else if (AtLeastaSecond && (IrReceiver.decodedIRData.address == 0x07 && (IrReceiver.decodedIRData.command == 0x12) || (IrReceiver.decodedIRData.command == 0x07))) {
      Serial.println("TV remote UP pressed");
      if (PL->GetCurrentLevel() < 12) {
        this->newLevel = PL->GetCurrentLevel() + 1;
      }
      this->button1_PushedMillis = 0;
    } else if (AtLeastaSecond && (IrReceiver.decodedIRData.address == 0x07 && (IrReceiver.decodedIRData.command == 0x10) || (IrReceiver.decodedIRData.command == 0x0B))) {
      Serial.println("TV remote DOWN pressed");
      if (PL->GetCurrentLevel() > 1) {
        this->newLevel = PL->GetCurrentLevel() - 1;
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
