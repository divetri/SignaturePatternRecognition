#include <SD.h>
#define CS_PIN D8
String dataIn,px;
boolean sending = false;

void setup() {
  Serial.begin(9600);
  if (!SD.begin(CS_PIN)) {
    Serial.println("ccc");

    return;
  }
  dataIn = "";
}
void loop() {
  if (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    dataIn += inChar;
    if (inChar == '\n') {
      sending = true;
    }
  }

  if (sending) {
    sendingData();
    sending = false;
    dataIn = "";
  }
}
void sendingData() {
  px = dataIn;
  File testfile = SD.open("testdata.txt", FILE_WRITE);
  if (testfile) {
    testfile.print(px);
  }
}
