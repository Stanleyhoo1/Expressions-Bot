#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void drawClosedEye(int x, int y) {
  tft.drawLine(x - 12, y, x + 12, y, ILI9341_BLACK);
}

void setup() {

  tft.begin();
  tft.setRotation(1);

  // full screen face
  tft.fillScreen(ILI9341_YELLOW);

  int cx = 160;
  int cy = 120;

  // sleeping eyes
  drawClosedEye(cx - 60, cy - 25);
  drawClosedEye(cx + 60, cy - 25);

  // relaxed sleeping mouth
  tft.fillCircle(cx, cy + 55, 10, ILI9341_BLACK);

  // ZZZ symbols
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextSize(3);
  tft.setCursor(cx + 70, cy - 90);
  tft.print("Z");

  tft.setTextSize(2);
  tft.setCursor(cx + 100, cy - 60);
  tft.print("Z");

  tft.setTextSize(1);
  tft.setCursor(cx + 115, cy - 40);
  tft.print("Z");
}

void loop() {
}