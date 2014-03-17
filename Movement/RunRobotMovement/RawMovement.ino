void lineFollow(){
  bool off[4];
  for(int i = 0; i < 4; ++i) {
    off[i] = qti[i].isBlack();
  }
  if(off[0] && off[1] && off[2] && off[3]) {
    left_servos.write(CENTER + DELTA);
    right_servos.write(CENTER - DELTA);
  }
  else if(off[0] && off[1] && !off[2] && off[3]) {
    left_servos.write(CENTER + DELTA);
    right_servos.write(CENTER - DELTA/8);
  }
  else if(off[0] && !off[1] && off[2] && off[3]) {
    left_servos.write(CENTER + DELTA/8);
    right_servos.write(CENTER - DELTA);
  }
  else if(off[0] && off[1] && off[2] && !off[3]) {
    left_servos.write(CENTER + DELTA);
    right_servos.write(CENTER - DELTA/16);
  }
  else if(!off[0] && off[1] && off[2] && off[3]) {
    left_servos.write(CENTER + DELTA/16);
    right_servos.write(CENTER - DELTA);
  }
  else {
      left_servos.write(CENTER + DELTA);
      right_servos.write(CENTER - DELTA);
  }
}

void lineFollow(int time) {
  long start_time = millis();
  long stop_time = start_time + time;
  while(millis() < stop_time) {
    lineFollow();
    delay(1);
  }
}

void lineFollowReverse(int time) {
  long start = millis();
  while(millis() - start < time) {
    lineFollowReverse();
    delay(1);
  }
}

void lineFollowReverse(){
  bool off[4];
  for(int i = 0; i < 4; ++i) {
    off[i] = qti_back[i].isBlack();
  }
  if(off[0] && off[1] && off[2] && off[3]) {
    left_servos.write(CENTER - DELTA);
    right_servos.write(CENTER + DELTA);
  }
  else if(off[0] && off[1] && !off[2] && off[3]) {
    left_servos.write(CENTER - DELTA/8);
    right_servos.write(CENTER + DELTA);
  }
  else if(off[0] && !off[1] && off[2] && off[3]) {
    left_servos.write(CENTER - DELTA);
    right_servos.write(CENTER + DELTA/8);
  }
    else if(off[0] && off[1] && off[2] && !off[3]) {
    left_servos.write(CENTER - DELTA/16);
    right_servos.write(CENTER + DELTA);
  }
  else if(!off[0] && off[1] && off[2] && off[3]) {
    left_servos.write(CENTER - DELTA);
    right_servos.write(CENTER + DELTA/16);
  }
  else {
    left_servos.write(CENTER - DELTA);
    right_servos.write(CENTER + DELTA);
  }
}

void forward(int time) {
  left_servos.write(CENTER + DELTA);
  right_servos.write(CENTER - DELTA);
  delay(time);
}

void backward(int time) {
  left_servos.write(CENTER - DELTA);
  right_servos.write(CENTER + DELTA);
  delay(time);
}

void stopMotor() {
  left_servos.write(90);
  right_servos.write(90);
}

void rotateLeft2() {
  left_servos.write(78);
  right_servos.write(70);
  delay(1000);
  while(qti_back[2].isBlack()) {}
  stopMotor();
}

void rotateRight2() {
  left_servos.write(95);
  right_servos.write(110);
  delay(1000);
  while(qti_back[1].isBlack()) {}
  //forward(500);
  lineFollowReverse(14*MS_PER_CM);
  stopMotor();
}

void jerkRight() {
  left_servos.write(CENTER + DELTA);
  right_servos.write(CENTER);
  delay(350);
}

