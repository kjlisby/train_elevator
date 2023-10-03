// The Display class is writing messages on the OLED display

#include "Display.h"

void Display::Init () {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  this->display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  this->display.display();
}

void Display::MovingUp () {
	// Show a upwards arrow with this->NextLevel on top and this->CurrentLevel at the bottom
	display.clearDisplay();
	display.setTextSize(3); // Draw 3X-scale text
	display.setTextColor(WHITE);
	display.setCursor(40, 0);
	display.println(F(this->NextLevel));
	display.setCursor(40, 40);
	display.println(F(this->CurrentLevel));
	display.fillTriangle(64, 20, 24, 30, 104, 30, WHITE);
	display.fillRect(54, 30, 20, 10, WHITE);
	display.display();
}

void Display::MovingDown () {
	// Show a downwards arrow with this->CurrentLevel on top and this->NextLevel at the bottom
	display.clearDisplay();
	display.setTextSize(3); // Draw 3X-scale text
	display.setTextColor(WHITE);
	display.setCursor(40, 0);
	display.println(F(this->CurrentLevel));
	display.setCursor(40, 40);
	display.println(F(this->NextLevel));
	display.fillRect(54, 20, 20, 10, WHITE);
	display.fillTriangle(64, 40, 24, 30, 104, 30, WHITE);
	display.display();
}

void Display::NewLevel (int Level) {
	this->NextLevel = Level;
	if (Level > this->CurrentLevel) {
		this->MovingUp();
	} else if (Level < this->CurrentLevel) {
		this->MovingDown();
	}
}

void Display::AtLevel (int Level) {
	// Show the number
	display.clearDisplay();
	display.setTextSize(6); // Draw 6X-scale text
	display.setTextColor(WHITE);
	display.setCursor(10, 10);
	display.println(F(Level));
	display.display();
	this->CurrentLevel = Level;
}

void Display::Homing () {
	display.clearDisplay();
	display.setTextSize(4); // Draw 4X-scale text
	display.setTextColor(WHITE);
	display.setCursor(10, 20);
	display.println(F("HOMING.."));
	display.display();
}
