// Spielt den Anfang von "Alle meine Entchen"
#define LAUTSPRECHER 15

// Frequenzen in Hertz (gerundet)
#define TON_C 262
#define TON_D 294
#define TON_E 330
#define TON_F 349
#define TON_G 392
#define TON_A 440

#define ACHTEL 250      // Dauer einer Achtelnote in ms

void setup() {
    tone(LAUTSPRECHER, TON_C, ACHTEL - 30);
    delay(ACHTEL);
    tone(LAUTSPRECHER, TON_D, ACHTEL - 30);
    delay(ACHTEL);
    tone(LAUTSPRECHER, TON_E, ACHTEL - 30);
    delay(ACHTEL);
    tone(LAUTSPRECHER, TON_F, ACHTEL - 30);
    delay(ACHTEL);
    tone(LAUTSPRECHER, TON_G, 2 * ACHTEL - 30);
    delay(2 * ACHTEL);
    tone(LAUTSPRECHER, TON_G, 2 * ACHTEL - 30);
    delay(2 * ACHTEL);
    tone(LAUTSPRECHER, TON_A, ACHTEL - 30);
    delay(ACHTEL);
    tone(LAUTSPRECHER, TON_A, ACHTEL - 30);
    delay(ACHTEL);
    tone(LAUTSPRECHER, TON_A, ACHTEL - 30);
    delay(ACHTEL);
    tone(LAUTSPRECHER, TON_A, ACHTEL - 30);
    delay(ACHTEL);
    tone(LAUTSPRECHER, TON_G, 4 * ACHTEL - 30);
    delay(4 * ACHTEL);
}

void loop() {
}
