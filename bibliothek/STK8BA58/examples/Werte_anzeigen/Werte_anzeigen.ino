// Gibt die Beschleunigung zweimal pro Sekunde im seriellen Monitor aus.
// Für den PicoBoy Color Plus; auf anderen Boards begin() ohne Pins benutzen.
#include <STK8BA58.h>

STK8BA58 sensor;

void setup() {
    Serial.begin(115200);
    if (!sensor.begin(20, 21)) {
        Serial.println("Sensor antwortet nicht");
    }
    sensor.setOrientation(STK8BA58::PICOBOY_COLOR_PLUS);
}

void loop() {
    float x, y, z;
    sensor.readAcceleration(x, y, z);
    Serial.printf("x %5.2f g   y %5.2f g   z %5.2f g\n", x, y, z);
    delay(500);
}
