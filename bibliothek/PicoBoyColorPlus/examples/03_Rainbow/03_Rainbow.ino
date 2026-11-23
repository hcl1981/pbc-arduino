// Die RGB-LED laeuft langsam durch alle Farben des Regenbogens
#include <Adafruit_NeoPixel.h>

#define RGB_LED 11

Adafruit_NeoPixel rgb(1, RGB_LED, NEO_GRB + NEO_KHZ800);

uint16_t farbton = 0;       // 0 bis 65535: einmal rund um den Farbkreis

void setup() {
    rgb.begin();
    rgb.setBrightness(40);
}

void loop() {
    uint32_t farbe = rgb.gamma32(rgb.ColorHSV(farbton));
    rgb.setPixelColor(0, farbe);
    rgb.show();
    farbton = farbton + 256;    // nach 256 Schritten ist der Kreis rund
    delay(20);
}
