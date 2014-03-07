
#include <Servo.h>

// Gun will rotate on its platform back and forth
// Then tilt downwards, stop, and shoot
// After, it will tilt upwards to return to starting position

// Set the number of shots to take
int numShots = 3;


Servo triggerServo;
Servo tiltServo;
Servo rotateServo;
int triggerPin = 5;
int gunPowerPin = 2;
int tiltPin = 3;
int rotatePin = 6;    //pin to rotate platform

void setup()
{
  // attach servos and define pins
  pinMode(gunPowerPin, OUTPUT);
  pinMode(triggerPin, OUTPUT);
  pinMode(tiltPin, OUTPUT);
  pinMode(rotatePin, OUTPUT);
  triggerServo.attach(triggerPin);
  tiltServo.attach(tiltPin);
  rotateServo.attach(rotatePin);
  
  for (int i=0; i<numShots; i++){
    shootRoutine();
  }
}

void loop(){
  //shootRoutine();
}



void shootRoutine(){
  // turn gun off
  digitalWrite(gunPowerPin,LOW);
  // trigger back
  triggerServo.writeMicroseconds(1900);
  delay(500);
  // rotate a little CW from top
  for (int i=0; i<22; i++){
    rotateServo.writeMicroseconds(1600);
    delay(50);
  }
  rotateServo.writeMicroseconds(1500);
  // rotate a little CCW from top
  for (int i=0; i<15; i++){
    rotateServo.writeMicroseconds(1400);
    delay(50);
  }
  rotateServo.writeMicroseconds(1500);
  // tilt to neutral
  tiltServo.writeMicroseconds(1800);
  delay(500);
  
  // tilt up
  for (int i=0; i<50; i++){
    tiltServo.writeMicroseconds(1800-6*i);
    delay(50);
  }
  delay(1000);
  // turn gun on
  digitalWrite(gunPowerPin,HIGH);
  delay(1000);
  //trigger forward to shoot
  triggerServo.writeMicroseconds(1000);
  delay(800);
  
  //trigger back
  triggerServo.writeMicroseconds(1900);
  delay(500);
  // turn gun off
  digitalWrite(gunPowerPin,LOW);
  delay(500);
  // tilt down
  for (int i=0; i<50; i++){
    tiltServo.writeMicroseconds(1500+6*i);
    delay(50);
  }
  delay (500);
}

