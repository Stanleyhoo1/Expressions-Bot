#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {

  tft.begin();
  tft.setRotation(1);

  // Full screen face
  tft.fillScreen(ILI9341_YELLOW);

  int cx = 160;
  int cy = 120;

  // Angry eyebrows (slanted downward toward center)
  tft.drawLine(cx - 95, cy - 70, cx - 40, cy - 50, ILI9341_BLACK);
  tft.drawLine(cx + 95, cy - 70, cx + 40, cy - 50, ILI9341_BLACK);

  // Eyes
  tft.fillCircle(cx - 60, cy - 25, 18, ILI9341_WHITE);
  tft.drawCircle(cx - 60, cy - 25, 18, ILI9341_BLACK);
  tft.fillCircle(cx - 60, cy - 25, 10, ILI9341_BLACK);

  tft.fillCircle(cx + 60, cy - 25, 18, ILI9341_WHITE);
  tft.drawCircle(cx + 60, cy - 25, 18, ILI9341_BLACK);
  tft.fillCircle(cx + 60, cy - 25, 10, ILI9341_BLACK);

  // Angry frown
  tft.drawLine(cx - 45, cy + 55, cx + 45, cy + 55, ILI9341_BLACK);
  tft.drawLine(cx - 45, cy + 56, cx + 45, cy + 56, ILI9341_BLACK);
}

void loop() {
}