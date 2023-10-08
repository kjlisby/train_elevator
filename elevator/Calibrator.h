#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <stdint.h>

// Keep track of the exact number of steps compared to the home position
// each stepper motor must move to obtain the correct height of the elevator.
// Persist the data on SD card.
class Calibrator {
	public:
		void SetOffset(int Level, long NewLeftValue, long NewRightValue);
		long GetOffset(bool Left, int Level);
		void ReadFromSD();
		void WriteToSD();
	private:
		const char *FileName = "/CalibrationData.txt";
		long steps_per_level = 10000;
		long offset [12][2] = { {0, 0},
		                        {0, 0},
		                        {10000, 10000}, 
		                        {10000, 10000},
		                        {20000, 20000},
		                        {20000, 20000},
		                        {30000, 30000},
		                        {30000, 30000},
		                        {40000, 40000},
		                        {40000, 40000},
		                        {50000, 50000},
		                        {50000, 50000} };
};

#endif
