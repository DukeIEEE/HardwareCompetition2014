void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println('running');
}

void loop() {
  Serial.println('begin loop');
  Serial.println('!ping');
  delay(10);
  Serial.println('!exe ls');
  Serial.println('!exe echo hi');
}
