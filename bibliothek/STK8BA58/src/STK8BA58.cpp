// SPDX-License-Identifier: CC0-1.0
#include "STK8BA58.h"

// Register (Datenblatt STK8BA58)
static const uint8_t REG_CHIP_ID  = 0x00;
static const uint8_t REG_XOUT1    = 0x02;   // danach XOUT2, YOUT1, YOUT2, ZOUT1, ZOUT2
static const uint8_t REG_RANGESEL = 0x0F;
static const uint8_t REG_BWSEL    = 0x10;
static const uint8_t REG_POWMODE  = 0x11;
static const uint8_t REG_SWRST    = 0x14;

static const uint8_t RANGE_2G = 0x03;
static const uint8_t RANGE_4G = 0x05;
static const uint8_t RANGE_8G = 0x08;
static const uint8_t BW_125HZ = 0x0C;
static const uint8_t SOFT_RESET = 0xB6;

static const float G_TO_MS2 = 9.80665f;

STK8BA58::STK8BA58(TwoWire &wire, uint8_t address) : _wire(wire), _address(address) {}

#if defined(ARDUINO_ARCH_RP2040)
bool STK8BA58::begin(int sdaPin, int sclPin) {
    _wire.setSDA(sdaPin);
    _wire.setSCL(sclPin);
    return begin();
}
#endif

bool STK8BA58::begin() {
    _wire.begin();
    if (readRegister(REG_CHIP_ID) != CHIP_ID) {
        return false;
    }
    writeRegister(REG_SWRST, SOFT_RESET);
    delay(5);
    writeRegister(REG_POWMODE, 0x00);   // Normalbetrieb
    writeRegister(REG_BWSEL, BW_125HZ);
    return setRange(2);
}

bool STK8BA58::setRange(int g) {
    uint8_t value;
    if (g <= 2) {
        g = 2;
        value = RANGE_2G;
    } else if (g <= 4) {
        g = 4;
        value = RANGE_4G;
    } else {
        g = 8;
        value = RANGE_8G;
    }
    if (!writeRegister(REG_RANGESEL, value)) {
        return false;
    }
    _range = g;
    delay(20);                          // bis der erste Messwert vorliegt
    return true;
}

bool STK8BA58::accelerationAvailable() {
    // Bit 0 der Register XOUT1, YOUT1, ZOUT1: neuer Wert vorhanden
    return (readRegister(REG_XOUT1) & 0x01) != 0;
}

bool STK8BA58::readRaw(int16_t &x, int16_t &y, int16_t &z) {
    uint8_t b[6];
    if (!readRegisters(REG_XOUT1, b, sizeof b)) {
        x = y = z = 0;
        return false;
    }
    // 12 Bit, linksbündig: oberes Nibble von OUT1 = Bits 3..0, OUT2 = Bits 11..4
    int16_t cx = (int16_t)((uint16_t)b[1] << 8 | (b[0] & 0xF0)) >> 4;
    int16_t cy = (int16_t)((uint16_t)b[3] << 8 | (b[2] & 0xF0)) >> 4;
    int16_t cz = (int16_t)((uint16_t)b[5] << 8 | (b[4] & 0xF0)) >> 4;
    if (_orientation == PICOBOY_COLOR_PLUS) {
        // Chip gedreht eingebaut: x und y vertauscht, Vorzeichen anders
        // (am Gerät gemessen: flach liegend z = +1 g)
        x = cy;
        y = -cx;
        z = -cz;
    } else {
        x = cx;
        y = cy;
        z = cz;
    }
    return true;
}

bool STK8BA58::readAcceleration(float &x, float &y, float &z) {
    int16_t rx, ry, rz;
    bool ok = readRaw(rx, ry, rz);
    // ±2 g: 1024 Stufen pro g, ±4 g: 512, ±8 g: 256
    float proG = 2048.0f / _range;
    x = rx / proG;
    y = ry / proG;
    z = rz / proG;
    return ok;
}

bool STK8BA58::readAccelerationMS2(float &x, float &y, float &z) {
    bool ok = readAcceleration(x, y, z);
    x *= G_TO_MS2;
    y *= G_TO_MS2;
    z *= G_TO_MS2;
    return ok;
}

float STK8BA58::readX() { float x, y, z; readAcceleration(x, y, z); return x; }
float STK8BA58::readY() { float x, y, z; readAcceleration(x, y, z); return y; }
float STK8BA58::readZ() { float x, y, z; readAcceleration(x, y, z); return z; }

uint8_t STK8BA58::readRegister(uint8_t reg) {
    uint8_t value = 0;
    readRegisters(reg, &value, 1);
    return value;
}

bool STK8BA58::writeRegister(uint8_t reg, uint8_t value) {
    _wire.beginTransmission(_address);
    _wire.write(reg);
    _wire.write(value);
    return _wire.endTransmission() == 0;
}

bool STK8BA58::readRegisters(uint8_t reg, uint8_t *buffer, size_t length) {
    _wire.beginTransmission(_address);
    _wire.write(reg);
    if (_wire.endTransmission(false) != 0) {     // ohne Stopp: gleich weiterlesen
        return false;
    }
    if (_wire.requestFrom(_address, length) != length) {
        return false;
    }
    for (size_t i = 0; i < length; i++) {
        buffer[i] = _wire.read();
    }
    return true;
}
