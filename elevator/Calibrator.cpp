#include "Calibrator.h"
#include <SD.h>

void Calibrator::SetOffset (int Level, long NewLeftValue, long NewRightValue) {
  this->offset[Level-1][1] = NewLeftValue;
  this->offset[Level-1][0] = NewRightValue;
}

long Calibrator::GetOffset (bool Left, int Level) {
  if (Left) {
    return this->offset[Level-1][1];
  } else {
    return this->offset[Level-1][0];
  }
}

void Calibrator::ReadFromSD () {
  char buffer[20];
  int i = 0;
  int level = 1;
  int side = 0;
  File myFile = SD.open(this->FileName);
  if (myFile) {
    do {
      buffer[i++] = myFile.read();
      if (buffer[i] == '_') {
        buffer[i] = '\0';
        this->offset[level-1][side] = atol(buffer);
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
  File myFile = SD.open(this->FileName, FILE_WRITE);
  if (myFile) {
    for (int level = 1; level <= 12; level++) {
      for (int side = 0; side <= 1; side++) {
        myFile.print (this->offset[level-1][side]);
        myFile.print ('_');
      }
    }
    close(myFile);
  }
}
