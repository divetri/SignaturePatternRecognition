#include <Elegoo_GFX.h>    //core graphics library
#include <Elegoo_TFTLCD.h> //hardware-specific library
#include <TouchScreen.h>

//declare port
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//declare color code
#define BLACK     0x0000
#define BLUE      0x001F
#define RED       0xF800
#define GREEN     0x03E0
#define WHITE     0xFFFF
#define DARKGREY  0x7BEF

//UI details
#define BUTTON_X 40
#define BUTTON_Y 110
#define BUTTON_W 70
#define BUTTON_H 35
#define BUTTON_SPACING_X 10
#define BUTTON_SPACING_Y 10
#define BUTTON_TEXTSIZE 2

//text box where numbers go
#define TEXT_X 10
#define TEXT_Y 10
#define TEXT_W 220
#define TEXT_H 50
#define TEXT_TSIZE 3
#define TEXT_TCOLOR WHITE
#define TEXT_LEN 9
char textfield[TEXT_LEN + 1] = "";
uint8_t textfield_i = 0;

//touch for new ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900
#define TS_MINY 70
#define TS_MAXY 920
#define MINPRESSURE 1
#define MAXPRESSURE 1000

int awal = 0, waktu = 0; //set points to be lines
String nim; //set userID
String stat; // set session status
String entryInfo;
int px0, px1, py0, py1; //px0 and py0 for start point, px1 and py1 for end point
int currentPage; //page code for recent viewed page
#define PENRADIUS 0.5 //set line boldness
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//set button for inputing NIM
Elegoo_GFX_Button buttons[12];
char buttonlabels[12][6] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "Clear", "0", "Reset"};
uint16_t buttoncolors[12] = {BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, DARKGREY, BLUE, RED};

void setup() {
  Serial.begin(9600);
  tft.reset();
  uint16_t identifier = tft.readID(); 
  identifier = 0x9341; //depends on your touch screen type
  tft.begin(identifier);
  tft.setRotation(2); //set screen orientation
  currentPage = 0; //'input NIM' page code
  drawStatus(); //draw 'input NIM' page
}

void loop() {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (currentPage == 0) {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    }
    if (p.x > TEXT_X && p.x < (TEXT_X + TEXT_W)){
      if (p.y > 150 && p.y < 185){
        stat = "reg#";
        Serial.print(stat);
        delay(100);
        currentPage=1;
        drawNim();
      }
      if (p.y > 200 && p.y < 235){
        stat = "pre#";
        Serial.print(stat);
        delay(100);
        currentPage=1;
        drawNim();
      }
    }
  }
  //'input NIM' page loop command
  if (currentPage == 1) {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    }
    if (p.x > TEXT_X && p.x < (TEXT_X + TEXT_W)){
      if (p.y > 275 && p.y < 310){
        if(textfield_i==9){
          nim = textfield;
          Serial.print(nim+"#");
          currentPage = 2; //'signature paint' page code
          drawPaint(); //draw 'signature paint' page
        }
        if(textfield_i!=9){
          textfield_i = 0;
          textfield[textfield_i] = ' ';
          Serial.println("res");
          drawError();
          delay(1000);
          digitalWrite(LCD_RESET, LOW); //reset screen
          setup();
        }
      }
    }
    for (uint8_t b = 0; b < 12; b++) {
      if (buttons[b].contains(p.x, p.y)) {
        buttons[b].press(true);  // tell the button it is pressed
      } else {
        buttons[b].press(false);  // tell the button it is NOT pressed
      }
    }
    for (uint8_t b = 0; b < 12; b++) {
      if (buttons[b].justReleased()) {
        // Serial.print("Released: "); Serial.println(b);
        buttons[b].drawButton();  // draw normal
      }
      if (buttons[b].justPressed()) {
        buttons[b].drawButton(true);  // draw invert
        if (b < 9 || b==10) {
          if (textfield_i < TEXT_LEN) {
            textfield[textfield_i] = buttonlabels[b][0];
            textfield_i++;
            textfield[textfield_i] = 0; // zero terminate
          }
        }
        // clr button delete char
        if (b == 9) {
          textfield[textfield_i] = 0;
          if (textfield > 0) {
            textfield_i--;
            textfield[textfield_i] = ' ';
          }
        }
        // update the current text field
        tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
        tft.setTextColor(TEXT_TCOLOR, BLACK);
        tft.setTextSize(TEXT_TSIZE);
        tft.print(textfield);
        //reset screen
        if (b == 11) {
          textfield[textfield_i] = 0;
          while (textfield_i > 0) {
            textfield_i--;
            textfield[textfield_i] = ' ';
          }
          Serial.println("res");
          Serial.print(stat);
          currentPage=1;
          drawNim();
        }
        delay(100); //UI debouncing
      }
    }
  }

  //'signature paint' page loop command
  if (currentPage == 2) {
    waktu++; //counting touch screen delayed
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
      if (((p.y - PENRADIUS) > 40) && ((p.y + PENRADIUS) < tft.height())) {
        Serial.print(awal); Serial.print(","); Serial.print(p.x); Serial.print(","); Serial.print(p.y); Serial.print("#");
        tft.fillCircle(p.x, p.y, PENRADIUS , WHITE);
	//first point of line
        if (awal == 0) {
          px0 = p.x; py0 = p.y; //set start point for making line
          waktu = 0; //reset delay count
          awal = 1; //set start point status
        }
	//next point of line
        else {
          px1 = px0; py1 = py0; 
          px0 = p.x; py0 = p.y;
          tft.drawLine(px1, py1, px0, py0, WHITE);
          waktu = 0; //reset delay count
        }
      }
      //button pressed
      if (p.y < 40) {
	//'reset' button pressed
        if (p.x < 120) {
          waktu = 0;
          awal = 0;
          Serial.print("res\n"); //send reset command to slave
          delay(1000);
          currentPage = 2;
          drawPaint(); //reset paint field
          Serial.print(stat);
          Serial.print(nim);
          Serial.print("#");
        }
	//'submit' button pressed
        else if (p.x > 120) {
          waktu = 0;
          awal = 0;
          textfield_i = 0;
          textfield[textfield_i] = ' ';
          Serial.print("sub\n"); //send submit command to slave
          delay(1000);
          drawEntry();
          currentPage=3;
        }
      }

    }
    //screen is not pressed in 100 ms
    if (waktu > 50) {
      waktu = 0; //reset delay count
      awal = 0; //set next point is new start point
    }
  }
  if (currentPage == 3) {
    if(stat=="reg#"){
      tft.setCursor(45, 155);
      tft.print("Menyimpan...");
      while (!Serial.available());
      tft.fillScreen(BLACK);
      String var1 = Serial.readStringUntil(','); // writes in the string all the inputs till a comma
      Serial.read(); 
      String var2 = Serial.readStringUntil('\n');
      tft.setCursor(65, 100);
      tft.setTextColor(GREEN);
      tft.print("Tersimpan");
      if (var2.toInt()<10){
        tft.setTextColor(WHITE);
        tft.setCursor(15, 140);
        tft.print("Silakan registrasi");
        tft.setCursor(60, 180);
        tft.print(10-var2.toInt());
        tft.print(" kali lagi");
      }
      else{
        tft.setTextColor(WHITE);
        tft.setCursor(40, 140);
        tft.print("Regitrasi Anda");
        tft.setCursor(45, 160);
        tft.print("sudah lengkap");
        tft.setCursor(40, 200);
        tft.setTextColor(GREEN);
        tft.print("Selamat Datang");
        tft.setCursor(70, 220);
        tft.print(nim);
      }
    }
    if(stat=="pre#"){
      tft.setCursor(5, 155);
      tft.print("Mengidentifikasi...");
      while (!Serial.available());
      tft.fillScreen(BLACK);
      String var1 = Serial.readStringUntil(','); // writes in the string all the inputs till a comma
      Serial.read(); 
      String var2 = Serial.readStringUntil('\n');
      tft.setCursor(15, 60);
      tft.print("Hasil Identifikasi");
      tft.setCursor(25, 100);
      tft.print("Threshold = ");
      tft.print(var1);
      tft.setCursor(25, 140);
      tft.print("Nilai TTD = ");
      tft.print(var2);
      if(var2.toFloat()>=var1.toFloat()){
        tft.fillRect(0, 180, 240, 35, GREEN);
        tft.setCursor(95, 190);
        tft.print("COCOK");
        tft.setCursor(40, 230);
        tft.setTextColor(GREEN);
        tft.print("Selamat Datang");
        tft.setCursor(70, 260);
        tft.print(nim);
      }
      else{
        tft.fillRect(0, 180, 240, 35, RED);
        tft.setCursor(55, 190);
        tft.print("TIDAK COCOK");
        tft.setCursor(40, 230);
        tft.print("Silakan ulangi");
      }
    }
    delay(8000);
    digitalWrite(LCD_RESET, LOW); //reset screen
    setup();
  }
}


