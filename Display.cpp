// The Display class is writing messages on the OLED display and keep the WEB server updated as well

#include "Display.h"

void Display::Init (SDWebServer *WS) {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  this->display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  this->display.display();
  this->WS = WS;
}

void Display::MovingUp () {
	// Show a upwards arrow with this->NextLevel on top and this->CurrentLevel at the bottom
	this->display.clearDisplay();
	this->display.setTextSize(3); // Draw 3X-scale text
	this->display.setTextColor(WHITE);
	this->display.setCursor(40, 0);
	this->display.println(F(this->NextLevel));
	this->display.setCursor(40, 40);
	this->display.println(F(this->CurrentLevel));
	this->display.fillTriangle(64, 20, 24, 30, 104, 30, WHITE);
	this->display.fillRect(54, 30, 20, 10, WHITE);
	this->display.display();
}

void Display::MovingDown () {
	// Show a downwards arrow with this->CurrentLevel on top and this->NextLevel at the bottom
	this->display.clearDisplay();
	this->display.setTextSize(3); // Draw 3X-scale text
	this->display.setTextColor(WHITE);
	this->display.setCursor(40, 0);
	this->display.println(F(this->CurrentLevel));
	this->display.setCursor(40, 40);
	this->display.println(F(this->NextLevel));
	this->display.fillRect(54, 20, 20, 10, WHITE);
	this->display.fillTriangle(64, 40, 24, 30, 104, 30, WHITE);
	this->display.display();
}

void Display::NewLevel (int Level) {
	this->NextLevel = Level;
	if (Level > this->CurrentLevel) {
		this->MovingUp();
	} else if (Level < this->CurrentLevel) {
		this->MovingDown();
	}
	this->WS->sendMessage("STATUS: Moving from "+to_string(this->CurrentLevel)+" to "+to_string(this->NextLevel));
}

void Display::AtLevel (int Level) {
	// Show the number
	this->display.clearDisplay();
	this->display.setTextSize(6); // Draw 6X-scale text
	this->display.setTextColor(WHITE);
	this->display.setCursor(10, 10);
	this->display.println(F(Level));
	this->display.display();
	this->WS->sendMessage("STATUS: AtLevel "+to_string(Level));
	this->CurrentLevel = Level;
}

void Display::Homing () {
	this->display.clearDisplay();
	this->display.setTextSize(4); // Draw 4X-scale text
	this->display.setTextColor(WHITE);
	this->display.setCursor(10, 20);
	this->display.println(F("HOMING.."));
	this->display.display();
	this->WS->sendMessage("STATUS: HOMING");
}
