#include "QTI.h"
#include "Utilities.h"

long QTI::UpdateState() {
  digitalWrite(pin, HIGH); 
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
