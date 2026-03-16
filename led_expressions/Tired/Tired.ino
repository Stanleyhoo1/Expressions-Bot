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

  // tired eyebrows
  tft.drawLine(cx - 90, cy - 60, cx - 40, cy - 65, ILI9341_BLACK);
  tft.drawLine(cx + 40, cy - 65, cx + 90, cy - 60, ILI9341_BLACK);

  // eye whites
  tft.fillCircle(cx - 60, cy - 25, 18, ILI9341_WHITE);
  tft.drawCircle(cx - 60, cy - 25, 18, ILI9341_BLACK);

  tft.fillCircle(cx + 60, cy - 25, 18, ILI9341_WHITE);
  tft.drawCircle(cx + 60, cy - 25, 18, ILI9341_BLACK);

  // pupils (lowered slightly)
  tft.fillCircle(cx - 60, cy - 20, 7, ILI9341_BLACK);
  tft.fillCircle(cx + 60, cy - 20, 7, ILI9341_BLACK);

  // heavy eyelids
  tft.fillRect(cx - 78, cy - 42, 36, 15, ILI9341_YELLOW);
  tft.fillRect(cx + 42, cy - 42, 36, 15, ILI9341_YELLOW);

  // tired mouth
  tft.fillRect(cx - 35, cy + 55, 70, 6, ILI9341_BLACK);
}

void loop() {
}