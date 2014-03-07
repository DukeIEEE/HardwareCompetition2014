char RPi_buffer[20];
int RPi_target_x, RPi_target_y;
boolean RPi_check() {
  RPi_buffer[0] = 0;
  Serial.println("!ping");
  Serial.setTimeout(3000);
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
  boolean worked = false;
  while(!worked) {
    Serial.println("!target_coords");
    Serial.readBytesUntil('\n', RPi_buffer, 20);
  }
}


