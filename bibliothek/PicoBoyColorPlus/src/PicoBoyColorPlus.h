// PicoBoy Color Plus: pin map
// The examples write the pin numbers directly. If you prefer names,
// include this file.
#pragma once

// display (ST7789, 240 x 280, SPI0)
#define PBC_TFT_CS        10
#define PBC_TFT_DC         8
#define PBC_TFT_RST        9
#define PBC_TFT_BACKLIGHT 26

// joystick and buttons (pressed = LOW, use INPUT_PULLUP)
#define PBC_UP             4
#define PBC_DOWN           2
#define PBC_LEFT           3
#define PBC_RIGHT          1
#define PBC_CENTER         0
#define PBC_BUTTON_A      27
#define PBC_BUTTON_B      28

// LEDs (HIGH = on) and RGB LED (WS2812)
#define PBC_LED_RED       14
#define PBC_LED_YELLOW    13
#define PBC_LED_GREEN     12
#define PBC_RGB_LED       11

// speaker, accelerometer (I2C), battery voltage
#define PBC_SPEAKER       15
#define PBC_SDA           20
#define PBC_SCL           21
#define PBC_BATTERY       29   // analogRead
