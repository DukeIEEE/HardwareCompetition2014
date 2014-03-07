#define CENTER_X 640
#define CENTER_Y 480

#define X_THRESH 20
#define Y_THRESH 20

Servo tilt, platform, trigger;

void setupGun() {
  //prevent motor from running
  pinMode(PIN_FIRE, OUTPUT);
  digitalWrite(PIN_FIRE, LOW);
}

void aimAndFire() {
  tilt.attach(PIN_TILT);
  platform.attach(PIN_PLATFORM);
  trigger.attach(PIN_TRIGGER);
  
  RPi_target_x = CENTER_X;
  RPi_target_y = CENTER_Y;
  
  //align in x direction
  do {
    //RPi_getTargetCoords();
    if(RPi_target_x <= CENTER_X)
      platform.writeMicroseconds(1400);
    else
      platform.writeMicroseconds(1600);
    delay(50);
  } while(RPi_target_x > CENTER_X + X_THRESH || RPi_target_x < CENTER_X - X_THRESH);  
  platform.writeMicroseconds(1500);

  //align in y direction
  do {
    //RPi_getTargetCoords();
    if(RPi_target_y <= CENTER_Y)
      tilt.writeMicroseconds(1400);
    else
      tilt.writeMicroseconds(1600);
    delay(50);
  } while(RPi_target_y > CENTER_Y + Y_THRESH || RPi_target_y < CENTER_Y - Y_THRESH);
  tilt.writeMicroseconds(1500);
  
  //fire gun
  //turn gun on
 /* digitalWrite(PIN_FIRE,HIGH);
  delay(1000);
  
  //trigger forward to shoot
  trigger.writeMicroseconds(1000);
  delay(800);
  
  //trigger back
  trigger.writeMicroseconds(1900);
  delay(500);
  
  // turn gun off
  digitalWrite(PIN_FIRE,LOW);
  */
  delay(1000);
  tilt.detach();
  platform.detach();
  trigger.detach();
}



