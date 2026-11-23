// Shows "Hello!" on the display
#include <Adafruit_ST7789.h>

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9
#define TFT_BACKLIGHT 26

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    tft.init(240, 280);
    tft.fillScreen(ST77XX_BLACK);
    pinMode(TFT_BACKLIGHT, OUTPUT);
    digitalWrite(TFT_BACKLIGHT, HIGH);

    tft.setCursor(66, 128);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(3);
    tft.print("Hello!");
}

void loop() {
}
