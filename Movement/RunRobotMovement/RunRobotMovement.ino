#include <SoftwareSerial.h>
#include <Servo.h>

#include "QTI.h"
#include "ColorSensor.h"

#include "Utilities.h"


#define PIN_LEFT_QTI                 A1
#define PIN_CENTER_LEFT_QTI          A2
#define PIN_CENTER_RIGHT_QTI         A3
#define PIN_RIGHT_QTI                A4
#define PIN_LEFT_QTI_BACK            A8
#define PIN_CENTER_LEFT_QTI_BACK     A9
#define PIN_CENTER_RIGHT_QTI_BACK    A10
#define PIN_RIGHT_QTI_BACK           A12
#define PIN_COLOR_SENSOR             4  
#define PIN_COLOR_SENSOR_UNUSED      255
#define PIN_LEFT_MOTOR               10
#define PIN_RIGHT_MOTOR              11
#define PIN_TRIGGER                  5
#define PIN_PLATFORM                 6
#define PIN_TILT                     3
#define PIN_FIRE                     2
#define PIN_PHOTO                    A5
#define PIN_DEBUG_IMAGE              8
#define PIN_DEBUG_CONN               8


// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

// picture yourself facing same direction as robot is facing:
// leftmost QTI - 1, 2, 3, 4 - rightmost QTI

// setting QTI input pins
QTI qti[] = {QTI(PIN_LEFT_QTI), QTI(PIN_CENTER_LEFT_QTI), QTI(PIN_CENTER_RIGHT_QTI), QTI(PIN_RIGHT_QTI)};
QTI qti_back[] = {QTI(PIN_LEFT_QTI_BACK), QTI(PIN_CENTER_LEFT_QTI_BACK), QTI(PIN_CENTER_RIGHT_QTI_BACK), QTI(PIN_RIGHT_QTI_BACK)};

//ColorSensor color_sensor(PIN_COLOR_SENSOR, PIN_COLOR_SENSOR_UNUSED);
Servo left_servos, right_servos;

//raspberry pi stuff
char RPi_buffer[20];
int RPi_target_x, RPi_target_y;

long photo_threshold = 0;
void setup() {
  // set up the ADC
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  //own prescaler
  ADCSRA |= PS_32;    // set our own prescaler to 64 
  
  delay(500);
  Serial.begin(9600);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  setupGun();

  left_servos.attach(PIN_LEFT_MOTOR);
  right_servos.attach(PIN_RIGHT_MOTOR);
  stopMotor();

  setupPhotoSensor();
 
  for(int i = 0; i < 4; ++i) {
    qti[i].Setup();
    qti_back[i].Setup();
  }
  //ignore initial qti readings
  for(int j = 0; j < 10; ++j) {
    for(int i = 0; i < 4; ++i) {
      qti[i].UpdateState();
      qti_back[i].UpdateState();
      //Serial.print(' ');
    }
    //Serial.println();
    //j/=10;
  }

  //setup debug pins
  pinMode(PIN_DEBUG_IMAGE, OUTPUT);
}

void loop() {
  Serial.println("Running");
  //make sure RPi is connected and operating properly
  while(!RPi_check()) {
    delay(100);
  }
  
  //show that rpi has connected
  for(int i = 0; i < 5; ++i) {
    digitalWrite(PIN_DEBUG_CONN, HIGH);
    delay(100);
    digitalWrite(PIN_DEBUG_CONN, LOW);
    delay(100);
  }
  
  leaveStartingArea();
  
 /* if(RPi_check()) {
    for(int i = 0; i < 5; ++i) {
      digitalWrite(PIN_DEBUG_CONN, HIGH);
      delay(100);
      digitalWrite(PIN_DEBUG_CONN, LOW);
      delay(100);
    }
  }

  while(1){
    aimAndFire(1);
    delay(2000);
  }*/

  // turn to targets 1, 2, 3, and shoot them.
  // then turn back to main line.
  for(int i = 0; i < 3; ++i)
    turnAndShoot(i);
  
  findEndingArea();
  
  stopMotor();
  stall();
}

