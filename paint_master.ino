// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO
//Technical support:goodtft@163.com

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>




#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif
int awal=0,waktu=0;


#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//#define TS_MINX 50
//#define TS_MAXX 920

//#define TS_MINY 100
//#define TS_MAXY 940

//Touch For New ILI9341 TP
#define TS_MINX 125
#define TS_MAXX 900

#define TS_MINY 90
#define TS_MAXY 900

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define PENRADIUS 0.5

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Paint!"));
  
  tft.reset();
  
  uint16_t identifier = tft.readID();
  identifier=0x9341;

  tft.begin(identifier);
  tft.setRotation(2);

  tft.fillScreen(WHITE); 
  tft.fillRect(0, 0, 40, 40, RED);
  pinMode(13, OUTPUT);

}

#define MINPRESSURE 1
#define MAXPRESSURE 1000
int px0, px1, py0, py1;

void loop()
{
  waktu++;
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    //p.x = tft.width()-map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     //p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
      tft.fillCircle(p.x, p.y, PENRADIUS, BLACK);
      Serial.write(p.x, p.y);
    if (awal==0){ 
      px0 = p.x; py0 = p.y;
      awal=1;
    }
   else {
      px1 = px0; py1 = py0;
      px0 = p.x; py0 = p.y;
      tft.drawLine(px1, py1, px0, py0, BLACK);
           
   }
  if (waktu>100){
    waktu=0;
    awal = 0;
    }
  
}
}
