// Plays the beginning of the German children's song "Alle meine Entchen"
#define SPEAKER 15

// frequencies in hertz (rounded)
#define NOTE_C 262
#define NOTE_D 294
#define NOTE_E 330
#define NOTE_F 349
#define NOTE_G 392
#define NOTE_A 440

#define EIGHTH 250      // length of an eighth note in ms

void setup() {
    tone(SPEAKER, NOTE_C, EIGHTH - 30);
    delay(EIGHTH);
    tone(SPEAKER, NOTE_D, EIGHTH - 30);
    delay(EIGHTH);
    tone(SPEAKER, NOTE_E, EIGHTH - 30);
    delay(EIGHTH);
    tone(SPEAKER, NOTE_F, EIGHTH - 30);
    delay(EIGHTH);
    tone(SPEAKER, NOTE_G, 2 * EIGHTH - 30);
    delay(2 * EIGHTH);
    tone(SPEAKER, NOTE_G, 2 * EIGHTH - 30);
    delay(2 * EIGHTH);
    tone(SPEAKER, NOTE_A, EIGHTH - 30);
    delay(EIGHTH);
    tone(SPEAKER, NOTE_A, EIGHTH - 30);
    delay(EIGHTH);
    tone(SPEAKER, NOTE_A, EIGHTH - 30);
    delay(EIGHTH);
    tone(SPEAKER, NOTE_A, EIGHTH - 30);
    delay(EIGHTH);
    tone(SPEAKER, NOTE_G, 4 * EIGHTH - 30);
    delay(4 * EIGHTH);
}

void loop() {
}
