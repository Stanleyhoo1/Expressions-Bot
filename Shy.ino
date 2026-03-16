#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void drawClosedEye(int x, int y) {
  tft.drawLine(x - 12, y, x - 4, y - 6, ILI9341_BLACK);
  tft.drawLine(x - 4, y - 6, x + 4, y, ILI9341_BLACK);
  tft.drawLine(x + 4, y, x + 12, y - 6, ILI9341_BLACK);
}

void drawSmallMouth(int x, int y) {
  tft.drawLine(x - 10, y, x + 10, y, ILI9341_BLACK);
  tft.drawLine(x - 8, y + 1, x + 8, y + 1, ILI9341_BLACK);
}

void drawStressMark(int x, int y) {
  tft.drawLine(x, y, x + 6, y - 8, ILI9341_BLACK);
  tft.drawLine(x + 6, y - 8, x + 12, y, ILI9341_BLACK);
}

void setup() {
  tft.begin();
  tft.setRotation(1);

  // full-screen face
  tft.fillScreen(ILI9341_YELLOW);

  int cx = 160;
  int cy = 120;

  // blush cheeks
  tft.fillCircle(cx - 85, cy + 5, 20, ILI9341_RED);
  tft.fillCircle(cx + 85, cy + 5, 20, ILI9341_RED);

  // closed flustered eyes
  drawClosedEye(cx - 50, cy - 35);
  drawClosedEye(cx + 50, cy - 35);

  // little stress marks
  drawStressMark(cx - 95, cy - 55);
  drawStressMark(cx + 75, cy - 55);

  // tiny tense mouth
  drawSmallMouth(cx, cy + 45);
}

void loop() {
}