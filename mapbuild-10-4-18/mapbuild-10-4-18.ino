/**
* @author Keith Lim, Sam Gullinello, Keller Martin, Jared _____
* The Main file for working the MAP
* Consists of the latest working code as of 10/4/18
**/

#include <Servo.h>
Servo myservo;
int pos = 0;
int servoState = 0;
//int pressLength = 0;                //delete
bool gripOpen = false;
int thresholdPot = A4;
int emg = A5;
int emgArray[25] = {0};
int readIndex;
int emgValue;
int rmsValue;
long total;
int getRMSSignal = 0;
//int threshold;
int *maxNum;
int *first;
int *last;
int maxValue = 0;



void setup() {
  // put your setup code here, to run once:
  myservo.attach(4);
  myservo.write(servoState);
  pinMode(thresholdPot, INPUT);
  pinMode(emg, INPUT);
  Serial.begin(9600);
  *maxNum = 0;
  *first = 0;
  *last = 0;
  //Serial.println(sizeof(int));
}

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
  int16_t minSignal;
  int amountOfSeconds;


public:
  MuscleMotor(int16_t, int16_t);                    //done
  void readSignal(int16_t);
  //int  getAmountOfSeconds();                        //delete
  bool checkGripPosition(int16_t);
  void setMaxSignal(int16_t);
  int  rms(int);
  void openCloseActuator(/*bool, int*/);            //placing the openCloseActuator function in the class
  //void reset();                                   //to be added later. This should reset all data to default so that the program can start running fresh again.
};

/**
* Set the fields to a default value.
**/
MuscleMotor::MuscleMotor(int16_t maxsignal, int16_t minsignal)
{
  this->openGrip = true;
  this->currentGrip = true;
  this->maxSignal = maxsignal;
  this->minSignal = minsignal;
  this->amountOfSeconds = 0;
}

void MuscleMotor::setMaxSignal(int16_t maxSignal)
{
  this->maxSignal = maxSignal;
}

/**
* Outputs the amount of seconds we received the signals                       //delete
**/
/*int MuscleMotor::getAmountOfSeconds()
{
  return this->amountOfSeconds;
}*/

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
int MuscleMotor::rms(int emgValue) {
  //Updates array with new value from the emg
  total = total - emgArray[readIndex];
  emgArray[readIndex] = sq(emgValue);
  total = total + emgArray[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= 25) {
    readIndex = 0;
  }


  //adds maximum value back in, updates it, and then removes it again
  total += maxValue;                     

  first = emgArray;
  last = emgArray+24;

  maxNum = maxElement(first, last);
  maxValue = *maxNum;

  total -= *maxNum;                       


  //calculates rms
  rmsValue = (sqrt(total/24));

  //Print things to the monitor. Creates the plot
  Serial.print(emgValue);
  Serial.print("\t");
 // Serial.print(threshold);
  Serial.print(this->maxSignal);
  Serial.print("\t");
  Serial.println(rmsValue);
  delay(25);

  return rmsValue;
}

/**
*  Open or close the actuator based on a boolean ___________ and a pressLength int
**/

void MuscleMotor::openCloseActuator(/*bool gripOpen, int pressLength*/) {
  // if we receive a boolean that says, open is true
  // we move the actuator so that it opens.
  // else we close it.

  if (currentGrip) {


    if (amountOfSeconds >= 2000) {                                                              //change these to 1900 and it might work
      //writing onto the servo to open it (extend it)
      for (pos = 0; pos < 180; pos = pos + 1){
        myservo.write(pos);
        delay(5);
      }

    }
  } else {
    if (amountOfSeconds >= 2000) {
      //writing onto the servo to close it (retract it)
      for (pos = 180; pos > 1; pos = pos - 1) {
        myservo.write(pos);
        delay(5);
      }
    }

  }
}

int* maxElement(int * first, int * last){
  
  maxNum = first;
  
  while(++first != last){
    if(*first > *maxNum){
      maxNum = first;
    }
  }

  return maxNum;
}

/** ###HARDWARE CONTROL### **/

/**
* func() openCloseActuator
*
* Open or close the actuator based on a boolean
* @parameter bool open: if gripOpen is true, open the hand
* @parameter int pressLength: We use pressLength to measure if it
*     has been 2 seconds. if val >= 2000, then its been 2 seconds.
**/

/* OLD openCloseActuator() function */
/*void openCloseActuator(bool gripOpen, int pressLength) {
  // if we receive a boolean that says, open is true
  // we move the actuator so that it opens.
  // else we close it.

  if (gripOpen) {


    if (pressLength >= 2000) {
      //writing onto the servo to open it (extend it)
      for (pos = 0; pos < 180; pos = pos + 1){
        myservo.write(pos);
        delay(5);
      }

    }
  } else {
    if (pressLength >= 2000) {
      //writing onto the servo to close it (retract it)
      for (pos = 180; pos > 1; pos = pos - 1) {
        myservo.write(pos);
        delay(5);
      }
    }

  }
}*/


/* OLD RMS function
int RMS(int emgValue) {
  total = total - emgArray[readIndex];
  emgArray[readIndex] = sq(emgValue);
  total = total + emgArray[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= 25) {
    readIndex = 0;
  }
  rmsValue = (sqrt(total/25));
  Serial.print(emgValue);
  Serial.print("\t");
  Serial.print(threshold);
  Serial.print("\t");
  Serial.println(rmsValue);
  delay(25);

  return rmsValue;
}
*/

//Instantiate the class.
MuscleMotor* mm = new MuscleMotor(25, 0);

void loop() {
  // put your main code here, to run repeatedly:
  // Rule of thumb for optimization:
  // The code within this box should not be more than 8 lines

  //getRMSSignal = mm->rms(analogRead(emg) - 334);
  getRMSSignal = mm->rms(analogRead(emg) - 575);

  // Setting variable threshold
 // threshold = analogRead(thresholdPot)/10;
  // threshold = 25;
  // this will be used to change gripPosition
  mm->setMaxSignal(analogRead(thresholdPot)/10);
  gripOpen = mm->checkGripPosition(getRMSSignal);
  //pressLength = mm->getAmountOfSeconds();                               //delete
  mm->openCloseActuator(/*gripOpen, pressLength*/);

}

//Git Test