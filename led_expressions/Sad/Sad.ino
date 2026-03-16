#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <math.h>

#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// draw frown curve
void drawFrown(int x, int y, int r) {
  for (int angle = 200; angle <= 340; angle += 2) {
    float rad = angle * DEG_TO_RAD;
    int px = x + r * cos(rad);
    int py = y + r * sin(rad);
    tft.fillCircle(px, py, 4, ILI9341_BLACK);
  }
}

void setup() {

  tft.begin();
  tft.setRotation(1);

  // FULL SCREEN FACE
  tft.fillScreen(ILI9341_YELLOW);

  int cx = 160;
  int cy = 120;

  // Eyes
  tft.fillCircle(cx - 60, cy - 40, 20, ILI9341_BLACK);
  tft.fillCircle(cx + 60, cy - 40, 20, ILI9341_BLACK);

  // Large frown
  drawFrown(cx, cy + 100, 90);
}

void loop() {
}