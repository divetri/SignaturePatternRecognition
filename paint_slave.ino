#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <SD.h>
#define CS_PIN D8
const char *ssid = "[yourwifissidname]"; //SSID that connected to device
const char *password = "[yourwifipassword]";
String dataIn, px;
boolean sending = false;
HTTPClient http;
WiFiClient client;
void setup() {
  delay(1000);
  Serial.begin(9600);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  //SD checking
  if (!SD.begin(CS_PIN)) {
    Serial.println("SD Initialisation failed");
    return;
  }
  else {
    Serial.print("SD Initialisation ready!");
  }
  dataIn = "";
}
void loop() {
  //reading coordinate from master on serial monitor
  if (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    Serial.write(inChar);
    dataIn += inChar;
    if (inChar == '\n') {
      sending = true; //send dataIn every sending condition is true
    }
  }

  if (sending) {
    sendingData();
    sending = false;
    dataIn = ""; //clean dataIn
  }
}
void sendingData() {
  px = dataIn;
  char infile; //contain read file content per character
  //'submit' button is pressed to submit coordinates
  if (px == "sub\n") {
    http.begin("[yourserver]:[yourport]/presensi");      //specify request destination
    File testfile = SD.open("[yourtextdata]", FILE_READ); //read text data from SDcard
    int ukuran = testfile.size(); //for content-length purpose
    //Serial.println(ukuran);
    testfile.close(); //reset cursor on file read
    testfile = SD.open("testdata.txt", FILE_READ); //re-read text data from SDcard
    client.println("POST /presensi HTTP/1.1");
    client.println("Host: [yourserver]");
    client.println("Content-Type: plain/text");
    client.println("Content-Length: " + String(ukuran));
    client.println();
    while (testfile.available()) {
      infile = (char)testfile.read();
      client.print(infile);
      //Serial.print(infile);
    }
    testfile.close(); //reset cursor on file read
    delay(500);
    String payload = http.getString();
    Serial.println(payload);    //print request response payload
    http.end();  //Close connection
    //SD.remove("testdata.txt"); //cleanse text file for the next use
    //Serial.println("terkirim");
  }
  //'reset' button in paint page on master is pressed to reset coordinates
  else if (px == "res\n") {
    File testfile = SD.open("testdata.txt", FILE_WRITE);
    if (testfile) {
      SD.remove("testdata.txt"); //
      //Serial.print("reset\n");
    }
  }
  //saving coordinates to text file on SDcard
  else {
    File testfile = SD.open("testdata.txt", FILE_WRITE);
    if (testfile) {
      testfile.print(px);
      //Serial.print("sent\n");
    }
  }

}
