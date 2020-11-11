#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
int awal=0,waktu=0;
#define YP A3
#define XM A2
#define YM 9
#define XP 8
#define TS_MINX 120
#define TS_MAXX 900
#define TS_MINY 70
#define TS_MAXY 920
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define MINPRESSURE 1
#define MAXPRESSURE 1000
int px0, px1, py0, py1;
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#define boxsize 40
#define PENRADIUS 0.5
void setup(void) {
  Serial.begin(9600);
  tft.reset();
  uint16_t identifier = tft.readID();
  identifier=0x9341;
  tft.begin(identifier);
  tft.setRotation(2);
  tft.fillScreen(WHITE); 
  tft.fillRect(0, 0, 80, 40, RED);
  tft.fillRect(80, 0, 80, 40, BLACK);
  pinMode(13, OUTPUT);
}
void loop(){
  waktu++;
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE){
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    if (((p.y-PENRADIUS) > 40) && ((p.y+PENRADIUS) < tft.height())) {
      Serial.print(p.x);Serial.print(",");Serial.print(p.y);Serial.print("#");
      tft.fillCircle(p.x, p.y, PENRADIUS , BLACK);
      if (awal==0){ 
        px0 = p.x; py0 = p.y;
        waktu=0;
        awal=1;
      }
      else{
        px1 = px0; py1 = py0;
        px0 = p.x; py0 = p.y;
        tft.drawLine(px1, py1, px0, py0, BLACK);  
        waktu=0;     
      }
    }
    if (p.y < 40) {
       if (p.x < 40) {
         waktu=0;
         awal=0;
         digitalWrite(LCD_RESET, LOW);
         setup(); 
       }
    }
    
  }
  if(waktu>100){
        waktu=0;
        awal = 0;
      }
}
