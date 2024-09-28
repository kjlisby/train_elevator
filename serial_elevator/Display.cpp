// The Display class is writing messages on the OLED display and keep the WEB server updated as well

#include "Display.h"

void Display::Init () {
  this->MyDisplay = new Adafruit_SSD1306 (SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  this->MyDisplay->begin(SSD1306_SWITCHCAPVCC, 0x3C);

  this->MyDisplay->clearDisplay();
  this->MyDisplay->setTextSize(2);
  this->MyDisplay->setTextColor(WHITE);
  this->MyDisplay->setCursor(30, 30);
  this->MyDisplay->print(F("Hello"));

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  this->MyDisplay->display();
}

void Display::MovingUp () {
  // Show a upwards arrow with this->NextLevel on top and this->CurrentLevel at the bottom
  this->MyDisplay->clearDisplay();
  this->MyDisplay->setTextSize(3); // Draw 3X-scale text
  this->MyDisplay->setTextColor(WHITE);
  if (this->NextLevel < 10) {
    this->MyDisplay->setCursor(55, 0);
  } else {
    this->MyDisplay->setCursor(45, 0);
  }
  this->MyDisplay->println(this->NextLevel);
  if (this->CurrentLevel < 10) {
    this->MyDisplay->setCursor(55, 45);
  } else {
    this->MyDisplay->setCursor(45, 45);
  }
  this->MyDisplay->print(this->CurrentLevel);
  this->MyDisplay->fillTriangle(64, 22, 44, 30, 84, 30, WHITE);
  this->MyDisplay->fillRect(54, 30, 20, 10, WHITE);
  this->MyDisplay->display();
}

void Display::MovingDown () {
  // Show a downwards arrow with this->CurrentLevel on top and this->NextLevel at the bottom
  this->MyDisplay->clearDisplay();
  this->MyDisplay->setTextSize(3); // Draw 3X-scale text
  this->MyDisplay->setTextColor(WHITE);
  if (this->CurrentLevel < 10) {
    this->MyDisplay->setCursor(55, 0);
  } else {
    this->MyDisplay->setCursor(45, 0);
  }
  this->MyDisplay->println(this->CurrentLevel);
  if (this->NextLevel < 10) {
    this->MyDisplay->setCursor(55, 45);
  } else {
    this->MyDisplay->setCursor(45, 45);
  }
  this->MyDisplay->print(this->NextLevel);
  this->MyDisplay->fillRect(54, 22, 20, 10, WHITE);
  this->MyDisplay->fillTriangle(64, 40, 44, 30, 84, 30, WHITE);
  this->MyDisplay->display();
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
  this->MyDisplay->clearDisplay();
  this->MyDisplay->setTextSize(6); // Draw 5X-scale text
  this->MyDisplay->setTextColor(WHITE);
  if (Level < 10) {
    this->MyDisplay->setCursor(45, 10);
  } else {
    this->MyDisplay->setCursor(25, 10);
  }
  this->MyDisplay->print(Level);
  this->MyDisplay->display();
  this->CurrentLevel = Level;
}

void Display::Homing () {
  this->MyDisplay->clearDisplay();
  this->MyDisplay->setTextSize(2); // Draw 3X-scale text
  this->MyDisplay->setTextColor(WHITE);
  this->MyDisplay->setCursor(30, 30);
  this->MyDisplay->print(F("HOMING"));
  this->MyDisplay->display();
}
