
String x;
String y;



void setup() {
Serial.begin(9600);

}

void loop() {
if (Serial.available()){
  x=Serial.read();
  y=Serial.read();
  Serial.println(x);
  Serial.println(y);


  
  }

}