#define CENTER 90
#define DELTA  90
#define MS_PER_CM 33
   
void lineFollow(){
  bool off[4];
  for(int i = 0; i < 4; ++i) {
    off[i] = qti[i].isBlack();
  }
  if(off[0] && off[1] && off[2] && off[3]) {
    left_servos.write(CENTER + DELTA);
    right_servos.write(CENTER - DELTA);
  }
  else if(off[0] && off[1] && !off[2] && off[3]) {
    left_servos.write(CENTER + DELTA);
    right_servos.write(CENTER - DELTA/8);
  }
  else if(off[0] && !off[1] && off[2] && off[3]) {
    left_servos.write(CENTER + DELTA/8);
    right_servos.write(CENTER - DELTA);
  }
  else if(off[0] && off[1] && off[2] && !off[3]) {
    left_servos.write(CENTER + DELTA);
    right_servos.write(CENTER - DELTA/16);
  }
  else if(!off[0] && off[1] && off[2] && off[3]) {
    left_servos.write(CENTER + DELTA/16);
    right_servos.write(CENTER - DELTA);
  }
  else {
      left_servos.write(CENTER + DELTA);
      right_servos.write(CENTER - DELTA);
  }
}

void lineFollow(int time) {
  long start_time = millis();
  long stop_time = start_time + time;
  while(millis() < stop_time) {
    lineFollow();
    delay(1);
  }
}

void lineFollowReverse(int time) {
  long start = millis();
  while(millis() - start < time) {
    lineFollowReverse();
    delay(1);
  }
}

void lineFollowReverse(){
  bool off[4];
  for(int i = 0; i < 4; ++i) {
    off[i] = qti_back[i].isBlack();
  }
  if(off[0] && off[1] && off[2] && off[3]) {
    left_servos.write(CENTER - DELTA);
    right_servos.write(CENTER + DELTA);
  }
  else if(off[0] && off[1] && !off[2] && off[3]) {
    left_servos.write(CENTER - DELTA/8);
    right_servos.write(CENTER + DELTA);
  }
  else if(off[0] && !off[1] && off[2] && off[3]) {
    left_servos.write(CENTER - DELTA);
    right_servos.write(CENTER + DELTA/8);
  }
    else if(off[0] && off[1] && off[2] && !off[3]) {
    left_servos.write(CENTER - DELTA/16);
    right_servos.write(CENTER + DELTA);
  }
  else if(!off[0] && off[1] && off[2] && off[3]) {
    left_servos.write(CENTER - DELTA);
    right_servos.write(CENTER + DELTA/16);
  }
  else {
    left_servos.write(CENTER - DELTA);
    right_servos.write(CENTER + DELTA);
  }
}

void forward(int time) {
  left_servos.write(CENTER + DELTA);
  right_servos.write(CENTER - DELTA);
  delay(time);
}

void backward(int time) {
  left_servos.write(CENTER - DELTA);
  right_servos.write(CENTER + DELTA);
  delay(time);
}

void stopMotor() {
  left_servos.write(90);
  right_servos.write(90);
}

void rotateLeft2() {
  left_servos.write(78);
  right_servos.write(70);
  delay(1000);
  while(qti_back[2].isBlack()) {}
  stopMotor();
}

void rotateRight2() {
  left_servos.write(95);
  right_servos.write(110);
  delay(1000);
  while(qti_back[1].isBlack()) {}
  lineFollowReverse(14*MS_PER_CM);
  stopMotor();
}

void setupPhotoSensor() {
  pinMode(PIN_PHOTO, INPUT);
  for(int i = 0; i < 100; ++i) {
    photo_threshold += analogRead(PIN_PHOTO);
    delay(10);
    Serial.println(analogRead(PIN_PHOTO));
  }
  photo_threshold /= 100;
  photo_threshold -= 60;
  Serial.print("Photo threshold: ");
  Serial.println(photo_threshold);
}

