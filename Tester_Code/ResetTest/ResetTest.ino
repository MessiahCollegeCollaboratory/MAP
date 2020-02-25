int Reset = 4;

void setup() {  
  digitalWrite(Reset, HIGH);
  delay(200); 
  pinMode(Reset, OUTPUT);     
  Serial.begin(9600);
  delay(200);

  Serial.println("A");
  delay(1000);               
  Serial.println("B");
  delay(1000);               
  Serial.println("Now we are Resetting Arduino Programmatically");
  Serial.println();
  delay(1000);
  digitalWrite(Reset, LOW);
  Serial.println("Arduino will never reach there.");
}
void loop() 
{


}
