// SPDX-License-Identifier: CC0-1.0
// STK8BA58 – Arduino-Bibliothek für den 3-Achsen-Beschleunigungssensor
// STK8BA58 von Sensortek (I2C, Adresse 0x18 oder 0x19).
//
// Der Sensor liefert für jede Achse einen 12-Bit-Wert. Im Messbereich ±2 g
// entsprechen 1024 Stufen einer Erdbeschleunigung (1 g).
//
// Beispiel:
//   STK8BA58 sensor;
//   sensor.begin();                     // Wire mit den Standard-Pins
//   float x, y, z;
//   sensor.readAcceleration(x, y, z);   // in g
//
// PicoBoy Color Plus: Der Sensor hängt an GP20 (SDA) und GP21 (SCL) und liegt
// gedreht auf der Platine. Deshalb:
//   sensor.begin(20, 21);
//   sensor.setOrientation(STK8BA58::PICOBOY_COLOR_PLUS);
// Danach zeigt +x nach rechts, +y nach unten (zum Joystick), +z aus dem
// Display heraus. Flach auf dem Tisch: x = 0, y = 0, z = +1 g.

#ifndef STK8BA58_H
#define STK8BA58_H

#include <Arduino.h>
#include <Wire.h>

class STK8BA58 {
public:
    // Ausrichtung der Achsen
    enum Orientation {
        CHIP,                // Achsen genau wie im Datenblatt des Chips
        PICOBOY_COLOR_PLUS   // wie oben beschrieben für den PicoBoy Color Plus
    };

    static const uint8_t DEFAULT_ADDRESS = 0x18;   // SDO an GND
    static const uint8_t CHIP_ID = 0x87;            // Inhalt von Register 0x00

    explicit STK8BA58(TwoWire &wire = Wire, uint8_t address = DEFAULT_ADDRESS);

    // Startet Wire, prüft die Chip-ID und stellt ±2 g, 125 Hz, Normalbetrieb
    // ein. Gibt false zurück, wenn der Sensor nicht antwortet.
    bool begin();
#if defined(ARDUINO_ARCH_RP2040)
    // Wie begin(), legt vorher die I2C-Pins fest (nur RP2040/RP2350).
    bool begin(int sdaPin, int sclPin);
#endif

    // Messbereich ±2, ±4 oder ±8 g. Größerer Bereich = gröber aufgelöst.
    bool setRange(int g);
    int range() const { return _range; }

    void setOrientation(Orientation orientation) { _orientation = orientation; }

    // Neue Messwerte seit dem letzten Lesen?
    bool accelerationAvailable();

    // Beschleunigung in g (1 g = 9,81 m/s²). false bei einem I2C-Fehler.
    bool readAcceleration(float &x, float &y, float &z);
    // Dasselbe in m/s².
    bool readAccelerationMS2(float &x, float &y, float &z);
    // Rohwerte (-2048 … 2047), schon gedreht nach setOrientation().
    bool readRaw(int16_t &x, int16_t &y, int16_t &z);

    // Einzelne Achse in g (liest jeweils alle drei Achsen neu).
    float readX();
    float readY();
    float readZ();

    // Registerzugriff für eigene Experimente
    uint8_t readRegister(uint8_t reg);
    bool writeRegister(uint8_t reg, uint8_t value);

private:
    bool readRegisters(uint8_t reg, uint8_t *buffer, size_t length);

    TwoWire &_wire;
    uint8_t _address;
    int _range = 2;
    Orientation _orientation = CHIP;
};

#endif
