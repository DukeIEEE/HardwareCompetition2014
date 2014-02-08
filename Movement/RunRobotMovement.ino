// picture yourself facing same direction as robot is facing:
// leftmost QTI - 1, 2, 3, 4 - rightmost QTI

// setting QTI input pins
int qti1 = 3;
int qti2 = 4;
int qti3 = 5;
int qti4 = 6;
  
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

}

long RCTime(int sensorIn){
   long duration = 0;
   pinMode(sensorIn, OUTPUT);     // Make pin OUTPUT
   digitalWrite(sensorIn, HIGH);  // Pin HIGH (discharge capacitor)
   delay(1);                      // Wait 1ms
   pinMode(sensorIn, INPUT);      // Make pin INPUT
   digitalWrite(sensorIn, LOW);   // Turn off internal pullups
   while(digitalRead(sensorIn)){  // Wait for pin to go LOW
      duration++;
   }
   return duration;
}
   
void lineFollow(){
  if (RCTime(qti1) >= 200 && RCTime(qti2) >= 200 && RCTime(qti3) >= 200 && RCTime(qti4) >= 200){
    // All QTIs read black, therefore:
    // move straight forward
  }
  else if ((RCTime(qti1) >= 200 && RCTime(qti2) < 200 && RCTime(qti3) >= 200 && RCTime(qti4) >= 200)){
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
  else{
    // if none of above cases, do nothing
  }
}

void leaveStartingArea(){
  // This function reads when the LEDs of the starting area get lit, then
  // the robot moves out of the starting box onto the line.
  
  // TODO: read when LEDs turn on. 
  // ??? do LEDs affect QTI sensors ???
  while (true){
    lineFollow();
    // when all QTIs read white, stop.
    // this indicates that we are at the starting box boundary.
    if (RCTime(qti1) < 200 && RCTime(qti2) < 200 && RCTime(qti3) < 200 && RCTime(qti4) < 200)
      break;
  }
  
  // TODO: now move forward past this boundary.
  // code for moving forward like 2 inches
}



void turnAndShoot(){
  // This function is called when robot is on the main line.
  // It will follow the main line until it reaches a left turn, then it will
  // make that left turn and line-follow to the blue acrylic square, shoot target,
  // move back to main line, then turn left.
  
  
  // line follow on center line until reach the left turn
  while (true){
    lineFollow();
    // when left QTIs read white, stop.
    // this indicates that we are on the left turn.
    if (RCTime(qti1) < 200 && RCTime(qti2) < 200 && RCTime(qti3) >= 200 && RCTime(qti4) >= 200)
      break;
  }
  // TODO: code for turning left
  
  // keep line following until we hit the BLUE square thingy.
  while (true){
    lineFollow();
    // TODO: if read BLUE square thingy, do the shooty thingy.
    // then
    break;
  }
  
  // TODO: execute awesome 180 degree turn HERE
  while (true){
    // TODO: turn cw or ccw (idk which), until centered back on line following line.
    // when one of the middle QTIs read white and others still read black, we are close to centered
    // on the white line.
    if (RCTime(qti1) >= 200 && RCTime(qti2) < 200 && RCTime(qti3) >= 200 && RCTime(qti4) >= 200)
      break;
  }
  
  
  // line following back to main line
  while (true){
    lineFollow();
    // when all QTIs read white, stop.
    // this indicates that we are back on the main line and need to make a left turn.
    if (RCTime(qti1) < 200 && RCTime(qti2) < 200 && RCTime(qti3) < 200 && RCTime(qti4) < 200)
      break;
  }
  // TODO: execute awesome left turn.
}

