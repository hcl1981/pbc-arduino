"""Holt die fünf Beispiele aus dem Handbuch „Programmieren mit Arduino“.

    python3 werkzeuge/beispiele_holen.py ~/Documents/pbcplus/buecher/dokumentation_arduino/buch/programme
"""
import os
import shutil
import sys

QUELLE = sys.argv[1]
ZIEL = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'bibliothek', 'PicoBoyColorPlus', 'examples')
BEISPIELE = [
    ('01_Hello', 'k03_hallo'),
    ('02_Joystick', 'k09_punkt'),
    ('03_Rainbow', 'k10_regenbogen'),
    ('04_Melody', 'k10_melodie'),
    ('05_SpiritLevel', 'k17_wasserwaage'),
]
shutil.rmtree(ZIEL, ignore_errors=True)
for name, buch in BEISPIELE:
    ordner = os.path.join(ZIEL, name)
    os.makedirs(ordner)
    for datei in os.listdir(os.path.join(QUELLE, buch)):
        if datei.startswith('.'):
            continue
        neu = name + '.ino' if datei == buch + '.ino' else datei
        shutil.copy(os.path.join(QUELLE, buch, datei), os.path.join(ordner, neu))
    print(name, '←', buch, sorted(os.listdir(ordner)))
