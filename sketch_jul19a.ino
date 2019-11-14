

// Creates a bitmap (BMP) snapshot of the Adafruit 3.5" screen with HX8357D controller
// The BMP file has 2 bytes per pixel, RGB555 format
//
// The readPixel function was written by David Evans, June 2016
//
// The BMP writing function is a mashup of code from here:
// http://forum.arduino.cc/index.php?topic=177361.0
// http://forum.arduino.cc/index.php?topic=112733.0


/*  Used for testing:
 *   
 *  Magenta: 11111 000000 11111
 *  DkGreen: 00011 011111 00000
 *  Yellow:  11111 111111 00000
 *  Grey:    11011 111000 11100
 *  Red:     11111 000000 00000
 *  LtRed:   11111 110011 11001
 *  Black:   00000 000000 00000
 *  White:   11111 111111 11111
 *  
 *  const unsigned int GREY = 0xDF1C;
 *  const unsigned int LTRED = 0xFE79;
 *  const unsigned int DKGREEN = 0x1BE0;
 */

#include <SPI.h>
#include <SdFat.h>
#include <Elegoo_TFTLCD.h>
#include <pin_magic.h>
#include <registers.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define PIN_SD_CS 10 // Elegoo SD shields and modules: pin 10

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

const byte SD_CS = 4;

const int w = 480;     // image width in pixels
const int h = 320;     // height

char str[] = "TEST11.BMP";
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
const unsigned int GREY = 0xDF1C;
const unsigned int LTRED = 0xFE79;
const unsigned int DKGREEN = 0x1BE0;

SdFat SD;
File outFile;

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup()
{
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));

#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();
    uint16_t identifier = tft.readID();
   if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9341;
  
  }
  tft.begin(identifier);

  // print some objects on TFT to be captured in BMP
  tft.setRotation(3);
  tft.setTextSize(3);
  
  tft.fillScreen(BLUE);
  
  tft.drawPixel(13,3,WHITE);
  tft.drawPixel(14,3,WHITE);
  tft.drawPixel(15,3,WHITE);
  
  tft.drawLine(0,1,100,200,YELLOW);
  tft.drawLine(0,2,100,202,YELLOW);
  tft.drawLine(0,3,100,203,YELLOW);
  
  tft.fillRect(150, 150, 30, 30, GREY);
  tft.fillRect(30, 150, 30, 30, LTRED);
  tft.fillRect(150, 30, 30, 30, DKGREEN);
  
  tft.setCursor(50, 50);
  tft.print("hello world");
  // end test print to TFT
  
  Serial.begin(115200);
  Serial.println("starting");
  
  //init SD Card
  if (!SD.begin(SD_CS))
  {
    Serial.println("err strtng SD");
    while (1);    //If failed, stop here
  }

  Serial.println("working");
  GrabImage(str);
  Serial.println("done");
  
  tft.setCursor(100, 100);
  tft.print("Done");
}

void GrabImage(char* str)
{
  byte VH, VL;
  int i, j = 0;

  //Create the File
  outFile = SD.open(str, FILE_WRITE);
  if (! outFile) {
    Serial.println("err opng file");
    return;
  };

  unsigned char bmFlHdr[14] = {
    'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0
  };
  // 54 = std total "old" Windows BMP file header size = 14 + 40
  
  unsigned char bmInHdr[40] = {
    40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 16, 0
  };   
  // 40 = info header size
  //  1 = num of color planes
  // 16 = bits per pixel
  // all other header info = 0, including RI_RGB (no compr), DPI resolution

  unsigned long fileSize = 2ul * h * w + 54; // pix data + 54 byte hdr
  
  bmFlHdr[ 2] = (unsigned char)(fileSize      ); // all ints stored little-endian
  bmFlHdr[ 3] = (unsigned char)(fileSize >>  8); // i.e., LSB first
  bmFlHdr[ 4] = (unsigned char)(fileSize >> 16);
  bmFlHdr[ 5] = (unsigned char)(fileSize >> 24);

  bmInHdr[ 4] = (unsigned char)(       w      );
  bmInHdr[ 5] = (unsigned char)(       w >>  8);
  bmInHdr[ 6] = (unsigned char)(       w >> 16);
  bmInHdr[ 7] = (unsigned char)(       w >> 24);
  bmInHdr[ 8] = (unsigned char)(       h      );
  bmInHdr[ 9] = (unsigned char)(       h >>  8);
  bmInHdr[10] = (unsigned char)(       h >> 16);
  bmInHdr[11] = (unsigned char)(       h >> 24);

  outFile.write(bmFlHdr, sizeof(bmFlHdr));
  outFile.write(bmInHdr, sizeof(bmInHdr));

  for (i = h; i > 0; i--) {
    for (j = 0; j < w; j++) {

      uint16_t rgb = readPixA(j,i); // get pix color in rgb565 format
      
      VH = (rgb & 0xFF00) >> 8; // High Byte
      VL = rgb & 0x00FF;        // Low Byte
      
      //RGB565 to RGB555 conversion... 555 is default for uncompressed BMP
      //this conversion is from ...topic=177361.0 and has not been verified
      VL = (VH << 7) | ((VL & 0xC0) >> 1) | (VL & 0x1f);
      VH = VH >> 1;
      
      //Write image data to file, low byte first
      outFile.write(VL);
      outFile.write(VH);
    }
  }
  //Close the file
  outFile.close();
}

void loop()
{
}

uint16_t readPixA(int x, int y) { // get pixel color code in rgb565 format

  tft.setAddrWindow(x,y,x,y);

  digitalWrite(LCD_DC, LOW);
  digitalWrite(LCD_CLK, LOW);
  digitalWrite(LCD_CS, LOW);
  tft.spiwrite(0x2E); // memory read command

  digitalWrite(LCD_DC, HIGH);

  uint16_t r = 0;
  r = tft.spiread(); // discard dummy read
  r = tft.spiread() >> 3; // red: use 5 highest bits (discard three LSB)
  r = (r << 6) | tft.spiread() >> 2; // green: use 6 highest bits (discard two LSB)
  r = (r << 5) | tft.spiread() >> 3; // blue: use 5 highest bits (discard three LSB)

  digitalWrite(LCD_CS, HIGH);

  return r;
}
