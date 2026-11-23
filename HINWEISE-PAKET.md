# PicoBoy Color Plus – Board-Paket für die Arduino IDE

Version {VERSION}, beruht auf **Arduino-Pico {KERN}** von Earle F. Philhower, III
(https://github.com/earlephilhower/arduino-pico).

## Was hier geändert ist

Gegenüber Arduino-Pico {KERN} sind nur diese Dinge anders:

- `boards.txt` enthält nur das Board **PicoBoy Color Plus**. Es entspricht
  „Generic RP2350“ mit 16 MB Flash, davon 8 MB Dateisystem, und dem
  USB-Namen „PicoBoy Color Plus“.
- Zusätzliche Upload-Methode **PBConPC (Emulator)**: Sie startet den Sketch im
  Emulator PBConPC statt auf dem Gerät (`platform.txt`, Werkzeug `pbconpc`).
- `platform.txt`: Name und Version dieses Pakets.
- Die Ordner `docs` und `tests` fehlen; sie werden zum Bauen nicht gebraucht.
- Zusätzlich im Ordner `libraries`:
  - **PicoBoy Color Plus**: fünf Beispiele aus dem Handbuch „Programmieren
    mit Arduino“ und eine Datei mit der Pinbelegung (© Jan Schulz, CC0)
  - **STK8BA58**: Bibliothek für den Neigungssensor aus dem Handbuch (CC0)
  - **Adafruit GFX Library** (BSD), **Adafruit BusIO** (MIT), **Adafruit
    ST7735 and ST7789 Library** (MIT) und **Adafruit NeoPixel** (LGPL 3.0),
    unverändert aus dem Arduino-Bibliotheksverwalter; die Lizenzen liegen in
    ihren Ordnern

Alles andere ist unverändert.

## Lizenzen

Arduino-Pico steht unter der GNU LGPL 2.1 (`LICENSE`); enthaltene Teile wie
das Pico SDK, TinyUSB, lwIP, BTstack und FreeRTOS stehen unter ihren eigenen
Lizenzen, die in ihren Ordnern liegen. Die Änderungen oben stehen ebenfalls
unter der LGPL 2.1. Die Werkzeuge (Compiler, picotool, OpenOCD …) lädt die
Boardverwaltung unverändert von den Releases von Arduino-Pico.

Quelltext dieses Pakets und Bauskript: https://github.com/hcl1981/pbc-arduino

„Arduino“ und „Raspberry Pi“ sind Marken ihrer Inhaber. Dieses Paket ist ein
unabhängiges Projekt und weder von Arduino noch von Raspberry Pi noch von
Earle F. Philhower, III herausgegeben.
