  #ifndef CALIBRATOR_H
  #define CALIBRATOR_H

  #include <stdint.h>
  #include <Preferences.h>

  // Keep track of the exact number of steps compared to the home position
  // each stepper motor must move to obtain the correct height of the elevator.
  // Persist the data on SD card.
  class Calibrator {
    public:
      void Init();
      void SetOffset(int Level, long NewLeftValue, long NewRightValue);
      long GetOffset(bool Left, int Level);
      void Restore();
      void Save();
    private:
      const char* MyPrefrencesNameSpace = "TEC"; //TrainElevatorCalibration
      long steps_per_level = 10000;
      long offset [12][2] = { {-2500, -2500},
                              {-2500, -2500},
                              {6500, 6500}, 
                              {6500, 6500},
                              {15500, 15500},
                              {15500, 15500},
                              {24500, 24500},
                              {24500, 24500},
                              {33500, 33500},
                              {33500, 33500},
                              {42500, 42500},
                              {42500, 42500} };
  };

  #endif
