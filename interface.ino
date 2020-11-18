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
#define ORANGE    0xFD20
#define DARKGREY  0x7BEF

//UI details
#define BUTTON_X 40
#define BUTTON_Y 100
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_SPACING_X 20
#define BUTTON_SPACING_Y 20
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
int px0, px1, py0, py1; //px0 and py0 for start point, px1 and py1 for end point
int currentPage; //page code for recent viewed page
#define PENRADIUS 0.5 //set line boldness
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//set button for inputing NIM
Elegoo_GFX_Button buttons[15];
char buttonlabels[15][5] = {"Pre", "", "Reg", "1", "2", "3", "4", "5", "6", "7", "8", "9", "Clr", "0", "Rst"};
uint16_t buttoncolors[15] = {ORANGE, BLACK, ORANGE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, DARKGREY, BLUE, RED};

void setup() {
  Serial.begin(9600);
  tft.reset();
  uint16_t identifier = tft.readID(); 
  identifier = 0x9341; //depends on your touch screen type
  tft.begin(identifier);
  tft.setRotation(2); //set screen orientation
  currentPage = 0; //'input NIM' page code
  drawNim(); //draw 'input NIM' page
}

void loop() {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //'input NIM' page loop command
  if (currentPage == 0) {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    }
    tft.setCursor(10, 65);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("Masukkan NIM");
    for (uint8_t b = 0; b < 15; b++) {
      if (buttons[b].contains(p.x, p.y)) {
        buttons[b].press(true);  // tell the button it is pressed
      } else {
        buttons[b].press(false);  // tell the button it is NOT pressed
      }
    }
    for (uint8_t b = 0; b < 15; b++) {
      if (buttons[b].justReleased()) {
        // Serial.print("Released: "); Serial.println(b);
        buttons[b].drawButton();  // draw normal
      }
      if (buttons[b].justPressed()) {
        buttons[b].drawButton(true);  // draw invert
        if (b >= 3 && b != 12 && b != 14) {
          if (textfield_i < TEXT_LEN) {
            textfield[textfield_i] = buttonlabels[b][0];
            textfield_i++;
            textfield[textfield_i] = 0; // zero terminate
          }
        }
        // clr button delete char
        if (b == 12) {
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
        if (b == 14) {
          textfield[textfield_i] = 0;
          while (textfield_i > 0) {
            textfield_i--;
            textfield[textfield_i] = ' ';
          }
          digitalWrite(LCD_RESET, LOW);
          setup();
        }
        //presensi or register button to drawing screen
        if (b == 0) {
          //Serial.print("pre#");
          nim = textfield;
          Serial.print(nim);
          Serial.print("#");
          currentPage = 1; //'signature paint' page code
          drawPaint();
        }
        if (b == 2) {
          //Serial.print("reg#");
          nim = textfield;
          Serial.print(nim);
          Serial.print("#");
          currentPage = 1; //'signature paint' page code
          drawPaint(); //draw 'signature paint' page
        }
        delay(100); //UI debouncing
      }
    }
  }

  //'signature paint' page loop command
  if (currentPage == 1) {
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
          currentPage = 1;
          drawPaint(); //reset paint field
          Serial.print(nim);
          Serial.print("#");
        }
	//'submit' button pressed
        else if (p.x > 120) {
          waktu = 0;
          awal = 0;
          Serial.print("sub\n"); //send submit command to slave
          delay(1000);
	  //reset NIM
          textfield[textfield_i] = 0;
          while (textfield_i > 0) {
            textfield_i--;
            textfield[textfield_i] = ' ';
          }
          digitalWrite(LCD_RESET, LOW); //reset screen
          setup();
        }
      }

    }
    //screen is not pressed in 100 ms
    if (waktu > 100) {
      waktu = 0; //reset delay count
      awal = 0; //set next point is new start point
    }
  }
}
void drawNim() {
  tft.fillScreen(BLACK);
  for (uint8_t row = 0; row < 5; row++) {
    for (uint8_t col = 0; col < 3; col++) {
      buttons[col + row * 3].initButton(&tft, BUTTON_X + col * (BUTTON_W + BUTTON_SPACING_X),
                                        BUTTON_Y + row * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                                        BUTTON_W, BUTTON_H, WHITE, buttoncolors[col + row * 3], WHITE,
                                        buttonlabels[col + row * 3], BUTTON_TEXTSIZE);
      if ((col + row * 3) != 1) {
        buttons[col + row * 3].drawButton();
      }
    }
  }
  // create 'text field'
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, WHITE);
}
void drawPaint() {
  tft.fillScreen(BLACK);
  tft.fillRect(2, 2, 116, 36, RED);
  tft.drawRect(2, 2, 116, 36, BLACK);
  tft.fillRect(122, 2, 116, 36, GREEN);
  tft.drawRect(122, 2, 116, 36, BLACK);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.setCursor(30, 13);
  tft.print("Reset");
  tft.setCursor(143, 13);
  tft.print("Submit");
}
