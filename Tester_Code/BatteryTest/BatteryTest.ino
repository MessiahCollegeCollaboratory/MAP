/*  Code to open and close the hand repeatedly to test
 *  the battery
 */

#include <Servo.h>
Servo myservo;
Servo myservo2;
int pos = 0;
int servoState = 0; //this means the hand starts opened
bool handState = false; //false indicates that hand is open

void setup() {
  // put your setup code here, to run once:
  myservo.attach(10);
  myservo2.attach(11);
  myservo.write(servoState);
  myservo2.write(servoState);
  Serial.begin(9600);
}


void moveMotors(){
  if(handState){
    
    //opens hand
    for (/*pos = 180*/; pos > 1; pos = pos - 1) {
        myservo2.write(pos);
        myservo.write(pos);
        delay(5); 
    }  

    handState = !handState;
  } else {
    
    //closes hand
    for (/*pos = 0*/; pos < 180; pos = pos + 1){
        myservo2.write(pos);
        myservo.write(pos);
        delay(5);
    }

    handState = !handState;
  }
}


void loop() {
  // put your main code here, to run repeatedly:

  moveMotors();
  delay(2000);
  Serial.println("run");
}
