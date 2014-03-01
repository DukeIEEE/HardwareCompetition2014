#include "QTI.h"
#include "Utilities.h"

long QTI::UpdateState() {
  time = RCTime(pin);
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
