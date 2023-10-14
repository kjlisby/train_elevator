/*
 * There are three serial ports on the ESP known as U0UXD, U1UXD and U2UXD.
 * 
 * U0UXD is used to communicate with the ESP32 for programming and during reset/boot.
 * U1UXD is unused and can be used for your projects. Some boards use this port for SPI Flash access though
 * U2UXD is unused and can be used for your projects.
 * 
*/

void SerialCLI::Init(PosLgic *PL, Calibrator *CA) {
	this->PL = PL;
	this->CA = CA;
	Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
	Serial2.setTimeout(1);
}

void SerialCLI::HandleCommand(String  request) {
	Serial.println(); Serial.println("SerialCLI GETINPUT");
	Serial.println(request);

	String cmdarray[10];
	int index = 0;
	int startindex = 0;
	int cmd_length = 0;
	while (index < request.length()) {
		if (requestUri[index] == '_') {
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
		Serial.print(i); Serial.print(": ");
		Serial.println(cmdarray[i]);
	}

	if (cmdarray[1].startsWith("getstatus")) {
		Serial2.print(PL->GetStatus());
	} else if (cmdarray[1].equals("get")) {
		if (cmdarray[2].equals("calibration")) {
			Serial2.print(GetCalibration(cmdarray[3].toInt()));
		} else {
			Serial2.print("Unknown HTLM element for get command");
		}
	} else if (cmdarray[1].equals("set")) {
		if (cmdarray[2].equals("calibration")) {
			Serial2.print(SetCalibration(cmdarray[3].toInt(), cmdarray[4].toInt(), cmdarray[5].toInt()));
		} else if (cmdarray[2].equals("level")) {
			Serial2.print(MoveToLevel(cmdarray[3].toInt()));
		} else if (cmdarray[2].equals("levelfromcalibration")) {
			Serial2.print(MoveToLevelFromCalibration(cmdarray[3].toInt(), cmdarray[4].toInt(), cmdarray[5].toInt()));
		} else {
			Serial2.print("Unknown HTLM element for set command");
		}
	} else {
		Serial2.print("Unknown AJAX command");
	}
	Serial.println(" slut på internalAjaxHandler handle");
}

void SerialCLI::loop() {
	char lastChar;
	while (Serial2.available()) {
		lastChar = this->cmdBuffer[this->readPos++] = char(Serial2.read());
		if (lastChar == '#') {
			// End of command
			this->cmdBuffer[this->readPos-1] = 0;
			this->HandleCommand(this->cmdBuffer);
			this->readPos = 0;
	}
}
