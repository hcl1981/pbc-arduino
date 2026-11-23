// Spirit level: the bubble rises to the higher side; the green LED lights up when level
#include <Adafruit_ST7789.h>
#include <STK8BA58.h>

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9
#define TFT_BACKLIGHT 26
#define LED_GREEN 12
#define SMOOTHING 0.2
#define LEVEL 0.03                  // this little tilt counts as level

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(240, 280);       // draw here first, then send it all at once: no flicker
STK8BA58 sensor;

float gx = 0;                       // smoothed readings
float gy = 0;

void setup() {
    tft.init(240, 280);
    tft.setSPISpeed(62500000);
    pinMode(TFT_BACKLIGHT, OUTPUT);
    digitalWrite(TFT_BACKLIGHT, HIGH);
    pinMode(LED_GREEN, OUTPUT);
    sensor.begin(20, 21);
    sensor.setOrientation(STK8BA58::PICOBOY_COLOR_PLUS);
}

void loop() {
    float x, y, z;
    sensor.readAcceleration(x, y, z);
    gx = gx + (x - gx) * SMOOTHING;
    gy = gy + (y - gy) * SMOOTHING;

    // a tilt of 0.1 g moves the bubble by 20 pixels, at most to the edge
    int bx = 120 + constrain(gx * 200, -80, 80);
    int by = 140 + constrain(gy * 200, -80, 80);
    bool level = abs(gx) < LEVEL && abs(gy) < LEVEL;

    canvas.fillScreen(ST77XX_BLACK);
    canvas.fillCircle(120, 140, 95, tft.color565(40, 110, 40));
    canvas.drawFastHLine(25, 140, 190, ST77XX_WHITE);
    canvas.drawFastVLine(120, 45, 190, ST77XX_WHITE);
    canvas.drawCircle(120, 140, 16, ST77XX_WHITE);
    if (level) {
        canvas.fillCircle(bx, by, 12, ST77XX_GREEN);
    } else {
        canvas.fillCircle(bx, by, 12, ST77XX_YELLOW);
    }
    canvas.setTextColor(ST77XX_WHITE);
    canvas.setTextSize(2);
    canvas.setCursor(30, 250);
    canvas.printf("x %5.2f  y %5.2f", gx, gy);
    tft.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 280);
    digitalWrite(LED_GREEN, level);
    delay(20);
}
