#include <SoftwareSerial.h>
#include <Servo.h>

#include "QTI.h"
#include "ColorSensor.h"

#include "Utilities.h"


#define PIN_LEFT_QTI            A1
#define PIN_CENTER_LEFT_QTI     A2
#define PIN_CENTER_RIGHT_QTI    A3
#define PIN_RIGHT_QTI           A4
#define PIN_COLOR_SENSOR        4
#define PIN_COLOR_SENSOR_UNUSED 255
#define PIN_LEFT_MOTOR          10
#define PIN_RIGHT_MOTOR         11
#define PIN_TRIGGER             5
#define PIN_PLATFORM            6
#define PIN_TILT                3
#define PIN_FIRE                2

// picture yourself facing same direction as robot is facing:
// leftmost QTI - 1, 2, 3, 4 - rightmost QTI

// setting QTI input pins
QTI qti[] = {QTI(PIN_LEFT_QTI), QTI(PIN_CENTER_LEFT_QTI), QTI(PIN_CENTER_RIGHT_QTI), QTI(PIN_RIGHT_QTI)};

//ColorSensor color_sensor(PIN_COLOR_SENSOR, PIN_COLOR_SENSOR_UNUSED);
Servo left_servos, right_servos;
void setup() {
  delay(1000);
  Serial.begin(9600);
  left_servos.attach(PIN_LEFT_MOTOR);
  right_servos.attach(PIN_RIGHT_MOTOR);
  left_servos.write(90);
  right_servos.write(90);
  
  //prevent motor from running
  pinMode(PIN_FIRE, OUTPUT);
  digitalWrite(PIN_FIRE, LOW);
  
  //Serial.println("Preparing color sensor...");
  //color_sensor.prepare();
}

void loop() {
  // for voltage source of 3.3 V,
  // BLACK - >300
  // WHITE LINE & RED SQUARE - 5-30
  // movement:
  //Serial.println(color_sensor.getColor(color_sensor.getColor()));
  //delay(1000);
  //return;
  Serial.println("Running");
  waitForKeyboard();

  leaveStartingArea();
  
  // turn to targets 1, 2, 3, and shoot them.
  // then turn back to main line.
  turnAndShoot(); 
  turnAndShoot(); 
  turnAndShoot(); 
  
  stall();
  // TODO: red = stop/done? 
  // This part not done with QTIs, but with some other sensor on bottom of robot.
}
   
void lineFollow(){
  bool off[4];
  for(int i = 0; i < 4; ++i) {
    off[i] = qti[i].isBlack();
  }
  if(off[0] && off[1] && off[2] && off[3]) {
    left_servos.write(100);
    right_servos.write(80);
  }
  else if(off[0] && !off[1] && off[2] && off[3]) {
    left_servos.write(100);
    right_servos.write(60);
  }
  else if(off[0] && off[1] && !off[2] && off[3]) {
    left_servos.write(120);
    right_servos.write(80);
  }
}

void leaveStartingArea(){
  // This function reads when the LEDs of the starting area get lit, then
  // the robot moves out of the starting box onto the line.
  
  // TODO: read when LEDs turn on. (perhaps also include timeout to ensure that we don't wait for eternity
  // ??? do LEDs affect QTI sensors ???
  Serial.println("Line following...");
  while (true){
    lineFollow();
    // when all QTIs read white, stop.
    // this indicates that we are at the starting box boundary.
    if (qti[0].isWhite() && qti[1].isWhite() && qti[2].isWhite() && qti[3].isWhite())
      break;
  }
  Serial.println("Found");
  //stopMotor();
  //waitForKeyboard();
  forward(500);
  //stopMotor();
  //waitForKeyboard();
  // code for moving forward like 2 inches
  Serial.println("Moving forward past starting boundary line.");
}


void turnAndShoot(){
  // This function is called when robot is on the main line.
  // It will follow the main line until it reaches a left turn, then it will
  // make that left turn and line-follow to the blue acrylic square, shoot target,
  // move back to main line, then turn left.
  
  
  // line follow on center line until reach the left turn
  Serial.println("Line following...");
  while (true){
    lineFollow();
    // when left QTIs read white, stop.
    // this indicates that we are on the left turn.
    if (/*qti[0].isWhite() && qti[1].isWhite() && */qti[2].isWhite() && qti[3].isWhite()) //left qtis ignored in case there is a block there
      break;
  }
  Serial.println("Reached line. Turning left.");
stopMotor();
waitForKeyboard();
  rotateLeft();
  
  // keep line following until we hit the BLUE square thingy.
  while (true){
    lineFollow();
    // TODO: if read BLUE square thingy, do the shooty thingy.
    Serial.println("Reached blue square. Shooting target.");
    // then
    break;
  }
  
  // TODO: execute awesome 180 degree turn HERE
  Serial.println("Done shooting. Turning around/Getting oriented on line.");
  while (true){
    // TODO: turn cw or ccw (idk which), until centered back on line following line.
    // when one of the middle QTIs read white and others still read black, we are close to centered
    // on the white line.
    if (qti[0].isBlack() && qti[1].isWhite() && qti[2].isBlack() && qti[3].isBlack())
      break;
  }
  
  Serial.println("Line following...");
  // line following back to main line
  while (true){
    lineFollow();
    // when all QTIs read white, stop.
    // this indicates that we are back on the main line and need to make a left turn.
    if (qti[0].isWhite() && qti[1].isWhite() && qti[2].isWhite() && qti[3].isWhite())
      break;
  }
  // TODO: execute awesome left turn.
  Serial.println("Reached main line. Turning left.");
}

void forward(int time) {
  left_servos.write(100);
  right_servos.write(80);
  delay(time);
}

void stopMotor() {
  left_servos.write(90);
  right_servos.write(90);
}

void rotateLeft() {
  forward(500);
  left_servos.write(150);
  right_servos.write(30);
  while(!qti[1].isWhite()) {}

}

