/* Define properties in this file */

#include "Arduino.h"

/* --------- CONSTANTS ---------- */
extern const int INITIAL_STATE;
extern const int EMG_ARRAY_LENGTH;
extern const int TIME_OF_FLEX;
extern const int FSR_THRESHOLD;
extern const int LOOP_DELAY;
extern const int LED_BLUE_THRESHOLD;
extern const int LED_RED_THRESHOLD;

/* --------- PINS ---------- */
// Analog Pins
extern const int fsr;
extern const int BatteryLevelRead;
extern const int thresholdPot;
extern const int emg;

//Digital Pins
extern const int BatteryLevelLEDR;
extern const int BatteryLevelLEDG;
extern const int BatteryLevelLEDB;
extern const int led; //---------------------> Testing LED
extern const int servoLeft;
extern const int servoRight;
