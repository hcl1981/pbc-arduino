// Wasserwaage auf dem PicoBoy Color Plus: Eine Blase wandert zur höchsten Stelle.
// Braucht zusätzlich die Bibliotheken "Adafruit ST7735 and ST7789" und "Adafruit GFX".
#include <Adafruit_ST7789.h>
#include <STK8BA58.h>

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9
#define TFT_LICHT 26

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 bild(240, 280);
STK8BA58 sensor;

void setup() {
    tft.init(240, 280);
    pinMode(TFT_LICHT, OUTPUT);
    digitalWrite(TFT_LICHT, HIGH);
    sensor.begin(20, 21);
    sensor.setOrientation(STK8BA58::PICOBOY_COLOR_PLUS);
}

void loop() {
    float x, y, z;
    sensor.readAcceleration(x, y, z);
    // Eine angehobene Seite misst positiv: Die Blase wandert dorthin,
    // wie bei einer echten Wasserwaage zur höchsten Stelle.
    int bx = 120 + (int)(x * 100);
    int by = 140 + (int)(y * 100);
    bild.fillScreen(ST77XX_BLACK);
    bild.drawCircle(120, 140, 100, ST77XX_WHITE);
    bild.drawCircle(120, 140, 12, ST77XX_GREEN);
    bild.fillCircle(bx, by, 10, ST77XX_YELLOW);
    tft.drawRGBBitmap(0, 0, bild.getBuffer(), 240, 280);
    delay(20);
}
