// Bildschirmschoner: viele Baelle prallen ab, oben steht die Bildrate
#include <Adafruit_ST7789.h>

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9
#define TFT_LICHT 26
#define ANZAHL 8
#define TAKT 33

struct Ball {
    float x, y;
    float dx, dy;
    int radius;
    uint16_t farbe;
};

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 bild(240, 280);

Ball baelle[ANZAHL];
unsigned long letztes_bild = 0;
unsigned long sekunde_start = 0;
int anzahl = 0;                     // Bilder in dieser Sekunde
int bildrate = 0;                   // Bilder in der letzten Sekunde

void setup() {
    tft.init(240, 280);
    tft.setSPISpeed(62500000);
    pinMode(TFT_LICHT, OUTPUT);
    digitalWrite(TFT_LICHT, HIGH);
    for (int i = 0; i < ANZAHL; i++) {
        baelle[i].radius = random(6, 20);
        baelle[i].x = random(30, 210);
        baelle[i].y = random(40, 240);
        baelle[i].dx = random(-40, 41) / 10.0;
        baelle[i].dy = random(-40, 41) / 10.0;
        baelle[i].farbe = tft.color565(random(80, 256), random(80, 256), random(80, 256));
    }
}

// Ball Nummer i einen Schritt weiter bewegen und an den Raendern abprallen
void bewege(int i) {
    baelle[i].x = baelle[i].x + baelle[i].dx;
    baelle[i].y = baelle[i].y + baelle[i].dy;
    int r = baelle[i].radius;
    if (baelle[i].x < r || baelle[i].x > 239 - r) {
        baelle[i].x = constrain(baelle[i].x, r, 239 - r);
        baelle[i].dx = -baelle[i].dx;
    }
    if (baelle[i].y < r || baelle[i].y > 279 - r) {
        baelle[i].y = constrain(baelle[i].y, r, 279 - r);
        baelle[i].dy = -baelle[i].dy;
    }
}

void loop() {
    if (millis() - letztes_bild >= TAKT) {
        letztes_bild = millis();
        bild.fillScreen(ST77XX_BLACK);
        for (int i = 0; i < ANZAHL; i++) {
            bewege(i);
            bild.fillCircle(baelle[i].x, baelle[i].y, baelle[i].radius, baelle[i].farbe);
        }
        bild.setTextColor(ST77XX_WHITE);
        bild.setTextSize(2);
        bild.setCursor(60, 12);
        bild.printf("%d Bilder/s", bildrate);
        tft.drawRGBBitmap(0, 0, bild.getBuffer(), 240, 280);
        anzahl = anzahl + 1;
    }
    if (millis() - sekunde_start >= 1000) {
        bildrate = anzahl;
        anzahl = 0;
        sekunde_start = millis();
    }
}
