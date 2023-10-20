#include "SerialCLI.h"
#include "Calibrator.h"
#include "Display.h"
#include "PosLogic.h"
#include "DccInterface.h"


#define I2C_SDA_PIN    21
#define I2C_SCL_PIN    22

SerialCLI     *SC;
Calibrator    *CA;
Display       *DI;
PosLogic      *PL;
DccInterface  *DC;


void setup(void) {
	Serial.begin(19200);
	delay(5000);
	Serial.setDebugOutput(true);
	Serial.println("DEBUG Starting...");
	CA = new Calibrator();
	CA->Init();
	Serial.println("DEBUG CA started");
	DI = new Display();
	DI->Init();
	Serial.println("DEBUG DI started");
	PL = new PosLogic();
	PL->Init(CA, DI);
	Serial.println("DEBUG PL started");
	SC = new SerialCLI();
	SC->Init(PL, CA);
	DC = new DccInterface();
	DC->Init();
}

void loop(void) {
	if (!PL->isRunning()) {
		// Doing time-consuming stuff (such as displaying an HTML page) will prohibit smooth movement of the steppers
		SC->Loop();
		DC->Loop();
	}
	PL->Loop();
}
