#include "DccInterface.h"
#include "PosLogic.h"

extern PosLogic *PL;

void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) {
	Serial.print("DEBUG DCC Address: "); Serial.println(Addr);
	Serial.print("DEBUG DCC Direction: "); Serial.println(Direction);
	Serial.print("DEBUG DCC OutputPower: "); Serial.println(OutputPower);
	Serial.println("DEBUG DCC --------------------");
	
	int CurLev = PL->GetCurrentLevel();
	Serial.print("Elevator currently at "); Serial.println(CurLev);
	if (Addr == DECODER_ADDR_1 && Direction == 0) {
		Serial.println("DEBUG DCC Level 1");
		if (CurLev != 0 && CurLev != 1) {
			PL->MoveTo(1,0);
		}
	} else if (Addr == DECODER_ADDR_1 && Direction == 1) {
		Serial.println("DEBUG DCC Level 2");
		if (CurLev != 0 && CurLev != 2) {
			PL->MoveTo(2,0);
		}
	} else if (Addr == DECODER_ADDR_2 && Direction == 0) {
		Serial.println("DEBUG DCC Level 3");
		if (CurLev != 0 && CurLev != 3) {
			PL->MoveTo(3,0);
		}
	} else if (Addr == DECODER_ADDR_2 && Direction == 1) {
		Serial.println("DEBUG DCC Level 4");
		if (CurLev != 0 && CurLev != 4) {
			PL->MoveTo(4,0);
		}
	} else if (Addr == DECODER_ADDR_3 && Direction == 0) {
		Serial.println("DEBUG DCC Level 5");
		if (CurLev != 0 && CurLev != 5) {
			PL->MoveTo(5,0);
		}
	} else if (Addr == DECODER_ADDR_3 && Direction == 1) {
		Serial.println("DEBUG DCC Level 6");
		if (CurLev != 0 && CurLev != 6) {
			PL->MoveTo(6,0);
		}
	} else if (Addr == DECODER_ADDR_4 && Direction == 0) {
		Serial.println("DEBUG DCC Level 7");
		if (CurLev != 0 && CurLev != 7) {
			PL->MoveTo(1,0);
		}
	} else if (Addr == DECODER_ADDR_4 && Direction == 1) {
		Serial.println("DEBUG DCC Level 8");
		if (CurLev != 0 && CurLev != 8) {
			PL->MoveTo(2,0);
		}
	} else if (Addr == DECODER_ADDR_5 && Direction == 0) {
		Serial.println("DEBUG DCC Level 9");
		if (CurLev != 0 && CurLev != 9) {
			PL->MoveTo(3,0);
		}
	} else if (Addr == DECODER_ADDR_5 && Direction == 1) {
		Serial.println("DEBUG DCC Level 10");
		if (CurLev != 0 && CurLev != 10) {
			PL->MoveTo(4,0);
		}
	} else if (Addr == DECODER_ADDR_6 && Direction == 0) {
		Serial.println("DEBUG DCC Level 11");
		if (CurLev != 0 && CurLev != 11) {
			PL->MoveTo(5,0);
		}
	} else if (Addr == DECODER_ADDR_6 && Direction == 1) {
		Serial.println("DEBUG DCC Level 12");
		if (CurLev != 0 && CurLev != 12) {
			PL->MoveTo(12,0);
		}
	}
	Serial.println("DEBUG DCC --------------------");
}

void DccInterface::Init () {
	this->MyDcc = new NmraDcc();
	this->MyDcc->pin(digitalPinToInterrupt(DCC_PIN), DCC_PIN, false);
	this->MyDcc->init(MAN_ID_DIY, 1, FLAGS_DCC_ACCESSORY_DECODER | FLAGS_OUTPUT_ADDRESS_MODE, 0);
	this->MyDcc->setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, DECODER_ADDR_1 & 0xFF);
	this->MyDcc->setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, DECODER_ADDR_1 >> 8);
}

void DccInterface::Loop () {
	this->MyDcc->process();
}