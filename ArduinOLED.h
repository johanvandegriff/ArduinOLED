/*
This is a library for the ArduinOLED board to read the buttons and joystick.

http://johan.vandegriff.net/ArduinOLED/

Written by Johan Vandegriff
BSD license, check license.txt for more information
*/

#include "Arduino.h"

#define BUZZER_PIN 3

#define NUM_BUTTONS 14

#define BTN_A         0b0000000000000001
#define BTN_H         0b0000000000000010
#define BTN_X         0b0000000000000100
#define BTN_B         0b0000000000001000
#define BTN_L         0b0000000000010000
#define BTN_Y         0b0000000000100000
#define BTN_U         0b0000000001000000
#define BTN_R         0b0000000010000000
#define BTN_D         0b0000000100000000
#define BTN_Z         0b0000001000000000

#define BTN_JOY_L     0b0000010000000000
#define BTN_JOY_R     0b0000100000000000
#define BTN_JOY_U     0b0001000000000000
#define BTN_JOY_D     0b0010000000000000

#define BTN_BOTH_L    0b0000010000010000
#define BTN_BOTH_R    0b0000100010000000
#define BTN_BOTH_U    0b0001000001000000
#define BTN_BOTH_D    0b0010000100000000

#define BTN_JOY_DPAD  0b0011110000000000
#define BTN_DPAD      0b0000000111010000
#define BTN_BOTH_DPAD 0b0011110111010000

#define BTN_ABXY      0b0000000000101101
#define BTN_ABXYZ     0b0000001000101101
#define BTN_ANY       0b0011111111111111


class ArduinOLEDClass {
  public:
    static volatile uint16_t buttons; //the current states of the buttons
    static volatile uint16_t buttonsJustPressed;
    static volatile uint16_t buttonsJustReleased;
    static volatile int16_t joyX;
    static volatile int16_t joyY;

    static void begin();
    static void zeroJoystick();
    static void update();
    static boolean isPressed(uint16_t mask);
    static boolean justPressed(uint16_t mask);
    static boolean justReleased(uint16_t mask);
    static void edgeDetectionReset();
    void ArduinOLEDClass::pause(uint16_t mask);
  private:
    static uint16_t lastButtonReadings; //the previous readings of the buttons
    static uint16_t lastButtonStates; //the previous states of the buttons
    static uint32_t lastDebounceTimes[NUM_BUTTONS]; //the last time the output pin toggled
    static int16_t joyXcenter;
    static int16_t joyYcenter;

    static const uint32_t debounceDelay; //the debounce time
    static const uint8_t joyThreshold;
    static const uint8_t joyDeadzone;
};

static ArduinOLEDClass ArduinOLED;
