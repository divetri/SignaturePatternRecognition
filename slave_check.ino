char x;
void setup() {
Serial.begin(9600);

}

void loop() {
  if (Serial.available()){
  x=Serial.read();
  if (x=='a') 
  Serial.print(0);            
  else if (x=='b') 
  Serial.print(45);
  else if (x=='c') 
  Serial.print(90);
  
  }
}
