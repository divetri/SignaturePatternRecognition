#include <SD.h>
#define CS_PIN D8
String dataIn;
String dt[2];
int i, px, py;
boolean parsing = false;

void setup() {
  Serial.begin(9600);
  if (!SD.begin(CS_PIN)) {
    Serial.println("Falha, verifique se o cartão está presente.");
    //programa encerrrado
    return;
  }
  dataIn = "";
}
void loop() {
  if (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    dataIn += inChar;
    if (inChar == '\n') {
      parsing = true;
    }
  }

  if (parsing) {
    parsingData();
    parsing = false;
    dataIn = "";
  }
}
void parsingData() {
  int j = 0;
  dt[j] = "";
  for (i = 0; i < dataIn.length(); i++) {
    if (dataIn[i] == ',')
    {
      j++;
      dt[j] = "";
    }
    else
    {
      dt[j] = dt[j] + dataIn[i];
    }
  }
  px = dt[0].toInt();
  py = dt[1].toInt();
  File testfile = SD.open("testdata.txt", FILE_WRITE);
  if (testfile) {
    testfile.print("x : ");
    testfile.print(px);
    testfile.print("\n");
    testfile.print("y : ");
    testfile.print(py);
    testfile.print("\n");

  }
}
