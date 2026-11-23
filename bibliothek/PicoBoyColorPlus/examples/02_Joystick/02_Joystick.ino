// A dot you can move with the joystick; press the joystick to change its colour
#include <Adafruit_ST7789.h>

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9
#define TFT_BACKLIGHT 26
#define BUTTON_UP 4
#define BUTTON_DOWN 2
#define BUTTON_LEFT 3
#define BUTTON_RIGHT 1
#define BUTTON_CENTER 0
#define RADIUS 8

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);

int x = 120;
int y = 140;
uint16_t colour = ST77XX_YELLOW;
bool centerBefore = false;

void setup() {
    tft.init(240, 280);
    pinMode(TFT_BACKLIGHT, OUTPUT);
    digitalWrite(TFT_BACKLIGHT, HIGH);
    tft.fillScreen(ST77XX_BLACK);
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);
    pinMode(BUTTON_LEFT, INPUT_PULLUP);
    pinMode(BUTTON_RIGHT, INPUT_PULLUP);
    pinMode(BUTTON_CENTER, INPUT_PULLUP);
}

void loop() {
    tft.fillCircle(x, y, RADIUS, ST77XX_BLACK);     // erase the old dot

    // move while the joystick is pressed (pressed = LOW)
    if (digitalRead(BUTTON_LEFT) == LOW && x > 30) {
        x = x - 2;
    }
    if (digitalRead(BUTTON_RIGHT) == LOW && x < 210) {
        x = x + 2;
    }
    if (digitalRead(BUTTON_UP) == LOW && y > 30) {
        y = y - 2;
    }
    if (digitalRead(BUTTON_DOWN) == LOW && y < 250) {
        y = y + 2;
    }

    // once per press: change the colour
    bool center = digitalRead(BUTTON_CENTER) == LOW;
    if (center && !centerBefore) {
        if (colour == ST77XX_YELLOW) {
            colour = ST77XX_CYAN;
        } else {
            colour = ST77XX_YELLOW;
        }
    }
    centerBefore = center;

    tft.fillCircle(x, y, RADIUS, colour);           // draw the new dot
    delay(10);
}
