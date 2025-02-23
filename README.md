# Arduino controlled train elevator
At the time of writing this, I am building an elevator for my scale H0 model train layout. 

The reason is my very limited space, where I will build up my next layout on shelves in a U-shape with no room for a hidden yard to park trains.

Hence, I will use wall space for that purpose.

The physical elevator looks as follows on a CAD drawing. Only one car is shown. But there are two mechanically tied together, so that when one of them goes up, the other goes down and so that they outbalance each other: ![](mekanik%20synlig.jpg)

Primo 2025, the elevator is functioning: 

https://github.com/user-attachments/assets/1858c7c4-1a3d-495b-b5ac-6b174f491ff5

## Hardware

The hardware consist of:
  - ESP32 DoIT module (serving the elevator control and a WEB page turning a phone or a PC into the UI)
  - SD card reader to store the HTML files, icons etc. (only used for the WEB UI and therefore abandoned as I switched to the serial version)
  - A tiny 0,96" (later replaced with a 2,43") OLED display to show the status of the elevator
  - Stepper motors to drive the elevator
  - An optocoupler and a few diodes and resistors to act as a DCC interface
  - 20 volt power supply to drive the stepper motors
  - 5 volt power supply for the control system (not needed after switch to serial version, where the 5V comes from the USB connection from my PC)
  - Two micro switches to sense the end-stop position of each side of the elevator
  - Optical sensors (IR LEDs and IR photodiodes) for ensuring there is no train "sticking out" when moving the elevator
  - Relay modules (12 relays needed, even though only 8 are shown below) to provide power to only the tracks of the current level
  - MCP23017 I/O port expansion module to provide output ports for the relays
  - IR receiver for receiving inputs from my (Samsung) TV remote control

And it is put together as follows. Note that I will not need the "enable" wire to the stepper motors. Removing power from the steppers would only mean that the ESP32 is no longer knowing the exact location of the elevator.

Version 1:
![](Diagram.jpg)

Version 2:
![](Diagram_v2.jpg)

I was at first almost done with the WIFI version, which uses a Web browser as UI. But because of weak WIFI connection, I abandoned that idea and instead began the serial version, which simply connects via the USB port of the ESP32 to a PC and communicates directly with either JMRI (Java Model Railroad Interface) or the Python based UI. Or for that matter, the serial monitor in the Arduino IDE can be used as a command-line. Note that the SD card is not used by the serial version. So I have removed that.

Arduino Libraries used (for serial elevator version):
- AccelStepper by Mike McCauley
- Adafruit SSD1306 by Adafruit (with dependencies)
- NmraDcc by Axel Sheperd
- IRremote by shirriff

See more at http://lisby.dk/wordpress/?p=2779 (so far only in Danish).
