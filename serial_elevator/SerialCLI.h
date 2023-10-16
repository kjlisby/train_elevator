#ifndef SERIALCLI_H
#define SERIALCLI_H

#include "PosLogic.h"
#include "Calibrator.h"

#define RXD2 16
#define TXD2 17


class SerialCLI {
	public:
		void Init(PosLogic *PL, Calibrator *CA);
		void Loop();

	private:
		void HandleCommand(String request);
		PosLogic   *PL;
		Calibrator *CA;
		char cmdBuffer[200];
		int readPos = 0;
};

#endif