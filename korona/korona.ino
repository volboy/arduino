/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo


int val=100;    // variable to read the value from the analog pin

int pos = 0;
int timer=0;
void setup() {
  myservo.attach(6);  // attaches the servo on pin 9 to the servo object
  myservo.write(0);
delay(1000); 

Serial.begin(9600);

 
}

void loop() {
 
 val = analogRead(A0); 
 Serial.println(val);

 
 if (val<50) {
 timer++;
 }
 if (timer>500){
  timer=0;
for (pos = 0; pos <= 100; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                      // waits 15ms for the servo to reach the position
  }
  for (pos = 100; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(5);                       // waits 15ms for the servo to reach the position
  }

 }


}
