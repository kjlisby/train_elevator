#include "Calibrator.h"
#include <SD.h>

void Calibrator::SetOffset (int Level, long NewLeftValue, long NewRightValue) {
  offset[Level-1][1] = NewLeftValue;
  offset[Level-1][0] = NewRightValue;
}

long Calibrator::GetOffset (bool Left, int Level) {
  if (Left) {
    return offset[Level-1][1];
  } else {
    return offset[Level-1][0];
  }
}

void Calibrator::ReadFromSD () {
  char buffer[20];
  int i = 0;
  int level = 1;
  int side = 0;
  myFile = SD.open(Calibrator::FileName);
  if (myFile) {
    do {
      buffer[i++] = myFile.read();
      if (buffer[i] == '-1') {
        buffer[i] = '\0';
        offset[level-1][side] = atol(buffer);
        i = 0;
        if (side == 1) {
          side = 0;
          level++;
        } else {
          side = 1;
        }
      }
    } while (buffer[i] != -1);
    close (myFile);
  }
}

void Calibrator::WriteToSD () {
  myFile = SD.open(Calibrator::FileName, FILE_WRITE);
  if (myFile) {
    for (level = 1; level <= 12; level++) {
      for (side = 0; side <= 1; side++) {
        myFile.print (offset[level-1][side]);
        myFile.print ('_');
      }
    }
    close(myFile);
  }
}
