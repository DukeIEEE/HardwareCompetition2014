
long photo_threshold = 0;

void setupPhotoSensor() {
  pinMode(PIN_PHOTO, INPUT);
  for(int i = 0; i < 100; ++i) {
    photo_threshold += analogRead(PIN_PHOTO);
    delay(10);
    Serial.println(analogRead(PIN_PHOTO));
  }
  photo_threshold /= 100;
  photo_threshold -= 20;
  Serial.print("Photo threshold: ");
  Serial.println(photo_threshold);
}

int averagePhotoReading() {
  int val = 0;
  for(int i = 0; i < 25; ++i) {
    val += analogRead(PIN_PHOTO);
    delay(1);
  }
  Serial.println(val/25);
  return val / 25;
}


//are the start leds on?
boolean StartLedOn() {
  int ledOnRead = 0;
  int readThres = 10;
  while (ledOnRead < readThres){
    if (averagePhotoReading() < photo_threshold)
      ++ledOnRead;
    else
      ledOnRead = 0;
    Serial.println(ledOnRead);
    delay(40);
  }
  return 1;
}
