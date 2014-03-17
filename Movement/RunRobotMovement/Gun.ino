#define CENTER_X 335
#define CENTER_Y 240

#define X_GUN_THRESH 80
#define Y_GUN_THRESH 80

const int X_THRESH[] = { 20, 10, 10 };
const int Y_THRESH[] = { 20, 10, 10 };
const float X_FACTOR = .44;
const float Y_FACTOR[2] = {6, 3};

Servo tilt, platform, trigger;
int tilt_angle = 90;
int platform_counts = 0;

void setupGun() {
  //prevent motor from running
  pinMode(PIN_FIRE, OUTPUT);
  digitalWrite(PIN_FIRE, LOW);
  
  tilt.attach(PIN_TILT);
  tilt.write(90);
  delay(500);
  tilt.detach();
}

void calibrateHorizontalAim() {
  platform.attach(PIN_PLATFORM);
  
  //wait until target appears
  do {
    RPi_getTargetCoords();
  } while(RPi_target_x == 0);
  Serial.println("begin cw");
  
  //turn clockwise
  for(int j = 0; j < 10; ++j) {
    RPi_getTargetCoords();
    platform.writeMicroseconds(1625);
    delay(100);
    platform.writeMicroseconds(1500);
  }
  
  Serial.println("begin ccw");
  //turn counter clockwise
  for(int j = 0; j < 10; ++j) {
    RPi_getTargetCoords();
    platform.writeMicroseconds(1375);
    delay(100);
    platform.writeMicroseconds(1500);
  }
  
  stall();
}

void calibrateVerticalAim() {
  tilt.attach(PIN_TILT, 950, 2050);
  
   //wait until target appears
   do {
    RPi_getTargetCoords();
  } while(RPi_target_x == 0);
  Serial.println("begin");
  
  for(int i = 0; i <= 9; ++i) {
    tilt.write(i*20);
    delay(2000);
    RPi_getTargetCoords();
  }
  tilt.detach();
  stall();
}

void setTiltAngle(int delta) {
  tilt_angle += delta;
  if(tilt_angle > 180) {
    Serial.println("tilt exceeds 180");
    tilt_angle = 180;
  }
  // don't go lower than lev
  else if(tilt_angle < 0) {
    Serial.println("tilt below 0");
    tilt_angle = 0;
  }
  tilt.write(tilt_angle);
}

//keeps track of rotations done on platform to be able to restore it
void rotatePlatform(int time) {
  if(time < 0)
    platform.writeMicroseconds(1625);  // QIAN: used to be 1625
  else
    platform.writeMicroseconds(1375);  // QIAN: used to be 1375
  delay(abs(time));
  platform.writeMicroseconds(1500);
  platform_counts += time;
  if(abs(platform_counts) > 8000) {
    //way off!
    restorePlatform();
  }
}

//turn platform back to original position
void restorePlatform() {
    rotatePlatform(-platform_counts);
}

void aimAndFire(int line) {
  int start_gun = -1;
  //calibrated at 5 ft
  Serial.println("aiming");
  tilt.attach(PIN_TILT);
  platform.attach(PIN_PLATFORM);
  
  tilt.write(120); //set appropriate tilt
  delay(500);

  //put platform back to 90 degrees with robot
  //restorePlatform();
  
  //keep track of tries to bail out if we somehow can't find target fast enough
  int tries = 0;
  int failed_tries = 0;
  RPi_getTargetCoords();
  // tries capped at 5
  while(tries < 5 && (RPi_target_x > CENTER_X + X_THRESH[line] || RPi_target_x < CENTER_X - X_THRESH[line] 
      || RPi_target_y > CENTER_Y + Y_THRESH[line] || RPi_target_y < CENTER_Y - Y_THRESH[line])) {
      if(RPi_target_x != 0 && RPi_target_y != 0) { //make sure coordinates are good
        
        //aim vertical
        int delta = -(int)(X_FACTOR*(RPi_target_x - CENTER_X));
        Serial.println(delta);
        setTiltAngle(delta);
        
        //aim horizontal
        int time = RPi_target_y - CENTER_Y;
        if(time > 0)
          time *= Y_FACTOR[0];
        else
          time *= Y_FACTOR[1];
        Serial.println(time);
        rotatePlatform(time);
      }
      else {
        ++failed_tries;
        if(failed_tries >= 6){
          rotatePlatform(1400);
          break;
        }
        //try rotating platform to the left a little bit
        if(failed_tries <= 2) {
          rotatePlatform(700);          // QIAN: changed these to rotate left first and by smaller amounts
        }
        else if(failed_tries == 3) {
          rotatePlatform(-1600);
        }
        else { //now just hope we find something...
          rotatePlatform(-800);
        }
      }
      RPi_getTargetCoords();
      if(start_gun == -1 && ((RPi_target_x < CENTER_X + X_GUN_THRESH && RPi_target_x > CENTER_X - X_GUN_THRESH) ||
         (RPi_target_y < CENTER_Y + Y_GUN_THRESH && RPi_target_y > CENTER_Y - Y_GUN_THRESH))) {
         //prepare fire
         digitalWrite(PIN_FIRE, HIGH);
         start_gun = millis();
      }
      ++tries;
  }
  tilt.detach();
  platform.detach();
  
  if(tries >= 8) {
    Serial.println("failed: too many tries");
  }
  if(failed_tries >= 3) {
    Serial.println("failed: too many failed tries");
  }
  Serial.println("aim done");
  
  
  //start firing
  if(start_gun == -1) {
    digitalWrite(PIN_FIRE, HIGH);
    start_gun = millis();
  }
  
  int waitTime = 3000 - (millis() - start_gun);
  if(waitTime > 0)
    delay(waitTime);

  trigger.attach(PIN_TRIGGER);
  
  //trigger forward to shoot
  trigger.writeMicroseconds(1000);
  delay(1000);
  
  //trigger back
  trigger.writeMicroseconds(2000);
  delay(500);
  
  // turn gun off
  digitalWrite(PIN_FIRE,LOW);
  
  // turn trigger forward and back a little
  trigger.writeMicroseconds(1800);
  delay(500);
  trigger.writeMicroseconds(2000);
  delay(500);
  trigger.writeMicroseconds(1800);
  delay(500);
  trigger.writeMicroseconds(2000);
  delay(500);
  
  trigger.detach();
  
  platform.attach(PIN_PLATFORM);
  restorePlatform();
  platform.detach();
}

