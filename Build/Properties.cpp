/* Define properties in this file */

#include "Arduino.h"
#include "Properties.h"

/* --------- CONSTANTS ---------- */
const int INITIAL_STATE = 0;
const int EMG_ARRAY_LENGTH = 25;
const int TIME_OF_FLEX = 700;
const int FSR_THRESHOLD = 600;
const int LOOP_DELAY = 25;
const int LED_GREEN_THRESHOLD = 818; // 4V*1023/5V
const int LED_RED_THRESHOLD = 655; // 3.2V*1023/5V
const int MAX_FLEX_THRESHOLD = 2000;

/* --------- PINS ---------- */
// Analog Pins
const int fsr = A2;
const int BatteryLevelRead = A1;
const int thresholdPot = A4;
const int emg = A0;

//Digital Pins
const int BatteryLevelLEDR = 2;
const int BatteryLevelLEDG = 4;
const int BatteryLevelLEDB = 3;
const int led = 7; //---------------------> Testing LED
const int servoLeft = 10;
const int servoRight = 11;