int averagePhotoReading() {
  int val = 0;
  for(int i = 0; i < 25; ++i) {
    val += analogRead(PIN_PHOTO);
    delay(10);
  }
  return val / 25;
}

void leaveStartingArea(){
  // This function reads when the LEDs of the starting area get lit, then
  // the robot moves out of the starting box onto the line.
  
  // TODO: read when LEDs turn on. (perhaps also include timeout to ensure that we don't wait for eternity
  // ??? do LEDs affect QTI sensors ???
  Serial.println("Photo threshold:");
  Serial.println(photo_threshold);
  while(averagePhotoReading() > photo_threshold) delay(10);
  Serial.println("Line following...");
  while (true){
    lineFollow();
    // when all QTIs read white, stop.
    // this indicates that we are at the starting box boundary.
    if (qti[0].isWhite() && qti[1].isWhite() && qti[2].isWhite() && qti[3].isWhite())
      break;
  }
  Serial.println("Found");

  forward(500);
  // code for moving forward like 2 inches
  Serial.println("Moving forward past starting boundary line.");
}


void turnAndShoot(int line){
  // This function is called when robot is on the main line.
  // It will follow the main line until it reaches a left turn, then it will
  // make that left turn and line-follow to the blue acrylic square, shoot target,
  // move back to main line, then turn left.
  
  // line follow on center line until reach the left turn
  Serial.println("Line following...");
  
  boolean (*qti_line_check)(QTI*) = qti_set_CheckAllWhite;
  if(line == 1)
    qti_line_check = qti_set_Check3White;
  qti_set_Reset();
  while(true) {
    lineFollow();
    if(qti_line_check(qti))
      break;
  }
  Serial.println("Reached line. Turning left.");

  rotateLeft2();
  
  qti_set_Reset();
  while(true) {
    lineFollowReverse();
    if(qti_set_CheckAllWhite(qti))
      break;
  }
  
  Serial.println("Reached white line");
  
  boolean back_qti_found_white_line = false;
  
  int first_white_line_time = millis();
  
  forward(6.5*MS_PER_CM);
  
  qti_set_Reset();
  while(true) {
    lineFollow();
    if(qti_set_CheckAllWhite(qti_back)) {
      back_qti_found_white_line = true;
    }
    if(qti[1].isWhite(false) && qti[2].isWhite(false))
      break;
  }
  
  int block_time = millis();
  
  forward(12*MS_PER_CM);
  
  stopMotor();
  delay(1000);
  
  aimAndFire(line);
  
  Serial.println("Done shooting. Turning around/Getting oriented on line.");
  if(!back_qti_found_white_line) {
    Serial.println("back qtis finding white line");
    while(true) {
      lineFollow();
      if(qti_back[1].isWhite() && qti_back[2].isWhite())
        break;
    }
    lineFollow(2*MS_PER_CM);
    stopMotor();
    delay(200);
  }

  //find white line
  qti_set_Reset();
  while(true) {
    lineFollowReverse();
    if(qti_set_CheckAllWhite(qti_back))
      break;
  }

  //go off line
  while(true) {
    lineFollowReverse();
    if(qti_back[0].isBlack() && qti_back[3].isBlack())
      break;
  } 

  //start turn later if not enough momentum
  if(block_time - first_white_line_time < 3000) {
    backward(100);
  }

  Serial.println("Reached main line. Turning left");
  rotateRight2();
  
  qti_set_Reset();
  while(true) {
    lineFollow();
    if(qti_line_check(qti))
      break;
  }
  forward(2*MS_PER_CM);
}

void findEndingArea() {
  while(1) {
    lineFollow();
    if(qti[1].isWhite() && qti[2].isWhite())
      break;
  }
  forward(13*MS_PER_CM);
}




