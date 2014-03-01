#include "QTI.h"

#include "Utilities.h"

#define PIN_LEFT_QTI            3
#define PIN_CENTER_LEFT_QTI     4
#define PIN_CENTER_RIGHT_QTI    5
#define PIN_RIGHT_QTI           6
#define PIN_COLOR_SENSOR        7
#define PIN_COLOR_SENSOR_UNUSED 8

// picture yourself facing same direction as robot is facing:
// leftmost QTI - 1, 2, 3, 4 - rightmost QTI

// setting QTI input pins
QTI qti[] = {QTI(PIN_LEFT_QTI), QTI(PIN_CENTER_LEFT_QTI), QTI(PIN_CENTER_RIGHT_QTI), QTI(PIN_RIGHT_QTI)};

ColorSensor color_sensor(PIN_COLOR_SENSOR, PIN_COLOR_SENSOR_UNUSED);
  
void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(RCTime(2));	    // Connect to pin 2, display results
  delay(250);			    // Wait 250ms
  // for voltage source of 3.3 V,
  // BLACK - >300
  // WHITE LINE & RED SQUARE - 5-30
  
  // movement:
  leaveStartingArea();
  
  // turn to targets 1, 2, 3, and shoot them.
  // then turn back to main line.
  turnAndShoot(); 
  turnAndShoot(); 
  turnAndShoot(); 

  // TODO: red = stop/done? 
  // This part not done with QTIs, but with some other sensor on bottom of robot.
}
   
void lineFollow(){
  bool on[4];
  for(int i = 0; i < 3; ++i)
    on[i] = qti[i].isWhite();
   /*
  if (RCTime(qti1) >= QTI_TIME && RCTime(qti2) >= QTI_TIME && RCTime(qti3) >= QTI_TIME && RCTime(qti4) >= QTI_TIME){
    // All QTIs read black, therefore:
    // move straight forward
  }
  else if ((RCTime(qti1) >= QTI_TIME && RCTime(qti2) < QTI_TIME && RCTime(qti3) >= QTI_TIME && RCTime(qti4) >= 200)){
    // QTI 2 -> second QTI from left reads white, therefore:
    // turn slightly to the right
  }
  else if ((RCTime(qti1) < 200 && RCTime(qti2) >= 200 && RCTime(qti3) >= 200 && RCTime(qti4) >= 200)){
    // QTI 2 -> lefmost QTI, therefore:
    // turn more to the right
  }
  else if ((RCTime(qti1) < 200 && RCTime(qti2) < 200 && RCTime(qti3) >= 200 && RCTime(qti4) >= 200)){
    // QTI 1 and 2 -> left QTIs read white, therefore:
    // rotate to the right
  }
  else if ((RCTime(qti1) >= 200 && RCTime(qti2) >= 200 && RCTime(qti3) < 200 && RCTime(qti4) >= 200)){
    // QTI 3 -> second QTI from right reads white, therefore:
    // turn slightly to the left
  }
  else if ((RCTime(qti1) >= 200 && RCTime(qti2) >= 200 && RCTime(qti3) >= 200 && RCTime(qti4) < 200)){
    // QTI 3 -> rightmost QTI reads white, therefore:
    // rotate more to the left
  }
  else if ((RCTime(qti1) >= 200 && RCTime(qti2) >= 200 && RCTime(qti3) < 200 && RCTime(qti4) < 200)){
    // QTI 3 and 4 -> right QTIs read white, therefore:
    // rotate more to the left
  }
  else {
  }*/
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
  
  // TODO: now move forward past this boundary.
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
    if (qti[0].isWhite() && qti[1].isWhite() && qti[2].isBlack() && qti[3].isBlack())
      break;
  }
  // TODO: code for turning left
  Serial.println("Reached line. Turning left.");
  
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

