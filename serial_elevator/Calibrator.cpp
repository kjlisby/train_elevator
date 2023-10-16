#include "Calibrator.h"

Preferences preferences;
void Calibrator::Init () {
  this->Restore();
}

void Calibrator::SetOffset (int Level, long NewLeftValue, long NewRightValue) {
	this->offset[Level-1][1] = NewLeftValue;
	this->offset[Level-1][0] = NewRightValue;
}

long Calibrator::GetOffset (bool Left, int Level) {
//	Serial.print("DEBUG Calibrator::GetOffset("); Serial.print(Left); Serial.print(", "); Serial.print(Level); Serial.println(")");
	if (Left) {
		return this->offset[Level-1][1];
	} else {
		return this->offset[Level-1][0];
	}
}

void Calibrator::Restore () {
	long value;
	char key[200];
	preferences.begin(this->MyPrefrencesNameSpace, true);
	for (int level = 1; level <= 12; level++) {
		for (int side = 0; side <= 1; side++) {
			sprintf(key, "CD%d_%d", level, side);
			value = preferences.getLong(key, -7777);
			if (value != -7777) {
				this->offset[level-1][side] = value;
			}
		}
	}
	preferences.end();
	Serial.println("DEBUG Calibrator::Restore done");
}

void Calibrator::Save () {
	char key[200];
	preferences.begin(this->MyPrefrencesNameSpace, false);
	preferences.clear();
	for (int level = 1; level <= 12; level++) {
		for (int side = 0; side <= 1; side++) {
			sprintf(key, "CD%d_%d", level, side);
			preferences.putLong(key, this->offset[level-1][side]);
		}
	}
	preferences.end();
	Serial.println("DEBUG Calibrator::Save done");
}
