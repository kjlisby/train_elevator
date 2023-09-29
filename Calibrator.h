#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <stdint.h>

class Calibrator {
        public:
                void SetOffset(bool Left, bool Upwards, int Level, long NewValue);
                void GetOffset(bool Left, bool Upwards, int Level, long NewValue);
        private:
                String  FileName = "CalibrationData.txt";
                void    ReadFromSD();
                void    WriteToSD();
                long    steps_per_level = 10000;
                long    offset[6][2][2] = { { {0,0}, {0,0} },
                                            { {0,0}, {0,0} },
                                            { {0,0}, {0,0} },
                                            { {0,0}, {0,0} },
                                            { {0,0}, {0,0} },
                                            { {0,0}, {0,0} } };
};

#endif
