/*
 * This sketch is for the ArduinOLED board. for more info, visit:
 * 
 * https://johanv.xyz/ArduinOLED/
 */

#include <Arduino.h>
#include <U8g2lib.h> //library to control the display in graphics mode
#include <pitches.h> //list of pitches for the buzzer
#include <EEPROM.h>
#include <ArduinOLED.h> //library to read the buttons and joysticks

#define STACKER_EEPROM_ADDR 0
#define STACKER_EEPROM_LEN 1

//set up the display in page mode, which will draw part of the screen at a time
//https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
//there is a 128 byte (6.25% of 2048 bytes) difference between the 128 and 256 byte page sizes
//U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //128 byte page size
//U8G2_SSD1306_128X64_NONAME_2_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //256 byte page size
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //1024 byte page size (full page)

/*
 * This updates the display multiple times to display the whole image
 */
void render(void (*f)(void)) {
  display.firstPage();
  do {
    f();
  } while( display.nextPage() );
}

uint8_t screen[8][16];

void stackerScreen(char* s, boolean isGameOver) {
  
  for(int i=0; i<16; i++) {
    for(int j=0; j<8; j++) {
      if(screen[j][i]) display.drawBox(i*8,j*8,7,7);
    }
  }

  display.drawStr(128, 10, s);
  if (isGameOver) {
    display.setDrawColor(0);
    display.drawBox(2, 4, 38, 56);
    display.setDrawColor(1);
    display.drawStr(12*3, 6, "Game Over");
    display.drawStr(12*2, 6, "Press Any");
    display.drawStr(12*1, 10, "Button");
  }
}

void stackerHighscoreScreen(uint8_t score, uint8_t highscore) {
  char str1[11], str2[11], str4[7];
  
  if (score > highscore) {
    sprintf(str1, "%02i", score);
    sprintf(str2, "beats the", score);
//    sprintf(str3, "highscore");
    sprintf(str4, "of %02i!", highscore);
  } else if (score < highscore) {
    sprintf(str1, "%02i doesn't", score);
    sprintf(str2, "beat the");
//    sprintf(str3, "highscore");
    sprintf(str4, "of %02i.", highscore);
  } else {
    sprintf(str1, "%02i", score);
    sprintf(str2, "equals the");
//    sprintf(str3, "highscore");
    sprintf(str4, "of %02i.", highscore);
  }

  display.drawStr(12*8, 0, str1);
  display.drawStr(12*7, 0, str2);
  display.drawStr(12*6, 0, "highscore");
  display.drawStr(12*5, 0, str4);
}

void stacker() {
  display.setFontDirection(1);

  for(uint8_t i=0; i<16; i++) {
    for(uint8_t j=0; j<8; j++) {
      screen[j][i] = 0;
    }
  }

  uint8_t delayAmount = 200;
  uint32_t timer = 0;
  uint8_t xPos = 3, yPos = 0, len = 4;
  boolean isInc = true;

  uint8_t numToDelete = 0;
  uint8_t deleteX[4];

  uint8_t numToAdd = 0;
  uint8_t addX[4];

  char str[8] = "00 / 15";

  while (true) {
    if (ArduinOLED.isPressed(BTN_ANY)) {
      tone(BUZZER_PIN, NOTE_G5, 100);
      yPos++;
      numToDelete = 0;
      numToAdd = 0;
      for(int i=xPos; i<xPos+len; i++) {
        if (yPos == 1 || screen[i][yPos-2]) {
          addX[numToAdd] = i;
          numToAdd++;
        } else {
          deleteX[numToDelete] = i;
          numToDelete++;
        }
      }
      len = numToAdd;
      sprintf(str, "%02i / 15", yPos - (len == 0));
      if (numToDelete > 0) {
        for(int z=0; z<4; z++) {
          for(int i=0; i<numToDelete; i++) screen[deleteX[i]][yPos-1] = 0;
          //render the screen
          display.firstPage(); do { stackerScreen(str, false); } while(display.nextPage());
          for(int i=0; i<numToDelete; i++) screen[deleteX[i]][yPos-1] = 1;
          //render the screen
          display.firstPage(); do { stackerScreen(str, false); } while(display.nextPage());
        }
        for(int i=0; i<numToDelete; i++) screen[deleteX[i]][yPos-1] = 0;
      }
    
      //if (yPos > 1) for(int i=0; i<numToAdd; i++) screen[addX[i]][yPos] = 1;
      xPos = addX[0];
      delayAmount *= 0.85;
      //delayAmount = delayAmount >> 1; //divide by 2
      display.firstPage(); do { stackerScreen(str, false); } while(display.nextPage());
      while(ArduinOLED.isPressed(BTN_ANY));
      if (yPos == 15 || len == 0) {
        yPos -= (len == 0);
        uint8_t highscore = EEPROM.read(STACKER_EEPROM_ADDR);
        if (yPos > highscore) EEPROM.write(STACKER_EEPROM_ADDR, yPos);
        display.firstPage(); do { stackerScreen(str, true); } while(display.nextPage());
        while(!ArduinOLED.isPressed(BTN_ANY));
        while(ArduinOLED.isPressed(BTN_ANY));
        display.firstPage(); do { stackerHighscoreScreen(yPos, highscore); } while(display.nextPage());
        while(!ArduinOLED.isPressed(BTN_ANY));
        while(ArduinOLED.isPressed(BTN_ANY));
        display.setFontDirection(0);
        return;
      }
      //timer = millis() + delayAmount;
    }
    sprintf(str, "%02i / 15", yPos);

    if (millis() > timer) {
      timer = millis() + delayAmount;
      tone(BUZZER_PIN, NOTE_G3, 50);
      if(isInc) {
        xPos++;
      } else {
        xPos--;
      }
      if (xPos <= 0) {
        isInc = true;
      }
      if (xPos+len >= 8) {
        isInc = false;
      }
    }
    for(int i=0; i<8; i++) {
      screen[i][yPos] = (i >= xPos && i < xPos+len);
    }
    //render the screen
    display.firstPage(); do { stackerScreen(str, false); } while(display.nextPage());
  }
}

//to display anything on the display, put it inside a function and call render(functionName)
void setupScreen() {
  //display the info text
  display.setCursor(0, 0*10);
  display.print(F("ArduinOLED v1.0 "));
  display.setCursor(0, 1*10+5);
  display.print(F("Johan.Vandegriff"));
  display.setCursor(0, 2*10+5);
  display.print(F(".net/ArduinOLED "));
  display.setCursor(0, 4*10);
  display.print(F("  Stacker Game  "));
  display.setCursor(0, 5*10);
  display.print(F("Press any button"));
}

void resetScreen() {
  display.setCursor(0, 2*10+5);
  display.print(F("Highscore Reset"));
  display.setCursor(0, 5*10);
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

  //hold down the R button to reset the stacker highscore
  if(ArduinOLED.isPressed(BTN_R)) {
    EEPROM.write(STACKER_EEPROM_ADDR, 0);
    render(resetScreen); //display the "highscore reset" message
    ArduinOLED.pause(BTN_ANY); //wait for any button to be pressed
  }
  
  render(setupScreen); //display the menu

  ArduinOLED.pause(BTN_ANY); //wait for any button to be pressed
//  display.setFont(u8g2_font_unifont_t_symbols); //set the font to allow unicode later
}

void loop() {
  stacker();
}
