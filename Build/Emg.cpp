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
  delay(500);
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
