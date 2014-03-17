

boolean RPi_check() {
  RPi_buffer[0] = 0;
  Serial.println("!ping");
  Serial.setTimeout(500);
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
  Serial.setTimeout(10000);
  while(RPi_target_x == -1 || RPi_target_y == -1) {
    digitalWrite(PIN_DEBUG_IMAGE, LOW);
    Serial.println("!target_coords");
    RPi_target_x = Serial.parseInt();
    RPi_target_y = Serial.parseInt(); 
    ++tries;
    if(tries > 3)
      break;
  }
  if(RPi_target_x == -1) {
    RPi_target_x = RPi_target_y = 0;
  }
  if(RPi_target_x != 0 && RPi_target_y != 0) {
    digitalWrite(PIN_DEBUG_IMAGE, HIGH);
  }
}

