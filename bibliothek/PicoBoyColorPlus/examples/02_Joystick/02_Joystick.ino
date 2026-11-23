// Ein Punkt, der sich mit dem Joystick steuern laesst
#include <Adafruit_ST7789.h>

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9
#define TFT_LICHT 26
#define TASTE_HOCH 4
#define TASTE_RUNTER 2
#define TASTE_LINKS 3
#define TASTE_RECHTS 1
#define TASTE_MITTE 0
#define RADIUS 8

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);

int x = 120;
int y = 140;
uint16_t farbe = ST77XX_YELLOW;
bool mitte_vorher = false;

void setup() {
    tft.init(240, 280);
    pinMode(TFT_LICHT, OUTPUT);
    digitalWrite(TFT_LICHT, HIGH);
    tft.fillScreen(ST77XX_BLACK);
    pinMode(TASTE_HOCH, INPUT_PULLUP);
    pinMode(TASTE_RUNTER, INPUT_PULLUP);
    pinMode(TASTE_LINKS, INPUT_PULLUP);
    pinMode(TASTE_RECHTS, INPUT_PULLUP);
    pinMode(TASTE_MITTE, INPUT_PULLUP);
}

void loop() {
    tft.fillCircle(x, y, RADIUS, ST77XX_BLACK);     // alten Punkt loeschen

    // solange gedrueckt: bewegen
    if (digitalRead(TASTE_LINKS) == LOW && x > 30) {
        x = x - 2;
    }
    if (digitalRead(TASTE_RECHTS) == LOW && x < 210) {
        x = x + 2;
    }
    if (digitalRead(TASTE_HOCH) == LOW && y > 30) {
        y = y - 2;
    }
    if (digitalRead(TASTE_RUNTER) == LOW && y < 250) {
        y = y + 2;
    }

    // einmal pro Druck: Farbe wechseln
    bool mitte = digitalRead(TASTE_MITTE) == LOW;
    if (mitte && !mitte_vorher) {
        if (farbe == ST77XX_YELLOW) {
            farbe = ST77XX_CYAN;
        } else {
            farbe = ST77XX_YELLOW;
        }
    }
    mitte_vorher = mitte;

    tft.fillCircle(x, y, RADIUS, farbe);            // neuen Punkt zeichnen
    delay(10);
}
