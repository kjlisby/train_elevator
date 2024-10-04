#ifndef TV_REMOTE_H
#define TV_REMOTE_H

class TVRemote {
  public:
    void Init ();
    void Loop ();
  private:
    unsigned long button1_PushedMillis = 0;
    unsigned long anyButtonPushedMillis = 0;
    int newLevel = 0;
};

#endif