void drawStatus(){
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setCursor(40, 80);
  tft.setTextColor(WHITE);
  tft.print("Selamat Datang");
  tft.setCursor(20, 110);
  tft.print("Aplikasi Presensi");
  tft.drawRoundRect(TEXT_X-1, 149, TEXT_W, 37, 8, WHITE);
  tft.fillRoundRect(TEXT_X, 150, TEXT_W-2, 35, 8, GREEN);
  tft.drawRoundRect(TEXT_X-1, 199, TEXT_W, 37, 8, WHITE);
  tft.fillRoundRect(TEXT_X, 200, TEXT_W-2, 35, 8, GREEN);
  tft.setCursor(65, 160);
  tft.print("Registrasi");
  tft.setCursor(75, 210);
  tft.print("Presensi");
}
void drawNim() {
  tft.fillScreen(BLACK);
  tft.setCursor(10, 65);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Masukkan NIM");
  for (uint8_t row = 0; row < 4; row++) {
    for (uint8_t col = 0; col < 3; col++) {
      buttons[col + row * 3].initButton(&tft, BUTTON_X + col * (BUTTON_W + BUTTON_SPACING_X),
                                        BUTTON_Y + row * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                                        BUTTON_W, BUTTON_H, WHITE, buttoncolors[col + row * 3], WHITE,
                                        buttonlabels[col + row * 3], BUTTON_TEXTSIZE);
      buttons[col + row * 3].drawButton();
    }
  }
  // create 'text field'
  tft.drawRoundRect(TEXT_X-1, 274, TEXT_W, 37, 8, WHITE);
  tft.fillRoundRect(TEXT_X, 275, TEXT_W-2, 35, 8, GREEN);
  tft.setCursor(95, 285);
  tft.print("Next");
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, WHITE);
}
void drawPaint() {
  tft.fillScreen(BLACK);
  tft.drawRect(2, 41, 237, 278, WHITE);
  tft.fillRoundRect(3, 3, 114, 34, 8, RED);
  tft.drawRoundRect(2, 2, 116, 36, 8, WHITE);
  tft.fillRoundRect(122, 2, 116, 36, 8, GREEN);
  tft.drawRoundRect(122, 2, 116, 36, 8, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(30, 13);
  tft.print("Reset");
  tft.setCursor(143, 13);
  tft.print("Submit");
}
void drawError(){
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 155);
  tft.setTextColor(WHITE);
  tft.print("Invalid NIM");
}
void drawEntry(){
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
}
