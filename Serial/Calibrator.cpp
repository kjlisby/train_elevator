#include "Calibrator.h"
#include <SD.h>

void Calibrator::SetOffset (int Level, long NewLeftValue, long NewRightValue) {
	this->offset[Level-1][1] = NewLeftValue;
	this->offset[Level-1][0] = NewRightValue;
}

long Calibrator::GetOffset (bool Left, int Level) {
	Serial.print("Calibrator::GetOffset("); Serial.print(Left); Serial.print(", "); Serial.print(Level); Serial.println(")");
	if (Left) {
		return this->offset[Level-1][1];
	} else {
		return this->offset[Level-1][0];
	}
}

void Calibrator::ReadFromSD () {
	char buffer[200];
	char latest_char;
	int i = 0;
	int level = 1;
	int side = 0;
	File myFile = SD.open(this->FileName, FILE_READ);
	if (myFile) {
		do {
			latest_char = buffer[i++] = myFile.read();
			if (latest_char == '_' || latest_char == 'S') {
				buffer[i-1] = '\0';
				this->offset[level-1][side] = atol(buffer);
				i = 0;
				if (side == 1) {
					side = 0;
					level++;
				} else {
					side = 1;
				}
			}
		} while (latest_char != 'S');
		close (myFile);
	}
	Serial.println("Calibrator::ReadFromSD done");
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
