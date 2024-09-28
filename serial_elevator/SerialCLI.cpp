#include "SerialCLI.h"

/*
 * There are three serial ports on the ESP known as U0UXD, U1UXD and U2UXD.
 * 
 * U0UXD is used to communicate with the ESP32 for programming and during reset/boot.
 * U1UXD is unused and can be used for your projects. Some boards use this port for SPI Flash access though
 * U2UXD is unused and can be used for your projects.
 * 
*/

void SerialCLI::Init(PosLogic *PL, Calibrator *CA) {
	this->myPL = PL;
	this->myCA = CA;
//	Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
	Serial.setTimeout(1);
}

void SerialCLI::HandleCommand(String  request) {
	Serial.println(); Serial.println("DEBUG SerialCLI HandleCommand");
	Serial.print("DEBUG "); Serial.println(request);

	String cmdarray[10];
	int index = 0;
	int startindex = 0;
	int cmd_length = 0;
	while (index < request.length()) {
		if (request[index] == ' ') {
			cmdarray[cmd_length] = request.substring(startindex, index);
			startindex = index+1;
			cmd_length++;
		}
		index++;
	}
	cmdarray[cmd_length] = request.substring(startindex);
	index++;
	cmd_length++;

	for (int i=0; i<cmd_length; i++) {
		Serial.print("DEBUG "); Serial.print(i); Serial.print(": "); Serial.println(cmdarray[i]);
	}

	if (cmdarray[0].equals("get")) {
		if (cmdarray[1].equals("status")) {
			Serial.println(this->myPL->GetStatus());
		} else if (cmdarray[1].equals("calibration")) {
			Serial.print("CALIBRATION "); Serial.print(cmdarray[2]); Serial.print(" "); Serial.print(this->myCA->GetOffset(true,cmdarray[2].toInt())); Serial.print(" "); Serial.println(this->myCA->GetOffset(false,cmdarray[2].toInt()));
		} else if (cmdarray[1].equals("lock")) {
			if (this->myPL->isLocked()) {
				Serial.println("LOCKED YES");
			} else {
				Serial.println("LOCKED NO");
			}
		} else {
			Serial.println("DEBUG Unknown HTLM element for get command");
		}
	} else if (cmdarray[0].equals("set")) {
		if (cmdarray[1].equals("calibration")) {
			this->myCA->SetOffset(cmdarray[2].toInt(), cmdarray[3].toInt(), cmdarray[4].toInt());
			this->myCA->Save();
			Serial.print("CALIBRATION "); Serial.print(cmdarray[2]); Serial.print(" "); Serial.print(cmdarray[3]); Serial.print(" "); Serial.println(cmdarray[4]);
		} else if (cmdarray[1].equals("lock")) {
			if (cmdarray[2].equals("YES")) {
				this->myPL->Lock();
			} else {
				this->myPL->Unlock();
			}
		} else if (cmdarray[1].equals("level")) {
			this->myPL->MoveTo(cmdarray[2].toInt(), 0);
		} else if (cmdarray[1].equals("levelfromcalibration")) {
			this->myPL->MoveToSteps(cmdarray[2].toInt(), cmdarray[3].toInt(), cmdarray[4].toInt());
			Serial.println(this->myPL->GetStatus());
		} else {
			Serial.println("DEBUG Unknown HTLM element for set command");
		}
	} else {
		Serial.println("DEBUG Unknown command");
	}
	Serial.println("DEBUG slut på SerialCLI HandleCommand");
}

void SerialCLI::Loop() {
	char lastChar;
	while (Serial.available()) {
		lastChar = this->cmdBuffer[this->readPos++] = char(Serial.read());
		if (lastChar == '\n') {
			// End of command
			this->cmdBuffer[this->readPos-1] = 0;
			this->HandleCommand(this->cmdBuffer);
			this->readPos = 0;
		}
	}
}
