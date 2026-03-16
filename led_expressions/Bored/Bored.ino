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

  // full screen face
  tft.fillScreen(ILI9341_YELLOW);

  int cx = 160;
  int cy = 120;

  // eyes
  tft.fillCircle(cx - 60, cy - 30, 18, ILI9341_BLACK);
  tft.fillCircle(cx + 60, cy - 30, 18, ILI9341_BLACK);

  // neutral mouth
  tft.fillRect(cx - 45, cy + 45, 90, 10, ILI9341_BLACK);
}

void loop() {
}