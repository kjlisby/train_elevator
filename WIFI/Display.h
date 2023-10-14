#ifndef DISPLAY_H
#define DISPLAY_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

class Display {
	public:
		void Init     ();
		void NewLevel (int Level);
		void AtLevel  (int Level);
		void Homing   ();

	private:
		Adafruit_SSD1306 *MyDisplay;
		int CurrentLevel = 1;
		int NextLevel    = 1;
		void MovingUp ();
		void MovingDown ();
};
#endif
