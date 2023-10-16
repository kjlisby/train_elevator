#include "SDWebServer.h"
#include "OTAHandler.h"
#include "AjaxHandler.h"
#include "Calibrator.h"
#include "Display.h"
#include "PosLogic.h"
#include "DccInterface.h"

#define SD_CS_PIN       5
#define SD_D1_MOSI_PIN 23
#define SD_SCLK_PIN    18
#define SD_D0_MISO_PIN 19
#define I2C_SDA_PIN    21
#define I2C_SCL_PIN    22

SDWebServer   *WS;
OTAHandler    *OH;
AjaxHandler   *AH;
Calibrator    *CA;
Display       *DI;
PosLogic      *PL;
DccInterface  *DC;


void setup(void) {
	Serial.begin(9600);
	Serial.setDebugOutput(true);
	Serial.print("\n");
	WS = new SDWebServer();
	WS->Init(SD_CS_PIN);
  Serial.println("WS started");
	OH = new OTAHandler();
	OH->Init();
  Serial.println("OH started");
	AH = new AjaxHandler();
	AH->Init(WS->getServer());
  Serial.println("AH started");
	CA = new Calibrator();
  Serial.println("before CA->ReadFromSD");
	CA->ReadFromSD();
  Serial.println("CA started");
	DI = new Display();
	DI->Init();
  Serial.println("DI started");
	PL = new PosLogic();
	PL->Init(CA, DI, WS);
  Serial.println("PL started");
	// DC = new DccInterface();
	// DC->Init();
}

void loop(void) {
  //Serial.print(".");
	if (!PL->isRunning()) {
  //Serial.print("-");
		// Doing time-consuming stuff (such as displaying an HTML page) will prohibit smooth movement of the steppers
		WS->Loop();
  //Serial.print("|");
		OH->Loop();
  //Serial.print(":");
		AH->Loop();
//		DC->Loop();
	}
  //Serial.print("#");
	PL->Loop();
}
