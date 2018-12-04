/**
* @author Keith Lim, Sam Gullinello, Keller Martin, Jared Butler
* The Main file for working the MAP
* Consists of the latest working code as of 11/15/18
**/


#include <Servo.h>
Servo myservo;
Servo myservo2;
int pos = 0;
int servoState = 0;
bool gripOpen = false;
const int emgArrayLength = 25;
int32_t emgArray[emgArrayLength] = {0};
int readIndex;
int32_t emgValue;
int32_t rmsValue;
int32_t rmsValue2;
int32_t total;
int32_t total2;
int getRMSSignal = 0;
int32_t *maxNum;
int32_t *first;
int32_t *last;
int32_t maxValue = 0;
int32_t emgRead = 0;

// Calibration Sequence Variables
int32_t emgAvg = 0;
int32_t loopRuns = 0;

// Analog Pins
int fsr = A5;
int BatteryLevelReadBoth = A3;
int BatteryLevelReadBat2 = A2;
int thresholdPot = A4;
int emg = A1;

//Digital Pins
int BatteryLevelLEDR = 2;
int BatteryLevelLEDG = 4;
int BatteryLevelLEDB = 3;
int led = 7;
int servo = 10;
int servo2 = 11;


/**
* Start of Muscle Motor class.
*
* Muscle Motor class takes in variables and signals and
* decide on what the output should be. For example
* Takes in a signal and outputs a boolean to signify that
* we should or not move the arm.
*
* ##grip is always open when true and close when false
**/
class MuscleMotor {
private:
  bool openGrip;
  bool currentGrip;
  int16_t maxSignal;
  //int16_t minSignal;
  int amountOfSeconds;
  int16_t fsrReading;


public:
  MuscleMotor(/*int16_t, int16_t*/);                  
  void readSignal(int16_t);
  bool checkGripPosition(int16_t);
  void setMaxSignal(int16_t);
  int32_t  rms(int32_t);
  void openCloseActuator();  
  void setFsrReading(int16_t);
  void indicateBatteryLevel();
  void emgCal();         
};

/*
 * New class to make printing easier
 */
class Monitor {
private:
  double factor;
  int precision;
  
public:
  Monitor();
  void p(int);
  void pln(int);
};

Monitor::Monitor(){
  this->factor = 4.88759;
  this->precision = 0;
}

void Monitor::p(int in){
  Serial.print(in * factor, precision);
  Serial.print("\t");
}

void Monitor::pln(int in){
  Serial.println(in * factor, precision);
}

Monitor* Print = new Monitor();

//Instantiate the class. Default threshold set to 25. 
MuscleMotor* mm = new MuscleMotor();

/**
* Set the fields to a default value.
**/
MuscleMotor::MuscleMotor()
{
  this->openGrip = true;
  this->currentGrip = true;
  //this->maxSignal = maxsignal;
  //this->minSignal = minsignal;
  this->amountOfSeconds = 0;
}

void MuscleMotor::setMaxSignal(int16_t maxSignal)
{
  this->maxSignal = maxSignal;
}

void MuscleMotor::setFsrReading(int16_t fsrReading){
  this->fsrReading = fsrReading;
}



// check to see if the grip should be open or close
// have to make a new function that calculates 2 seconds
bool MuscleMotor::checkGripPosition(int16_t bicepValue)
{

  // A hack to allow the actuator to work. We've to change
  // the amount of seconds to be more than the time for
  // the grip to change. Once it is higher than the time
  // for the grip to change (2000 or 2 seconds). We change
  // the time press back to 0;

  if (amountOfSeconds >= 2000) {
  amountOfSeconds = 0;
  }
  
  //If the muscle is squeezed for 1.5 seconds, Switch the
  //grip, save the grip to the currentGrip and then
  // return the openGrip.
  if (amountOfSeconds >= 1900) {

    openGrip = !openGrip;
    currentGrip = openGrip;
    amountOfSeconds+=100;
    return openGrip;

    //if the value of Bicep and tripcep is not high (not squeezed)
    // return the currentGrip Position.
  } else if (bicepValue < maxSignal) {
    amountOfSeconds = 0;
    return currentGrip;

    //if the muscles is squeezed for less than 2 seconds,
    //delay the system for 1 millisecond, then add 1 millisecond
    // to the total amount of seconds.
  } else {
    delay(100);
    amountOfSeconds += 100;
  }

}

/**
* Calculates Root Mean Square (RMS) of last 25 readings when called, including the newest emgValue reading
* Removes the maximum value, in order to guarentee a more uniform reading
**/
int32_t MuscleMotor::rms(int32_t emgValue) {
  //Updates array with new value from the emg
  total = total - emgArray[readIndex];
  emgArray[readIndex] = sq(emgValue);
  total = total + emgArray[readIndex];
  total2 = total2 + emgArray[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= emgArrayLength) {
    readIndex = 0;
  }

  //adds maximum value back in, updates it, and then removes it again
  total += maxValue;                     

  first = emgArray;
  last = emgArray+emgArrayLength;

  maxNum = maxElement(first, last);
  maxValue = *maxNum;

  total -= *maxNum;                       


  //calculates rms
  rmsValue = (sqrt(total/(emgArrayLength - 1)));

  //Print things to the monitor. Creates the plot
  Print->p(emgValue);
  //Print->p(this->maxSignal);
  //Print->p(this->fsrReading);
  Print->pln(rmsValue);
  delay(25);

  return rmsValue;
}

