#include "Utilities.h"

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

void waitForKeyboard() {
  while(!Serial.available())
    ;
  Serial.read();
}

void stall() {
  while(1) delay(10000);
}
