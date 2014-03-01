void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("running");
}

int x = 0;
void loop() {
  Serial.println("begin loop");
  Serial.print(x);
  Serial.println();
  Serial.print('!add ');
  Serial.println(x);
  delay(10);
  x = Serial.parseInt();
  delay(1000);
}
