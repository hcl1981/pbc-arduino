// The RGB LED slowly fades through all the colours of the rainbow
#include <Adafruit_NeoPixel.h>

#define RGB_LED 11

Adafruit_NeoPixel rgb(1, RGB_LED, NEO_GRB + NEO_KHZ800);

uint16_t hue = 0;           // 0 to 65535: once around the colour wheel

void setup() {
    rgb.begin();
    rgb.setBrightness(40);
}

void loop() {
    uint32_t colour = rgb.gamma32(rgb.ColorHSV(hue));
    rgb.setPixelColor(0, colour);
    rgb.show();
    hue = hue + 256;            // after 256 steps the wheel is complete
    delay(20);
}
