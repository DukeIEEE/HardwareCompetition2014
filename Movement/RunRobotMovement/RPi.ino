

boolean RPi_check() {
  RPi_buffer[0] = 0;
  Serial.println("!ping");
  Serial.setTimeout(1000);
  int res = Serial.readBytesUntil('\n', RPi_buffer, 20);
  if(res > 0)
    Serial.println(RPi_buffer);
  Serial.println("RPi_check done");
  if(strcmp(RPi_buffer, "Success") == 0)
    return true;
  else
    return false;
}

void RPi_getTargetCoords() {
  int tries = 0;
  RPi_target_x = RPi_target_y = -1;
  boolean success = true;
  while(RPi_target_x == -1 || RPi_target_y == -1) {
    Serial.println("!target_coords");
    delay(2000);
    RPi_target_x = Serial.parseInt();
    RPi_target_y = Serial.parseInt(); 
    ++tries;
    if(tries > 3)
      break;
  }
}

//use these to request the coordinates but read them out later
void RPi_requestCoords() {
  Serial.println("!target_coords");
}

void RPi_readCoords() {
  int tries = 0;
  RPi_target_x = RPi_target_y = -1;
  boolean success = true;
  while(RPi_target_x == -1 || RPi_target_y == -1) {
    delay(2000);
    RPi_target_x = Serial.parseInt();
    RPi_target_y = Serial.parseInt(); 
    ++tries;
    if(tries > 2)
      break;
    Serial.println("!target_coords");
  }
}

