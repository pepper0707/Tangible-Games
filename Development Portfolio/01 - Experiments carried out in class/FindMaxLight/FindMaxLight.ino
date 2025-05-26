#include <Servo.h>

Servo myservo;  // Create Servo object to control a servo

int pos = 0;    // Variable to store the servo position

int maxLight = 0;
int maxLightAngle = 0;

void setup() {
  myservo.attach(9);  // Attaches the servo on pin 9 to the Servo object

    for (pos = 0; pos <= 180; pos += 1) { // Goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    int lightValue = analogRead(A0);
    if (lightValue > maxLight){
      maxLight = lightValue;
      maxLightAngle = pos;
    }


    myservo.write(pos);              // Tell servo to go to position in variable 'pos'
    delay(15);                       // Waits 15 ms for the servo to reach the position
  }

  myservo.write(maxLightAngle);       // Point towards strongest source of light
}

void loop() {
  
}
