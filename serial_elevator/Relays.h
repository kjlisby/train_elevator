#ifndef RELAYS_H
#define RELAYS_H

#include <Wire.h>

#define port_extender_address 0x20

class Relays {
  public:
    void Init     ();
    void AtLevel  (int Level);

  private:
    int CurrentLevel = 0;
};
#endif