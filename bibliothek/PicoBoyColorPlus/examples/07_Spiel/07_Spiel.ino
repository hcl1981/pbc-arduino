// Sternenjaeger: Sterne sammeln, Meteoren ausweichen
#include <Adafruit_ST7789.h>
#include <STK8BA58.h>
#include "bilder.h"

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST 9
#define TFT_LICHT 26
#define LAUTSPRECHER 15
#define LED_ROT 14
#define TASTE_LINKS 3
#define TASTE_RECHTS 1
#define TASTE_A 27

#define TAKT 33                     // ms pro Bild
#define RAKETE_Y 236                // die Rakete fliegt immer auf dieser Hoehe
#define RAKETE_TEMPO 4              // Pixel pro Bild mit dem Joystick
#define KIPP_TEMPO 12               // Pixel pro Bild bei 1 g Neigung
#define TOTZONE 0.08                // so wenig Neigung wird nicht beachtet
#define METEORE 4
#define START_LEBEN 3
#define PUNKTE_PRO_LEVEL 5

enum Zustand { START, SPIEL, ENDE };

struct Objekt {
    float x, y;                     // linke obere Ecke
    float tempo;                    // Pixel pro Bild nach unten
};

struct Rechteck {
    int x, y;
    int b, h;
};

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 bild(240, 280);
STK8BA58 sensor;

Zustand zustand = START;
bool sensor_da = false;
bool a_vorher = false;
unsigned long letztes_bild = 0;
unsigned long ende_zeit = 0;        // wann das letzte Spiel endete
unsigned long treffer_zeit = 0;     // wann die Rakete zuletzt getroffen wurde

float rakete_x = 112;
Objekt meteore[METEORE];
Objekt stern;
int punkte = 0;
int leben = START_LEBEN;
int level = 1;
int rekord = 0;                     // nur im RAM: nach dem Ausschalten weg
bool neuer_rekord = false;

// ---- Hilfsfunktionen ------------------------------------------------------

// true genau einmal pro Druck auf A
bool a_gedrueckt() {
    bool jetzt = digitalRead(TASTE_A) == LOW;
    bool neu = jetzt && !a_vorher;
    a_vorher = jetzt;
    return neu;
}

bool trifft(Rechteck a, Rechteck c) {
    return a.x < c.x + c.b && c.x < a.x + a.b &&
           a.y < c.y + c.h && c.y < a.y + a.h;
}

void text_mitte(String text, int y, int groesse, uint16_t farbe) {
    bild.setTextSize(groesse);
    bild.setTextColor(farbe);
    bild.setCursor(120 - text.length() * 3 * groesse, y);
    bild.print(text);
}

void meteor_nach_oben(int i) {
    meteore[i].x = random(20, 220 - METEOR_BREITE);
    meteore[i].y = random(-200, -METEOR_HOEHE);
    meteore[i].tempo = random(15, 30) / 10.0 + level * 0.5;
}

void stern_nach_oben() {
    stern.x = random(20, 220 - STERN_BREITE);
    stern.y = random(-60, -STERN_HOEHE);
    stern.tempo = 2.0 + level * 0.3;
}

void neues_spiel() {
    randomSeed(micros());           // der Zeitpunkt des Drucks auf A ist Zufall
    punkte = 0;
    leben = START_LEBEN;
    level = 1;
    neuer_rekord = false;
    rakete_x = 112;
    for (int i = 0; i < METEORE; i++) {
        meteor_nach_oben(i);
    }
    stern_nach_oben();
    zustand = SPIEL;
}

// ---- Die Rakete steuern --------------------------------------------------

void steuern() {
    if (digitalRead(TASTE_LINKS) == LOW) {
        rakete_x = rakete_x - RAKETE_TEMPO;
    }
    if (digitalRead(TASTE_RECHTS) == LOW) {
        rakete_x = rakete_x + RAKETE_TEMPO;
    }
    if (sensor_da) {
        float x, y, z;
        sensor.readAcceleration(x, y, z);
        if (abs(x) > TOTZONE) {
            rakete_x = rakete_x - x * KIPP_TEMPO;   // tiefere Seite: x negativ
        }
    }
    rakete_x = constrain(rakete_x, 20, 220 - RAKETE_BREITE);
}

// ---- Ein Schritt des Spiels ------------------------------------------------

void spiel_schritt() {
    steuern();
    Rechteck rakete = { (int)rakete_x + 4, RAKETE_Y + 2, 8, 20 };

    // Meteore: fallen, treffen, kommen oben wieder
    for (int i = 0; i < METEORE; i++) {
        meteore[i].y = meteore[i].y + meteore[i].tempo;
        Rechteck m = { (int)meteore[i].x + 2, (int)meteore[i].y + 2, 12, 12 };
        if (trifft(rakete, m)) {
            leben = leben - 1;
            treffer_zeit = millis();
            tone(LAUTSPRECHER, 150, 300);
            meteor_nach_oben(i);
        } else if (meteore[i].y > 280) {
            meteor_nach_oben(i);
        }
    }

    // der Stern: einsammeln gibt einen Punkt
    stern.y = stern.y + stern.tempo;
    Rechteck s = { (int)stern.x, (int)stern.y, STERN_BREITE, STERN_HOEHE };
    if (trifft(rakete, s)) {
        punkte = punkte + 1;
        tone(LAUTSPRECHER, 1320, 80);
        if (punkte % PUNKTE_PRO_LEVEL == 0) {
            level = level + 1;
        }
        stern_nach_oben();
    } else if (stern.y > 280) {
        stern_nach_oben();
    }

    // rote LED leuchtet kurz nach einem Treffer
    digitalWrite(LED_ROT, millis() - treffer_zeit < 300);

    if (leben <= 0) {
        if (punkte > rekord) {
            rekord = punkte;
            neuer_rekord = true;
        }
        Serial.printf("Spiel vorbei. Punkte: %d, Level %d, Rekord %d\n", punkte, level, rekord);
        ende_zeit = millis();
        zustand = ENDE;
    }
}

