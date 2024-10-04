#include "SerialCLI.h"
#include "Calibrator.h"
#include "Display.h"
#include "PosLogic.h"
#include "DccInterface.h"
#include "Relays.h"
#include "IRsensor.h"
#include "TVRemote.h"


#define I2C_SDA_PIN    21
#define I2C_SCL_PIN    22

SerialCLI     *SC;
Calibrator    *CA;
Display       *DI;
Relays        *RE;
PosLogic      *PL;
DccInterface  *DC;
IRsensor      *IR1;
IRsensor      *IR2;
IRsensor      *IR3;
IRsensor      *IR4;
TVRemote      *TVR;


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
  RE = new Relays();
  RE->Init();
  Serial.println("DEBUG RE started");
  IR1 = new IRsensor();
  IR1->Init(36);
  IR2 = new IRsensor();
  IR2->Init(39);
  IR3 = new IRsensor();
  IR3->Init(34);
  IR4 = new IRsensor();
  IR4->Init(35);
  PL = new PosLogic();
  PL->Init(CA, DI, RE, IR1, IR2, IR3, IR4);
  Serial.println("DEBUG PL started");
  SC = new SerialCLI();
  SC->Init(PL, CA);
  DC = new DccInterface();
  DC->Init();
  TVR = new TVRemote();
  TVR->Init();
}

int val;
int lastState = 0;
int loopcounter = 0;
void loop(void) {
  if (!PL->isRunning()) {
    // Doing time-consuming stuff (such as displaying an HTML page) will prohibit smooth movement of the steppers
    SC->Loop();
    DC->Loop();
    IR1->Loop();
    IR2->Loop();
    IR3->Loop();
    IR4->Loop();
    TVR->Loop();
  }
  PL->Loop();



  if (loopcounter++ > 50000) {
    loopcounter = 0;
    val = analogRead(36);
    Serial.println(" "); Serial.print("val "); Serial.print(val);
    if (IR1->TrainSeen() || IR2->TrainSeen() || IR3->TrainSeen() || IR4->TrainSeen()) {
      Serial.println(" TRAIN SEEN");
    } else {
      Serial.println(" NO TRAIN");
    }
  }
}
