#define CENTER_X 320
#define CENTER_Y 240

#define X_THRESH 20
#define Y_THRESH 20

Servo tilt, platform, trigger;
int tilt_angle = 1590;

void setupGun() {
  //prevent motor from running
  pinMode(PIN_FIRE, OUTPUT);
  digitalWrite(PIN_FIRE, LOW);
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
    for(int i = 0; i < 10; ++i) {
      platform.writeMicroseconds(1600);
      delay(50);
    }
    platform.writeMicroseconds(1500);
  }
  
  //reset
  for(int i = 0; i < 60; ++i) {
    platform.writeMicroseconds(1400);
    delay(50);
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


void setTiltAngle(int time) {
  float delta = (time - tilt_angle)/10.0;
  for(int i = 0; i < 10; ++i) {
    tilt.writeMicroseconds((int)(tilt_angle + i*delta));
    delay(50);
  }
  tilt_angle = time;
}

void aim() {
  //calibrated at 5 ft
  Serial.println("aiming");
  tilt.attach(PIN_TILT);
  platform.attach(PIN_PLATFORM);
  
  RPi_getTargetCoords();
  while(RPi_target_x > CENTER_X + X_THRESH || RPi_target_x < CENTER_X - X_THRESH 
      || RPi_target_y > CENTER_Y + Y_THRESH || RPi_target_y < CENTER_Y - Y_THRESH) {
      
      if(RPi_target_x != 0 && RPi_target_y != 0) { //make sure coordinates are good
        
        //aim vertical
        int target = tilt_angle - 2.78*(RPi_target_x - CENTER_X);
        if(target < 900 || target > 2000)
          target = 1590;
        setTiltAngle(target);
        
        //aim horizontal
        int iters = abs((int)(.15 * (RPi_target_y - CENTER_Y)))+1;
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
  
  tilt.detach();
  platform.detach();
  
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



