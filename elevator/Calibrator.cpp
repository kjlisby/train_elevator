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
  Serial.println("Calibrator::ReadFromSD 1");
	char buffer[200];
	char latest_char;
	int i = 0;
	int level = 1;
	int side = 0;
	File myFile = SD.open(this->FileName, FILE_READ);
  Serial.println("Calibrator::ReadFromSD 2");
	if (myFile) {
		do {
			latest_char = buffer[i++] = myFile.read();
  Serial.print("Calibrator::ReadFromSD 3 "); Serial.println(latest_char);
			if (latest_char == '_' || latest_char == 'S') {
  Serial.println("Calibrator::ReadFromSD 4");
				buffer[i-1] = '\0';
				this->offset[level-1][side] = atol(buffer);
  Serial.print("Calibrator::ReadFromSD 5 "); Serial.println(buffer);
				i = 0;
				if (side == 1) {
					side = 0;
					level++;
				} else {
					side = 1;
				}
			}
  Serial.println("Calibrator::ReadFromSD 6");
		} while (latest_char != 'S');
  Serial.println("Calibrator::ReadFromSD 7");
		close (myFile);
	}
  Serial.println("Calibrator::ReadFromSD 8");
}

void Calibrator::WriteToSD () {
	SD.remove(this->FileName);
	File myFile = SD.open(this->FileName, FILE_WRITE);
	if (myFile) {
		for (int level = 1; level <= 12; level++) {
			for (int side = 0; side <= 1; side++) {
				myFile.print (this->offset[level-1][side]);
				if (level == 12 && side == 1) {
					myFile.print ('S');
				} else {
					myFile.print ('_');
				}
			}
		}
		close(myFile);
	}
}
