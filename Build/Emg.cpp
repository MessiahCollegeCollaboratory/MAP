#include <stdint.h>
#include "Emg.h"
#include "Arduino.h"
#include "Properties.h"

Emg::Emg(){
  this->emgValue = 0;
  this->emgAvg = 0;
}

int32_t Emg::emgRead(){
  int32_t emgValue = analogRead(emg);

  this->emgValue = emgValue;
  return this->emgValue;
}

void Emg::emgCal(){
  int32_t emgAvg = 0;
  
  for(int i = 0; i < 25; i++){
    emgAvg += emgRead();
    delay(25);
  }

  // Finds average of 25 initial readings to calibrate
  this->emgAvg = emgAvg / 25;
}

int32_t Emg::getEmgValue(){
  return (this->emgValue - this->emgAvg);
}

int32_t Emg::getEmgAvg(){
  return this->emgAvg;
}

void Emg::calibrationSequence(){
  int32_t maxStrength[5] = {0,0,0,0,0};
  int32_t maxAvg = 0;
  int32_t normalAvg = 0;

  //Blink light blue three times
  digitalWrite(BatteryLevelLEDR, LOW);
  digitalWrite(BatteryLevelLEDG, LOW);
  digitalWrite(BatteryLevelLEDB, HIGH);
  delay(500);
  digitalWrite(BatteryLevelLEDB, LOW);
  delay(500);
  digitalWrite(BatteryLevelLEDB, HIGH);
  delay(500);
  digitalWrite(BatteryLevelLEDB, LOW);
  delay(500);
  digitalWrite(BatteryLevelLEDB, HIGH);
  delay(500);
  digitalWrite(BatteryLevelLEDB, LOW);

  //Make light green while running max strength test
  digitalWrite(BatteryLevelLEDG, HIGH);

  //Run max Strength testing
  for(int i = 0; i < 25; i++){
    emgRead();
    
    for(int j = 0; j < 5; j++){
      if(getEmgValue() > maxStrength[j]){
        maxStrength[j] = getEmgValue();
        break;
      }
    }

    delay(25);
  }
  for(int i = 0; i < 5; i++){
    maxAvg += maxStrength[i];
  }
  maxAvg = maxAvg / 5;

  //Turn light off
  digitalWrite(BatteryLevelLEDG, LOW);

  //Blink light blue three times
  digitalWrite(BatteryLevelLEDB, HIGH);
  delay(500);
  digitalWrite(BatteryLevelLEDB, LOW);
  delay(500);
  digitalWrite(BatteryLevelLEDB, HIGH);
  delay(500);
  digitalWrite(BatteryLevelLEDB, LOW);
  delay(500);
  digitalWrite(BatteryLevelLEDB, HIGH);
  delay(500);
  digitalWrite(BatteryLevelLEDB, LOW);

  //Make light green while running normal strength test
  digitalWrite(BatteryLevelLEDG, HIGH);

  //Run normal Strength testing 
  for(int i = 0; i < 25; i++){
    emgRead();

    normalAvg += getEmgValue();
    delay(25);
  }
  normalAvg = normalAvg / 25;
  
  //Turn light off
  digitalWrite(BatteryLevelLEDG, LOW);

  //set base and max thresholds
  g_baseThreshold = normalAvg - 15;
  g_maxThreshold = maxAvg + 50;
}