// ---- Zeichnen ------------------------------------------------------------------

void zeichne_himmel() {
    bild.fillScreen(tft.color565(10, 10, 40));
    // feste "Sterne" im Hintergrund, aus der Nummer berechnet
    for (int i = 0; i < 30; i++) {
        bild.drawPixel((i * 73) % 240, (i * 131) % 280, ST77XX_WHITE);
    }
}

void zeichne_start() {
    zeichne_himmel();
    text_mitte("STERNEN-", 50, 3, ST77XX_YELLOW);
    text_mitte("JAEGER", 80, 3, ST77XX_YELLOW);
    bild.drawRGBBitmap(112, 125, rakete_pixel, rakete_maske, RAKETE_BREITE, RAKETE_HOEHE);
    text_mitte("Sterne sammeln,", 165, 1, ST77XX_WHITE);
    text_mitte("Meteoren ausweichen", 177, 1, ST77XX_WHITE);
    text_mitte("Rekord: " + String(rekord), 200, 2, ST77XX_CYAN);
    text_mitte("A: Start", 235, 2, ST77XX_GREEN);
}

void zeichne_spiel() {
    zeichne_himmel();
    bild.drawRGBBitmap(stern.x, stern.y, stern_pixel, stern_maske, STERN_BREITE, STERN_HOEHE);
    for (int i = 0; i < METEORE; i++) {
        bild.drawRGBBitmap(meteore[i].x, meteore[i].y, meteor_pixel, meteor_maske,
                           METEOR_BREITE, METEOR_HOEHE);
    }
    // nach einem Treffer blinkt die Rakete eine Sekunde lang
    bool sichtbar = millis() - treffer_zeit > 1000 || (millis() / 100) % 2 == 0;
    if (sichtbar) {
        bild.drawRGBBitmap(rakete_x, RAKETE_Y, rakete_pixel, rakete_maske,
                           RAKETE_BREITE, RAKETE_HOEHE);
    }
    // oben: Punkte, Level und Leben als Herzen
    bild.setTextSize(2);
    bild.setTextColor(ST77XX_WHITE);
    bild.setCursor(30, 12);
    bild.print(punkte);
    bild.setTextColor(ST77XX_CYAN);
    bild.setCursor(80, 12);
    bild.printf("L%d", level);
    for (int i = 0; i < leben; i++) {
        bild.drawRGBBitmap(200 - i * 12, 16, herz_pixel, herz_maske, HERZ_BREITE, HERZ_HOEHE);
    }
}

void zeichne_ende() {
    zeichne_himmel();
    text_mitte("SPIEL VORBEI", 70, 2, ST77XX_RED);
    text_mitte("Punkte: " + String(punkte), 110, 3, ST77XX_WHITE);
    if (neuer_rekord) {
        text_mitte("Neuer Rekord!", 160, 2, ST77XX_YELLOW);
    } else {
        text_mitte("Rekord: " + String(rekord), 160, 2, ST77XX_CYAN);
    }
    text_mitte("A: Nochmal", 220, 2, ST77XX_GREEN);
}

// ---- setup und loop --------------------------------------------------------

void setup() {
    Serial.begin(115200);
    tft.init(240, 280);
    tft.setSPISpeed(62500000);
    pinMode(TFT_LICHT, OUTPUT);
    digitalWrite(TFT_LICHT, HIGH);
    pinMode(TASTE_LINKS, INPUT_PULLUP);
    pinMode(TASTE_RECHTS, INPUT_PULLUP);
    pinMode(TASTE_A, INPUT_PULLUP);
    pinMode(LED_ROT, OUTPUT);
    sensor_da = sensor.begin(20, 21);
    sensor.setOrientation(STK8BA58::PICOBOY_COLOR_PLUS);
}

void loop() {
    if (millis() - letztes_bild < TAKT) {
        return;
    }
    letztes_bild = millis();

    switch (zustand) {
        case START:
            if (a_gedrueckt()) {
                neues_spiel();
            }
            zeichne_start();
            break;
        case SPIEL:
            spiel_schritt();
            zeichne_spiel();
            break;
        case ENDE:
            // erst nach einer Sekunde: nicht aus Versehen gleich weiterspielen
            if (a_gedrueckt() && millis() - ende_zeit > 1000) {
                neues_spiel();
            }
            zeichne_ende();
            break;
    }
    tft.drawRGBBitmap(0, 0, bild.getBuffer(), 240, 280);
}
