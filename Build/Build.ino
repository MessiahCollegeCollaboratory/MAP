/**
* @author Keith Lim, Sam Gullinello, Keller Martin, Jared Butler
* The Main file for working the MAP
* Consists of the latest working code as of 3/4/19
**/

/**
 * -------------------Future Changes-------------------
 * LOOK FOR A COMMENT LIKE ^^^^^ TO FIND NECESSARY CHANGES
 * 
 * -------------------Future Changes-------------------
 * Minimize global variables
 */

#include <Servo.h>
#include "Properties.h"
#include "Print.h"
#include "Timer.h"
#include "Emg.h"

//-------------------Below Are functions not in mm class-----------------------//

/** ----- NONE ----- */

//--------------------------Start of mm Class----------------------------------//
/**
* Muscle Motor class keeps track of the state of the motors
* and provides functions which control the motors. For example, 
* checkGripPosition() takes in the most recent rms signal and 
* decides whether or not the hand should be opened or closed.
*
* ##grip is always open when false and closed when true
* ^^^^^check if this is true
* 
* -------------------Future Changes-------------------
* Move Emg related functions and data into separate class
* 
* openHand() and closeHand() may need to be made public later
* 
**/
class MuscleMotor {
  private:
    // Variables concerning state of motors/apparatus
    bool currentGrip;
    int32_t threshold;
    int servoPos;

    //Motors
    Servo myservo;
    Servo myservo2;

    // Functions concerning state of motors/apparatus
    void openHand();
    void closeHand();
    
  
  public:
    // Constructor(s)
    MuscleMotor();   

    // Functions which make decisions or control states
    void updateGripPosition(int32_t);
    void indicateBatteryLevel();

    // Getters and Setters
    void setThreshold(int32_t); 
    int32_t getThreshold();
    Servo getLeftMotor();
    Servo getRightMotor();  
};

//---------------------Instantiate necessary classes---------------------//

MuscleMotor* mm = new MuscleMotor();
Monitor* Print = new Monitor();
Timer* Time = new Timer();
Emg* HandEmg = new Emg();

//---------------------MuscleMotor class definitions---------------------//
/**
 * MuscleMotor class constructor
 * 
 * Sets initial values
 */
MuscleMotor::MuscleMotor()
{
  this->currentGrip = true;
  this->servoPos = 0;
}

void MuscleMotor::setThreshold(int32_t threshold)
{
  if(threshold < MAX_FLEX_THRESHOLD){
    this->threshold = threshold;
  }
}

int32_t MuscleMotor::getThreshold(){
  return this->threshold;
}

Servo MuscleMotor::getLeftMotor(){
  return this->myservo;
}

Servo MuscleMotor::getRightMotor(){
  return this->myservo2;
}

/**
 * Main decision making function. This keeps track of how long
 * the muscles are being flexed for, and changes the state of
 * the motors if necessary.
 */
void MuscleMotor::updateGripPosition(int32_t emgVal){
  // If Timer hasn't been initialized yet, getTime() returns 0
  // This if statement only executes once and never again
  if(!Time->getTime()){

    // This sets the Timer to 0 on first run-through
    Time->resetTimer();

  // Check to see if muscle has been flexed longer than specified time
  // If so, this toggles the state of the motors and sets Timer to 0
  } else if(Time->getTime() > TIME_OF_FLEX){

    // Toggle motor state
    currentGrip = !currentGrip;

    // Move motors based upon new state
    if(currentGrip){
      closeHand();
    } else{
      openHand();
    }

    // Set Timer to 0
    Time->resetTimer();

  // Check to see if muscle has been relaxed below threshold
  // If so, reset Timer to 0
  } else if(emgVal < threshold){

    Time->resetTimer();
  }

  /**
   * If none of the if statements are true, this function does nothing.
   * Next time it is called, the state may be different, so it may then
   * perform an action. If the function does nothing, the Timer continues
   * counting.
   */
}


/**
 * This opens the hand when called.
 * It also turns on an LED for testing purposes
 */
void MuscleMotor::openHand(){

  digitalWrite(led, HIGH);
  for (; servoPos > 1; servoPos--) {
    myservo2.write(servoPos);
    myservo.write(servoPos);
    
    delay(5); 
  }
}


/**
 * This closes the hand when called
 * It also turns off an LED for testing purposes
 * 
 * If the value of the fsr is larger than a set threshold
 * immediately stop closing the hand. 
 */
void MuscleMotor::closeHand(){
  
  digitalWrite(led, LOW);
  for (; servoPos < 180; servoPos++){
    myservo2.write(servoPos);
    myservo.write(servoPos);
   
    delay(5);
  }
}


/**
*  Light RGB LED to different colors to signal the battery level.
*  
*  -------------------Future Changes-------------------
*  This needs updating to prevent possible flickering
**/
void MuscleMotor::indicateBatteryLevel() {
  // Read battery levels
  int batLevel = analogRead(BatteryLevelRead);

  // This is be true if Battery is charged within working capacity
  if((batLevel > LED_GREEN_THRESHOLD)/* && (bat1Level > greenThreshold)*/) {
    digitalWrite(BatteryLevelLEDR, LOW);
    digitalWrite(BatteryLevelLEDB, LOW);
    digitalWrite(BatteryLevelLEDG, HIGH);
  }

  // This is true if battery is beginning to run low
  else if((batLevel > LED_RED_THRESHOLD)/* && (bat1Level > redThreshold)*/) {
    digitalWrite(BatteryLevelLEDR, LOW);
    digitalWrite(BatteryLevelLEDG, LOW);
    digitalWrite(BatteryLevelLEDB, HIGH);
  }

  // This is true if battery is too low to run the hand
  else if((batLevel <= LED_RED_THRESHOLD)/* || (bat1Level <= redThreshold)*/){
    digitalWrite(BatteryLevelLEDR, HIGH);
    digitalWrite(BatteryLevelLEDG, LOW);
    digitalWrite(BatteryLevelLEDB, LOW);
  }
}


/**
 * This runs once upon startup
 */
void setup() {
  // Initialize servos
  Servo leftMotor = mm->getLeftMotor();
  Servo rightMotor = mm->getRightMotor();
  leftMotor.attach(servoLeft);
  leftMotor.write(INITIAL_STATE);
  rightMotor.attach(servoRight);
  rightMotor.write(INITIAL_STATE);

  // Initialize pins
  pinMode(thresholdPot, INPUT);
  pinMode(emg, INPUT);
  pinMode(fsr, INPUT);
  pinMode(BatteryLevelRead, INPUT);
  pinMode(led, OUTPUT);
  pinMode(BatteryLevelLEDR, OUTPUT);
  pinMode(BatteryLevelLEDG, OUTPUT);
  pinMode(BatteryLevelLEDB, OUTPUT);

  HandEmg->emgCal();

  // Initialize Serial monitor/plotter
  Serial.begin(9600);
}

/**
 * This code runs repeatedly until shut off.
 * Try to minimize amount of operations performed in the loop
 */
void loop() {
  // Find emg value
  HandEmg->emgRead();

  // Update Battery indicators
  mm->indicateBatteryLevel();

  // Setting variable threshold
  mm->setThreshold(analogRead(thresholdPot));

  // Decide if grip state should change and if so, change it
  mm->updateGripPosition(HandEmg->getEmgValue());

  // This delay is important to keep the amount of loop executions smaller
  // This is important to save battery. 
  delay(LOOP_DELAY);

  //Print->p(HandEmg->getEmgValue());
  Print->pln(mm->getThreshold());
}
