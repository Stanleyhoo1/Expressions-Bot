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

  // Full-screen face
  tft.fillScreen(ILI9341_YELLOW);

  int cx = 160;
  int cy = 120;

  // Left eyebrow (raised)
  tft.fillRect(cx - 90, cy - 75, 40, 6, ILI9341_BLACK);

  // Right eyebrow (lower)
  tft.fillRect(cx + 50, cy - 55, 40, 6, ILI9341_BLACK);

  // Eyes
  tft.fillCircle(cx - 60, cy - 25, 18, ILI9341_WHITE);
  tft.drawCircle(cx - 60, cy - 25, 18, ILI9341_BLACK);
  tft.fillCircle(cx - 60, cy - 25, 7, ILI9341_BLACK);

  tft.fillCircle(cx + 60, cy - 25, 18, ILI9341_WHITE);
  tft.drawCircle(cx + 60, cy - 25, 18, ILI9341_BLACK);
  tft.fillCircle(cx + 60, cy - 25, 7, ILI9341_BLACK);

  // Slight sideways mouth
  tft.drawLine(cx - 25, cy + 55, cx + 35, cy + 50, ILI9341_BLACK);
  tft.drawLine(cx - 23, cy + 56, cx + 37, cy + 51, ILI9341_BLACK);
}

void loop() {
}