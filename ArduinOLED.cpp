/*
This is a library for the ArduinOLED board to read the buttons and joystick.

http://johan.vandegriff.net/ArduinOLED/

Written by Johan Vandegriff
BSD license, check license.txt for more information
*/

#include "Arduino.h"
#include "ArduinOLED.h"
#include "FastAnalogRead.h"

//Faster digital IO:
//https://github.com/mmarchetti/DirectIO
#include <DirectIO.h>

uint16_t volatile ArduinOLEDClass::buttons = 0;
uint16_t volatile ArduinOLEDClass::buttonsJustPressed = 0;
uint16_t volatile ArduinOLEDClass::buttonsJustReleased = 0;
int16_t volatile ArduinOLEDClass::joyX = 0;
int16_t volatile ArduinOLEDClass::joyY = 0;
uint16_t ArduinOLEDClass::lastButtonReadings = 0;
uint16_t ArduinOLEDClass::lastButtonStates = 0;
uint32_t ArduinOLEDClass::lastDebounceTimes[NUM_BUTTONS];
int16_t ArduinOLEDClass::joyXcenter = 512;
int16_t ArduinOLEDClass::joyYcenter = 512;

const uint32_t ArduinOLEDClass::debounceDelay = 50;
const uint8_t ArduinOLEDClass::joyThreshold = 150;
const uint8_t ArduinOLEDClass::joyDeadzone = 4;

Output<10> col0(HIGH);
Output<11> col1(HIGH);
Output<12> col2(HIGH);

Input<4> row0(true); //true means enable the pullup resistor
Input<7> row1(true);
Input<2> row2(true);

Input<8> joyButton(true);

void ArduinOLEDClass::begin() {
  EnableFastAnalogRead();
  zeroJoystick();

  for (uint8_t i=0; i<NUM_BUTTONS; i++){
    lastDebounceTimes[i] = 0;
  }

  //set up a timer interrupt every millisecond
  OCR0A = 0x80; //millis() counter uses 0
  TIMSK0 |= _BV(OCIE0A);
}

void ArduinOLEDClass::zeroJoystick() {
  joyXcenter = analogRead(A6);
  joyYcenter = analogRead(A7);
}

//timer interrupt function (every millisecond)
SIGNAL(TIMER0_COMPA_vect) {
  ArduinOLEDClass::update();
}

void ArduinOLEDClass::update() {
  joyX = joyXcenter-analogRead(A6);
  joyY = joyYcenter-analogRead(A7);

  if (joyX < joyDeadzone && joyX > -joyDeadzone) joyX = 0;
  if (joyY < joyDeadzone && joyY > -joyDeadzone) joyY = 0;

  uint16_t reading = 0;

//  uint16_t mask = 1;
//  for (uint8_t y=0; y<3; y++){
//    digitalWrite(buttonColPins[y], LOW);
//    for (uint8_t x=0; x<3; x++) {
//      //change the bit at the button's position to the reading
//      if (!digitalRead(buttonRowPins[x])) reading |= mask;
//      mask = mask << 1;
//    }
//    digitalWrite(buttonColPins[y], HIGH);
//  }

  col0 = LOW;
  delayMicroseconds(4);
  if (!row0) reading |= BTN_A;
  if (!row1) reading |= BTN_H;
  if (!row2) reading |= BTN_X;
  col0 = HIGH;
  col1 = LOW;
  delayMicroseconds(4);
  if (!row0) reading |= BTN_B;
  if (!row1) reading |= BTN_L;
  if (!row2) reading |= BTN_Y;
  col1 = HIGH;
  col2 = LOW;
  delayMicroseconds(4);
  if (!row0) reading |= BTN_U;
  if (!row1) reading |= BTN_R;
  if (!row2) reading |= BTN_D;
  col2 = HIGH;

  //joystick button
  if (!joyButton) reading |= BTN_Z;

  //joystick dpad
  if (joyX < -joyThreshold) reading |= BTN_JOY_L;
  if (joyX >  joyThreshold) reading |= BTN_JOY_R;
  if (joyY >  joyThreshold) reading |= BTN_JOY_U;
  if (joyY < -joyThreshold) reading |= BTN_JOY_D;

  //debounce
  uint16_t buttonsTmp = 0;
  uint16_t mask = 1;
  for (uint8_t i=0; i<NUM_BUTTONS; i++) {
    if (
        ((lastButtonReadings ^ reading) & mask)
        || (lastDebounceTimes[i] == 0)
       ) {
      // reset the debouncing timer
      lastDebounceTimes[i] = millis();
    } else if ((millis() - lastDebounceTimes[i]) >= debounceDelay) {
      // update the button state          
      if (reading & mask) buttonsTmp |= mask;
    }
    mask = mask << 1;
  }
  buttons = buttonsTmp;
  lastButtonReadings = reading;

  buttonsJustPressed |= ~lastButtonStates & buttons;
  buttonsJustReleased |= lastButtonStates & ~buttons;
  lastButtonStates = buttons;
}

boolean ArduinOLEDClass::isPressed(uint16_t mask){
  return buttons & mask;
}

boolean ArduinOLEDClass::justPressed(uint16_t mask){
  return buttonsJustPressed & mask;
}

boolean ArduinOLEDClass::justReleased(uint16_t mask){
  return buttonsJustReleased & mask;
}

void ArduinOLEDClass::edgeDetectionReset() {
  buttonsJustPressed = 0;
  buttonsJustReleased = 0;
}

/*
 * wait for a button or buttons to be pressed
 */
void ArduinOLEDClass::pause(uint16_t mask) {
  while(isPressed(mask));
  while(!isPressed(mask));
  while(isPressed(mask));
}

