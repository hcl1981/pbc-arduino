// PicoBoy Color Plus: Pinbelegung
// Die Beispiele schreiben die Nummern direkt hin, so wie das Handbuch
// „Programmieren mit Arduino“. Wer lieber Namen benutzt, bindet diese Datei ein.
#pragma once

// Display (ST7789, 240 × 280, SPI0)
#define PBC_TFT_CS     10
#define PBC_TFT_DC      8
#define PBC_TFT_RST     9
#define PBC_TFT_LICHT  26   // Hintergrundlicht

// Joystick und Tasten (gedrückt = LOW, mit INPUT_PULLUP)
#define PBC_HOCH        4
#define PBC_RUNTER      2
#define PBC_LINKS       3
#define PBC_RECHTS      1
#define PBC_MITTE       0
#define PBC_TASTE_A    27
#define PBC_TASTE_B    28

// LEDs (HIGH = an) und RGB-LED (WS2812)
#define PBC_LED_ROT    14
#define PBC_LED_GELB   13
#define PBC_LED_GRUEN  12
#define PBC_RGB_LED    11

// Lautsprecher, Neigungssensor (I2C), Akkuspannung
#define PBC_LAUTSPRECHER 15
#define PBC_SDA        20
#define PBC_SCL        21
#define PBC_AKKU       29   // analogRead