/**
*  Open or close the actuator based on a boolean ___________ and a pressLength int
**/

void MuscleMotor::openCloseActuator() {
  // if we receive a boolean that says, open is true
  // we move the actuator so that it opens.
  // else we close it.

  if (currentGrip) {


    if (amountOfSeconds >= 2000) {                                                         
      //writing onto the servo to close it
      digitalWrite(led, HIGH);
      for (/*pos = 0*/; pos < 180; pos = pos + 1){
        myservo2.write(pos);
        myservo.write(pos);
        mm->setFsrReading(analogRead(fsr));
        delay(5);
        
        if(fsrReading > 600){
          //digitalWrite(led, LOW);
          break;
        }
      }
    }
    
  } else {
    if (amountOfSeconds >= 2000) {
      //writing onto the servo to open it
      digitalWrite(led, LOW);
      for (/*pos = 180*/; pos > 1; pos = pos - 1) {
        myservo2.write(pos);
        myservo.write(pos);
        delay(5);
        
      }
    }
    
  }

  
}
/**
*  Light RGB LED to different colors to signal the battery level.
**/

void MuscleMotor::indicateBatteryLevel() {
  
  int bat2Level = analogRead(BatteryLevelReadBat2);
  int bat1Level = analogRead(BatteryLevelReadBoth);
  int greenThreshold = 818; // 4V*1023/5V
  int redThreshold = 655; // 3.2V*1023/5V
  //Serial.print(bat2Level);
  //Serial.print("\t");
  //Serial.println(bat1Level);

    digitalWrite(BatteryLevelLEDB, LOW);

    if((bat2Level > greenThreshold)/* && (bat1Level > greenThreshold)*/) {
    digitalWrite(BatteryLevelLEDR, LOW);
    digitalWrite(BatteryLevelLEDB, LOW);
    digitalWrite(BatteryLevelLEDG, HIGH);
    }
    
    else if((bat2Level > redThreshold)/* && (bat1Level > redThreshold)*/) {
      digitalWrite(BatteryLevelLEDR, LOW);
      digitalWrite(BatteryLevelLEDG, LOW);
      digitalWrite(BatteryLevelLEDB, HIGH);
    }
    
    else if((bat2Level <= redThreshold)/* || (bat1Level <= redThreshold)*/){
      digitalWrite(BatteryLevelLEDR, HIGH);
      digitalWrite(BatteryLevelLEDG, LOW);
      digitalWrite(BatteryLevelLEDB, LOW);
    }
    
}

void MuscleMotor::emgCal(){
  emgAvg = 0;
  
  for(int i = 0; i < 25; i++){
    emgAvg += analogRead(emg);
    delay(25);
  }

  emgAvg = emgAvg / 25;
}


int32_t* maxElement(int32_t * first, int32_t * last){
  
  maxNum = first;
  
  while(++first != last){
    if(*first > *maxNum){
      maxNum = first;
    }
  }

  return maxNum;
}

void setup() {
  // put your setup code here, to run once:
  myservo.attach(servo);
  myservo2.attach(servo2);
  myservo.write(servoState);
  myservo2.write(servoState);
  pinMode(thresholdPot, INPUT);
  pinMode(emg, INPUT);
  pinMode(fsr, INPUT);
  pinMode(BatteryLevelReadBoth, INPUT);
  pinMode(BatteryLevelReadBat2, INPUT);
  pinMode(led, OUTPUT);
  pinMode(BatteryLevelLEDR, OUTPUT);
  pinMode(BatteryLevelLEDG, OUTPUT);
  pinMode(BatteryLevelLEDB, OUTPUT);

  Serial.begin(9600);
  *maxNum = 0;
  *first = 0;
  *last = 0;

  mm->emgCal();
}


void loop() {
  // put your main code here, to run repeatedly:
  // Rule of thumb for optimization:
  // The code within this box should not be more than 8 lines
  emgRead = analogRead(emg);

  mm->indicateBatteryLevel();

  //set fsrReading variable
  mm->setFsrReading(analogRead(fsr));

  //getRMSSignal = mm->rms(analogRead(emg) - 334);
  getRMSSignal = mm->rms(emgRead - emgAvg);

  // Setting variable threshold
  mm->setMaxSignal(analogRead(thresholdPot));
  
  gripOpen = mm->checkGripPosition(getRMSSignal);
  mm->openCloseActuator();

  loopRuns++;
}
