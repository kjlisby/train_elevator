#IFNDEF SERIALCLI_H
#DEFINE SERIALCLI_H

#include <ESP32.h>
#include "PosLogic.h"
#include "Calibrator.h"

#define RXD2 16
#define TXD2 17


class SerialCLI {
	public:
		void Init(PosLgic *PL, Calibrator *CA);
		void SerialCLI::loop();

	private:
		void HandleCommand(char *cmd);
		PosLogic   *PL;
		Calibrator *CA;
		char cmdBuffer[200];
		int readPos = 0;
};

#ENDIF