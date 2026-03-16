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

  // Eyebrows
  tft.fillRect(cx - 85, cy - 75, 35, 6, ILI9341_BLACK);
  tft.fillRect(cx + 50, cy - 75, 35, 6, ILI9341_BLACK);

  // Eyes - big and open
  tft.fillCircle(cx - 65, cy - 30, 26, ILI9341_WHITE);
  tft.drawCircle(cx - 65, cy - 30, 26, ILI9341_BLACK);
  tft.fillCircle(cx - 65, cy - 30, 10, ILI9341_BLACK);

  tft.fillCircle(cx + 65, cy - 30, 26, ILI9341_WHITE);
  tft.drawCircle(cx + 65, cy - 30, 26, ILI9341_BLACK);
  tft.fillCircle(cx + 65, cy - 30, 10, ILI9341_BLACK);

  // Mouth - big shocked O
  tft.fillCircle(cx, cy + 55, 24, ILI9341_BLACK);
  tft.fillCircle(cx, cy + 55, 12, ILI9341_RED);
}

void loop() {
}