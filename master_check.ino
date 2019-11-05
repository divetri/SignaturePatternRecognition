void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

}

void loop() {
Serial.write('a');
delay(1000);
Serial.write('b');
delay(1000);
Serial.write('c');
delay(1000);
}
