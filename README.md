# Arduino controlled train elevator
At the time of writing this, I am planning to build an elevator for my scale H0 model train layout. 

The reason is my very limited space, where I will build up my next layout on shelves in a U-shape with no room for a hidden yard to park trains.

Hence, I will use wall space for that purpose.

So far, the elevator is only a CAD drawing.

## Hardware

The hardware consist of:
  - ESP32 DoIT module (serving the elevator control and a WEB page turning a phone into the UI)
  - SD card reader to store the HTML files, icons etc.
  - A tiny OLED display to show the status of the elevator
  - Stepper motors to drive the elevator
  - 20 volt power supply to drive the stepper motors
  - 5 volt power supply for the control system
  - Micro switches to sense the position of the elevator
  - Optical sensor for adding security
  - Possibly an MCP23017 based Arduino module to add more digital I/O ports

See more at http://lisby.dk/wordpress/?p=2779 (so far only in Danish).
