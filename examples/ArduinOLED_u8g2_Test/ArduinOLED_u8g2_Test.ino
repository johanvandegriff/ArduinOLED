/*
   This sketch is for the ArduinOLED board. for more info, visit:

   https://johanv.xyz/ArduinOLED/
*/

#include <Arduino.h>
#include <U8g2lib.h> //library to control the display in graphics mode
#include <pitches.h> //list of pitches for the buzzer
#include <ArduinOLED.h> //library to read the buttons and joysticks

//set up the display in page mode, which will draw part of the screen at a time
//https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
//there is a 128 byte (6.25% of 2048 bytes) difference between the 128 and 256 byte page sizes
U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //128 byte page size
//U8G2_SSD1306_128X64_NONAME_2_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //256 byte page size
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //1024 byte page size (full page)

/*
   This updates the display multiple times to display the whole image
*/
void render(void (*f)(void)) {
  display.firstPage();
  do {
    f();
  } while ( display.nextPage() );
}

//to display anything on the screen, put it inside a function and call render(functionName)
void setupScreen() {
  //display the info text
  display.setCursor(0, 0 * 10);
  display.print(F("ArduinOLED v1.0 "));
  display.setCursor(0, 1 * 10 + 5);
  display.print(F("  https://johanv"));
  display.setCursor(0, 2 * 10 + 5);
  display.print(F(".xyz/ArduinOLED "));
  display.setCursor(0, 4 * 10);
  display.print(F("  Test Program  "));
  display.setCursor(0, 5 * 10);
  display.print(F("Press any button"));
}

void setup() {
  ArduinOLED.begin(); //start the ArduinOLED library, which controls the buttons and joystick
  display.begin(); //start the u8g2 library, which controls the display

  display.setFont(u8g2_font_6x10_tf); //set the font
  display.setFontRefHeightExtendedText();
  display.setDrawColor(1);
  display.setFontPosTop();
  display.setFontDirection(0);
  render(setupScreen); //display the menu
  while (!ArduinOLED.isPressed(BTN_ANY)); //wait for a button to be pressed
  display.setFont(u8g2_font_unifont_t_symbols); //set the font to allow unicode later
}

void loopScreen() {
  //draw the border
  display.drawFrame(0, 0, display.getDisplayWidth(), display.getDisplayHeight() );

  //if a button is pressed, display a certain graphics element for that button
  if (ArduinOLED.isPressed(BTN_A)) {
    display.drawFrame(2, 2, display.getDisplayWidth() - 4, display.getDisplayHeight() - 4);
  }
  if (ArduinOLED.isPressed(BTN_H)) {
    display.drawBox(5, 10, 20, 10);
  }
  if (ArduinOLED.isPressed(BTN_X)) {
    display.drawFrame(5, 10 + 30, 20, 10);
  }
  if (ArduinOLED.isPressed(BTN_B)) {
    display.drawDisc(10, 18, 9);
  }
  if (ArduinOLED.isPressed(BTN_L)) {
    display.drawCircle(10, 18 + 30, 9);
  }
  if (ArduinOLED.isPressed(BTN_Y)) {
    display.drawRFrame(5, 10, 40, 30, 5);
  }
  if (ArduinOLED.isPressed(BTN_U)) {
    display.drawRBox(50, 10, 25, 40, 5);
  }
  if (ArduinOLED.isPressed(BTN_R)) {
    display.drawLine(20, 10, 100, 55);
  }
  if (ArduinOLED.isPressed(BTN_D)) {
    display.drawTriangle(10, 40, 45, 30, 86, 53);
  }
  if (ArduinOLED.isPressed(BTN_Z)) {
    display.drawUTF8(0, 24, "☀ ☁");
  }

  if (ArduinOLED.isPressed(BTN_JOY_L)) {
    display.drawUTF8(3, 36, "☂");
  }
  if (ArduinOLED.isPressed(BTN_JOY_R)) {
    display.drawUTF8(3, 36, "☔");
  }
  if (ArduinOLED.isPressed(BTN_JOY_U)) {
    tone(BUZZER_PIN, NOTE_G5, 100);
  }
  if (ArduinOLED.isPressed(BTN_JOY_D)) {
    tone(BUZZER_PIN, NOTE_G3, 100);
  }
}

void loop() {
  render(loopScreen); //render the screen
  delay(1); //wait 1 millisecond
}
