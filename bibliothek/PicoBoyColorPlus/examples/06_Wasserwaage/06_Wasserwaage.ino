// Wasserwaage: die Blase steigt zur hoeheren Seite
#include <Adafruit_ST7789.h>
#include <STK8BA58.h>

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9
#define TFT_LICHT 26
#define LED_GRUEN 12
#define ANTEIL 0.2
#define WAAGRECHT 0.03              // so wenig Neigung gilt als waagrecht

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 bild(240, 280);
STK8BA58 sensor;

float gx = 0;                       // geglaettete Messwerte
float gy = 0;

void setup() {
    tft.init(240, 280);
    tft.setSPISpeed(62500000);
    pinMode(TFT_LICHT, OUTPUT);
    digitalWrite(TFT_LICHT, HIGH);
    pinMode(LED_GRUEN, OUTPUT);
    sensor.begin(20, 21);
    sensor.setOrientation(STK8BA58::PICOBOY_COLOR_PLUS);
}

void loop() {
    float x, y, z;
    sensor.readAcceleration(x, y, z);
    gx = gx + (x - gx) * ANTEIL;
    gy = gy + (y - gy) * ANTEIL;

    // 0,1 g Neigung schiebt die Blase um 20 Pixel, hoechstens bis an den Rand
    int bx = 120 + constrain(gx * 200, -80, 80);
    int by = 140 + constrain(gy * 200, -80, 80);
    bool waagrecht = abs(gx) < WAAGRECHT && abs(gy) < WAAGRECHT;

    bild.fillScreen(ST77XX_BLACK);
    bild.fillCircle(120, 140, 95, tft.color565(40, 110, 40));
    bild.drawFastHLine(25, 140, 190, ST77XX_WHITE);
    bild.drawFastVLine(120, 45, 190, ST77XX_WHITE);
    bild.drawCircle(120, 140, 16, ST77XX_WHITE);
    if (waagrecht) {
        bild.fillCircle(bx, by, 12, ST77XX_GREEN);
    } else {
        bild.fillCircle(bx, by, 12, ST77XX_YELLOW);
    }
    bild.setTextColor(ST77XX_WHITE);
    bild.setTextSize(2);
    bild.setCursor(30, 250);
    bild.printf("x %5.2f  y %5.2f", gx, gy);
    tft.drawRGBBitmap(0, 0, bild.getBuffer(), 240, 280);
    digitalWrite(LED_GRUEN, waagrecht);
    delay(20);
}
