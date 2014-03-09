#define CENTER_X 335
#define CENTER_Y 240

#define X_THRESH 10
#define Y_THRESH 10

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

void aimHorizontal() {
  platform.attach(PIN_PLATFORM);
  
  //align in x direction
  RPi_getTargetCoords();
  while(RPi_target_y > CENTER_Y + Y_THRESH || RPi_target_y < CENTER_Y - Y_THRESH) {   
    Serial.print(RPi_target_x);
    Serial.print(",");
    Serial.println(RPi_target_y);
    
    if(RPi_target_x != 0 && RPi_target_y != 0) {
      int iters = abs((int)(.15 * (RPi_target_y - CENTER_Y)))+1;
      Serial.print("iters ");
      Serial.println(iters);
      for(int i = 0; i < iters; ++i) {
        if(RPi_target_y >= CENTER_Y)
          platform.writeMicroseconds(1400);
        else
          platform.writeMicroseconds(1600);
        delay(50);
      }
      platform.writeMicroseconds(1500);
    } 
    RPi_getTargetCoords();
  }
  
  platform.detach();
}

void calibrateHorizontalAim() {
  platform.attach(PIN_PLATFORM);
  
  //wait until target appears
  do {
    RPi_getTargetCoords();
  } while(RPi_target_x == 0);
  Serial.println("begin");
  
  //turn clockwise
  for(int j = 0; j < 5; ++j) {
    RPi_getTargetCoords();
    for(int i = 0; i < 30; ++i) {
      platform.writeMicroseconds(1600);
      delay(20);
    }
    platform.writeMicroseconds(1500);
  }
  
  //reset
  for(int i = 0; i < 50; ++i) {
    platform.writeMicroseconds(1380);
    delay(20);
  }
  platform.writeMicroseconds(1500);
  
  stall();
}

void calibrateVerticalAim() {
  tilt.attach(PIN_TILT);

  //wait until target appears
   do {
    RPi_getTargetCoords();
  } while(RPi_target_x == 0);
  Serial.println("begin");

  //tilt upwards from flat
  for(int j = 0; j <= 8; ++j) {
    for(int i = 0; i < 50; ++i) {
      tilt.writeMicroseconds(1000 + j*100);
      delay(50);
    }
    RPi_getTargetCoords();
  }
  
  //reset
  while(1) {
    tilt.writeMicroseconds(1000);
    delay(50);  
  }
}

void calibrateVerticalAim2() {
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
  /*for(int i = 0; i < 50; ++i) {
    tilt.writeMicroseconds(1500);
    delay(20);
  }*/
  tilt.detach();
}

void aimVertical() {
  tilt.attach(PIN_TILT);
  
  setTiltAngle(1590);
  
    //align in x direction
   RPi_getTargetCoords();
   while(RPi_target_x > CENTER_X + X_THRESH || RPi_target_x < CENTER_X - X_THRESH) {   
    Serial.print(RPi_target_x);
    Serial.print(",");
    Serial.println(RPi_target_y);
    
    if(RPi_target_x != 0 && RPi_target_y != 0) {
      int target = tilt_angle - 2.78*(RPi_target_x - CENTER_X);
      Serial.println(target);
      if(target < 900 || target > 2000)
        target = 1590;
      setTiltAngle(target);
    } 
    RPi_getTargetCoords();
  }
  
  tilt.detach();
}


void setTiltAngle(int delta) {
  /*float delta = (time - tilt_angle)/20.0;
  for(int i = 0; i < 20; ++i) {
    tilt.writeMicroseconds(tilt_angle);
    //tilt.writeMicroseconds((int)(tilt_angle + i*delta));
    delay(20);
  }
  tilt_angle = time;*/
  tilt_angle += delta;
  if(tilt_angle > 180) {
    Serial.println("tilt exceeds 180");
    tilt_angle = 180;
  }
  else if(tilt_angle < 0) {
    Serial.println("tilt below 0");
    tilt_angle = 0;
  }
  
  tilt.write(tilt_angle);
}

