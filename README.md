# PicoBoy Color Plus für die Arduino IDE

Ein Board-Paket für den **PicoBoy Color Plus** (PBC+). Es bringt alles mit,
was man für das Handbuch [Programmieren mit Arduino](https://pbcplus.de/programmieren/arduino/)
braucht:

- das Board **PicoBoy Color Plus** mit den richtigen Einstellungen
  (16 MB Flash, davon 8 MB Dateisystem),
- die Upload-Methode **PBConPC (Emulator)**: Hochladen startet den Sketch im
  Emulator [PBConPC](https://github.com/hcl1981/PBConPC) statt auf dem Gerät,
  ganz ohne PicoBoy (Windows und Linux),
- fünf **Beispiele** (englisch, nach dem Handbuch) unter **Datei → Beispiele → PicoBoy Color Plus**,
- die Bibliotheken für Display, RGB-LED und Neigungssensor.

Es beruht auf [Arduino-Pico](https://github.com/earlephilhower/arduino-pico)
von Earle F. Philhower, III, in einer festen Version. Was geändert ist und
welche Lizenzen gelten, steht in [`HINWEISE-PAKET.md`](HINWEISE-PAKET.md).

## Installieren

1. In der Arduino IDE **Datei → Einstellungen** öffnen und bei
   **Zusätzliche Boardverwalter-URLs** eintragen:
   ```
   https://github.com/hcl1981/pbc-arduino/releases/download/global/package_pbcplus_index.json
   ```
2. **Werkzeuge → Board → Boardverwaltung**, nach **PicoBoy** suchen und
   **PicoBoy Color Plus** installieren.
3. **Werkzeuge → Board → PicoBoy Color Plus** wählen.

## Im Emulator ausprobieren

**Werkzeuge → Upload Method → PBConPC (Emulator)** wählen und wie gewohnt
**Hochladen** klicken. Ein Port muss dafür nicht gewählt sein. Für das Gerät
stellt man die Upload-Methode wieder auf **Default (UF2)**.

Ausgaben von `Serial` zeigt PBConPC im Menü **View → USB serial terminal**.
PBConPC gibt es für Windows und Linux (x86-64), noch nicht für macOS.

## Bauen

```bash
python3 werkzeuge/paket_bauen.py --version 1.0.0 --kern 6.4.0 --pbconpc 1.1.0 \
    --basis-url https://github.com/hcl1981/pbc-arduino/releases/download/v1.0.0
```

Ergebnis in `dist/`: das Plattform-Archiv, PBConPC als Werkzeug für Windows,
Linux und als Hinweis für andere Systeme, und `package_pbcplus_index.json`.
Der Starter für Windows (`werkzeug-pbconpc/pbconpc-start.c`) wird dabei mit
MinGW übersetzt.

## Lizenz

Das Bauskript und die Starter: MIT. Der Kern: LGPL 2.1 (Arduino-Pico).
Beispiele und STK8BA58: CC0. Die übrigen Bibliotheken: unter ihren eigenen
Lizenzen. Einzelheiten in `HINWEISE-PAKET.md`.
