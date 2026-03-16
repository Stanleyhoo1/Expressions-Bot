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

  // annoyed eyebrow (slanted)
  tft.drawLine(cx - 90, cy - 65, cx - 40, cy - 75, ILI9341_BLACK);
  tft.drawLine(cx + 40, cy - 70, cx + 90, cy - 70, ILI9341_BLACK);

  // eye whites
  tft.fillCircle(cx - 60, cy - 30, 20, ILI9341_WHITE);
  tft.drawCircle(cx - 60, cy - 30, 20, ILI9341_BLACK);

  tft.fillCircle(cx + 60, cy - 30, 20, ILI9341_WHITE);
  tft.drawCircle(cx + 60, cy - 30, 20, ILI9341_BLACK);

  // side-looking pupils (both looking left)
  tft.fillCircle(cx - 70, cy - 30, 9, ILI9341_BLACK);
  tft.fillCircle(cx + 50, cy - 30, 9, ILI9341_BLACK);

  // eyelids (half closed)
  tft.fillRect(cx - 80, cy - 45, 40, 12, ILI9341_YELLOW);
  tft.fillRect(cx + 40, cy - 45, 40, 12, ILI9341_YELLOW);

  // flat annoyed mouth
  tft.fillRect(cx - 40, cy + 55, 80, 8, ILI9341_BLACK);
}

void loop() {
}