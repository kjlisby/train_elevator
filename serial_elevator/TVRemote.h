#ifndef TV_REMOTE_H
#define TV_REMOTE_H

#include "Display.h"

class TVRemote {
  public:
    void Init (Display *DI);
    void Loop (bool ThrowKeysAway);
  private:
    Display *MyDisplay;
    unsigned long button1_PushedMillis = 0;
    unsigned long anyButtonPushedMillis = 0;
    int newLevel = 0;
};

#endif