//keeps track of rotations done on platform to be able to restore it
void rotatePlatform(int iters) {
  int a_iters = abs(iters);
  if(a_iters < 3)
    a_iters = 3;
  for(int i = 0; i < a_iters; ++i) {
    if(iters > 0)
      platform.writeMicroseconds(1400);
    else
      platform.writeMicroseconds(1600);
    delay(50);
  }
  platform.writeMicroseconds(1500);
  platform_counts += iters;
}

//turn platform back to original position
void restorePlatform() {
    rotatePlatform(-platform_counts);
}

void aim() {
  //calibrated at 5 ft
  Serial.println("aiming");
  tilt.attach(PIN_TILT);
  platform.attach(PIN_PLATFORM);
  
  //put platform back to 90 degrees with robot
  //restorePlatform();
  
  //keep track of tries to bail out if we somehow can't find target fast enough
  int tries = 0;
  int failed_tries = 0;
  RPi_getTargetCoords();
  while(tries < 8 && failed_tries < 3 && (RPi_target_x > CENTER_X + X_THRESH || RPi_target_x < CENTER_X - X_THRESH 
      || RPi_target_y > CENTER_Y + Y_THRESH || RPi_target_y < CENTER_Y - Y_THRESH)) {
      if(RPi_target_x != 0 && RPi_target_y != 0) { //make sure coordinates are good
        
        //aim vertical
        //int target = tilt_angle - 2.78*(RPi_target_x - CENTER_X);
        //if(target < 900 || target > 2000)
        //  target = 1590;
        //setTiltAngle(target);
        int delta = -(int)(.446*(RPi_target_x - CENTER_X));
        Serial.println(delta);
        setTiltAngle(delta);
        
        //aim horizontal
        int iters = (int)(.15 * (RPi_target_y - CENTER_Y));
        Serial.println(iters);
        rotatePlatform(iters);
        /*for(int i = 0; i < iters; ++i) {
          if(RPi_target_y >= CENTER_Y)
            platform.writeMicroseconds(1400);
          else
            platform.writeMicroseconds(1600);
          delay(50);
        }
        platform.writeMicroseconds(1500);*/
      }
      else {
        ++failed_tries;
        
        //try rotating platform to the left a little bit
        if(failed_tries == 1) {
          rotatePlatform(14);
        }
        else { //now just hope we find something...
          rotatePlatform(-14);
        }
      }
      RPi_getTargetCoords();
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
}

void fire() {
  trigger.attach(PIN_TRIGGER);
  //turn gun on
  digitalWrite(PIN_FIRE,HIGH);
  delay(3000);
  
  //trigger forward to shoot
  trigger.writeMicroseconds(1000);
  delay(800);
  
  //trigger back
  trigger.writeMicroseconds(1900);
  delay(500);
  
  // turn gun off
  digitalWrite(PIN_FIRE,LOW);
  trigger.detach();
}

void aimAndFire() {
  tilt.attach(PIN_TILT);
  platform.attach(PIN_PLATFORM);
  trigger.attach(PIN_TRIGGER);
  
  RPi_target_x = CENTER_X;
  RPi_target_y = CENTER_Y;
  
  //align in x direction
  do {
    RPi_getTargetCoords();
    if(RPi_target_x <= CENTER_X)
      platform.writeMicroseconds(1400);
    else
      platform.writeMicroseconds(1600);
    delay(50);
    platform.writeMicroseconds(1500);
  } while(RPi_target_x > CENTER_X + X_THRESH || RPi_target_x < CENTER_X - X_THRESH);  
  

  //align in y direction
  do {
    RPi_getTargetCoords();
    if(RPi_target_y <= CENTER_Y)
      tilt.writeMicroseconds(1400);
    else
      tilt.writeMicroseconds(1600);
    delay(50);
  } while(RPi_target_y > CENTER_Y + Y_THRESH || RPi_target_y < CENTER_Y - Y_THRESH);
  tilt.writeMicroseconds(1500);
  
  //fire gun
  //turn gun on
  digitalWrite(PIN_FIRE,HIGH);
  delay(1000);
  
  //trigger forward to shoot
  trigger.writeMicroseconds(1000);
  delay(800);
  
  //trigger back
  trigger.writeMicroseconds(1900);
  delay(500);
  
  // turn gun off
  digitalWrite(PIN_FIRE,LOW);
  
  delay(1000);
  tilt.detach();
  platform.detach();
  trigger.detach();
}



