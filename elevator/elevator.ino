#include "SDWebServer.h"
#include "OTAHandler.h"
#include "AjaxHandler.h"
#include "Calibrator.h"
#include "Display.h"
#include "PosLogic.h"

#define SD_CS_PIN       5
#define SD_D1_MOSI_PIN 23
#define SD_SCLK_PIN    18
#define SD_D0_MISO_PIN 19
#define I2C_SDA_PIN    21
#define I2C_SCL_PIN    22
#define DCC_PIN        34

SDWebServer   *WS;
OTAHandler    *OH;
AjaxHandler   *AH;
Calibrator    *CA;
Display       *DI;
PosLogic      *PL;

void SDWebServer_handleNotFound() {
	WS->loadFromSdCard(WS->getServer()->uri());
}

void setup(void) {
	Serial.begin(9600);
	Serial.setDebugOutput(true);
	Serial.print("\n");
	WS = new SDWebServer();
	WS->Init(SD_CS_PIN);
	WS->getServer()->onNotFound(SDWebServer_handleNotFound);
	OH = new OTAHandler();
	OH->Init();
	AH = new AjaxHandler();
	AH->Init(WS->getServer());
	CA = new Calibrator();
	CA->ReadFromSD();
	DI = new Display();
	DI->Init(WS);
	PL = new PosLogic();
	PL->Init(CA, DI);
}

void loop(void) {
	WS->getServer()->handleClient();
	WS->Loop();
	OH->Loop();
	AH->Loop();
	PL->Loop();
}
