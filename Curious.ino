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

  // raised eyebrow (curious look)
  tft.fillRect(cx - 90, cy - 75, 40, 6, ILI9341_BLACK);

  // normal eyebrow
  tft.fillRect(cx + 50, cy - 60, 40, 6, ILI9341_BLACK);

  // eyes
  tft.fillCircle(cx - 60, cy - 30, 18, ILI9341_WHITE);
  tft.drawCircle(cx - 60, cy - 30, 18, ILI9341_BLACK);
  tft.fillCircle(cx - 60, cy - 30, 8, ILI9341_BLACK);

  tft.fillCircle(cx + 60, cy - 30, 18, ILI9341_WHITE);
  tft.drawCircle(cx + 60, cy - 30, 18, ILI9341_BLACK);
  tft.fillCircle(cx + 60, cy - 30, 8, ILI9341_BLACK);

  // curious tilted mouth
  tft.drawLine(cx - 30, cy + 50, cx + 30, cy + 40, ILI9341_BLACK);
  tft.drawLine(cx - 30, cy + 51, cx + 30, cy + 41, ILI9341_BLACK);
}

void loop() {
}