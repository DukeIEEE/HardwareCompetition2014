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

const int qti_sustain = 100;

boolean qti_set_CheckAllWhite(QTI* qti) {
  boolean works = true;
  for(int i = 0; i < 4; ++i) {
    if(qti[i].isWhite())
      qti_last[i] = qti_sustain + 1;
    if(qti_last[i] == 0)
      works = false;
    --qti_last[i];
    if(qti_last[i] < 0)
      qti_last[i] = 0;
  }
  return works;
}

boolean qti_set_Check3White(QTI* qti) {
  boolean works = true;
  for(int i = 1; i < 4; ++i) {
    if(qti[i].isWhite())
      qti_last[i] = qti_sustain + 1;
    if(qti_last[i] == 0)
      works = false;
    --qti_last[i];
    if(qti_last[i] < 0)
      qti_last[i] = 0;
  }
  return works;
}


