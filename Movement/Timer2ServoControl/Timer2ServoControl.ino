void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);
  TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);
  OCR2A = 0;
  OCR2B = 1;
}
int i = 8;
void loop() {
  Serial.println(i);
  OCR2B = i;
  delay(2000);
  i++;
  if(i > 18)
    i = 8;
}
