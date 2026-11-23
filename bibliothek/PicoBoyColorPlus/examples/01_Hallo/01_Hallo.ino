#include <Adafruit_ST7789.h>

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9
#define TFT_LICHT 26

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    tft.init(240, 280);
    tft.fillScreen(ST77XX_BLACK);
    pinMode(TFT_LICHT, OUTPUT);
    digitalWrite(TFT_LICHT, HIGH);

    tft.setCursor(66, 128);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(3);
    tft.print("Hallo!");
}

void loop() {
}
