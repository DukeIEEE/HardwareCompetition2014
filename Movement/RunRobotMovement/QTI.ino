#include "QTI.h"
#include "Utilities.h"

void QTI::Setup() {
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}

long QTI::UpdateState() {
  time = analogRead(pin);
  return time;
}

bool QTI::isWhite(bool update) {
  if(update)
    UpdateState();
  return time < QTI_THRESHOLD;
}

bool QTI::isBlack(bool update) {
  return !QTI::isWhite(update);
}

int qti_last[4];
void qti_set_Reset() {
  for(int i = 0; i < 4; ++i) {
    qti_last[i] = 0;
  }
}

boolean qti_set_CheckAllWhite(QTI* qti) {
  const int sustain = 100;
  boolean works = true;
  for(int i = 0; i < 4; ++i) {
    if(qti[i].isWhite())
      qti_last[i] = sustain + 1;
    if(qti_last[i] == 0)
      works = false;
    --qti_last[i];
    if(qti_last[i] < 0)
      qti_last[i] = 0;
   //Serial.print(qti_last[i]);
   //Serial.print(' ');
  }
  //Serial.println();
  return works;
}
