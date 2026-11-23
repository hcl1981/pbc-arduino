"""Baut das Board-Paket „PicoBoy Color Plus“ für die Arduino IDE.

    python3 werkzeuge/paket_bauen.py --version 1.0.0 --kern 6.4.0 --pbconpc 1.1.0 \
        --basis-url https://github.com/hcl1981/pbc-arduino/releases/download/v1.0.0

Das Paket bringt eine feste Fassung des Arduino-Pico-Kerns von Earle F.
Philhower III mit, mit genau einem Board und der zusätzlichen Upload-Methode
„PBConPC (Emulator)“. Die Werkzeuge (Compiler usw.) lädt die Boardverwaltung
von Earles Releases; PBConPC kommt aus diesem Paket.

Ergebnis in dist/: das Plattform-Archiv, die PBConPC-Archive und
package_pbcplus_index.json. Heruntergeladenes liegt in cache/.
"""
import argparse
import hashlib
import io
import json
import os
import shutil
import stat
import subprocess
import tarfile
import urllib.request
import zipfile

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.dirname(HIER)
CACHE = os.path.join(WURZEL, 'cache')
DIST = os.path.join(WURZEL, 'dist')
EARLE_INDEX = 'https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json'
PBCONPC = 'https://github.com/hcl1981/PBConPC/releases/download/v{v}/{datei}'
BOARD, QUELLE = 'picoboycolorplus', 'generic_rp2350'
FLASH = '16777216_8388608'          # 16 MB, davon 8 MB Dateisystem wie im Arduino-Buch
BIB_INDEX = 'https://downloads.arduino.cc/libraries/library_index.json.gz'
BIBLIOTHEKEN = [                     # was die Beispiele brauchen, in festen Versionen
    ('Adafruit GFX Library', '1.13.0'),
    ('Adafruit BusIO', '1.17.4'),
    ('Adafruit ST7735 and ST7789 Library', '1.12.0'),
    ('Adafruit NeoPixel', '1.15.5'),
]
# Bibliotheken des Kerns für Hardware, die der PBC+ nicht hat (Funk, Netzwerk,
# Kamera, Mikrofon, SD-Karte). Sie fallen weg, damit das Beispiel-Menü übersichtlich bleibt.
OHNE_BIBLIOTHEKEN = {
    'ArduinoOTA', 'AsyncUDP', 'BLE', 'BTstackLib', 'BluetoothAudio', 'BluetoothHCI', 'BluetoothHIDMaster',
    'DNSServer', 'ESPHost', 'HID_Bluetooth', 'HTTPClient', 'HTTPUpdate', 'HTTPUpdateServer', 'JoystickBLE',
    'JoystickBT', 'KeyboardBLE', 'KeyboardBT', 'LEAmDNS', 'MouseBLE', 'MouseBT', 'NetBIOS', 'PDM', 'PicoCamera',
    'SD', 'SDFS', 'SdFat', 'SerialBT', 'SimpleMDNS', 'WebServer', 'WiFi', 'http-parser', 'iLabs_ESP-NOW',
    'iLabs_Hearth', 'lwIP_CYW43', 'lwIP_ESPHost', 'lwIP_Ethernet', 'lwIP_USB_NCM', 'lwIP_WINC1500',
    'lwIP_enc28j60', 'lwIP_w5100', 'lwIP_w5500', 'lwIP_w55rp20', 'lwIP_w6100', 'lwIP_w6300', 'tusb-ncm',
}
# Beispiele verbliebener Bibliotheken, die eine der entfernten brauchen
OHNE_BEISPIELE = (
    'libraries/Adafruit_TinyUSB_Arduino/examples/MassStorage/msc_sd/',
    'libraries/Adafruit_TinyUSB_Arduino/examples/MassStorage/msc_esp32_file_browser/',
    'libraries/SoftwareSPI/examples/W5500SoftwareSPI/',
    'libraries/SoftwareSPI/examples/FilesSoftwareSPI/',
    'libraries/LittleFS/examples/SpeedTest/',
    'libraries/VFS/examples/VFSMultipleMounts/',
    'libraries/Adafruit_ST7735_and_ST7789_Library/examples/shieldtest/',
    'libraries/Adafruit_ST7735_and_ST7789_Library/examples/seesaw_shield18_test/',
    # USB-Host-Anschluss, SD-Karte, externer Flash, SAMD-Chips, Zusatzboards: hat der PBC+ nicht
    'libraries/Adafruit_TinyUSB_Arduino/examples/DualRole/',
    'libraries/Adafruit_TinyUSB_Arduino/examples/Host/',
    'libraries/Adafruit_TinyUSB_Arduino/examples/MassStorage/msc_external_flash/',
    'libraries/Adafruit_TinyUSB_Arduino/examples/MassStorage/msc_external_flash_sdcard/',
    'libraries/Adafruit_TinyUSB_Arduino/examples/MassStorage/msc_internal_flash_samd/',
    'libraries/Adafruit_TinyUSB_Arduino/examples/MassStorage/msc_sdfat/',
    'libraries/Adafruit_TinyUSB_Arduino/examples/HID/hid_composite_joy_featherwing/',
)
WINDOWS = ('x86_64-mingw32', 'i686-mingw32')
LINUX64 = 'x86_64-pc-linux-gnu'


def laden(url, name):
    os.makedirs(CACHE, exist_ok=True)
    pfad = os.path.join(CACHE, name)
    if not os.path.exists(pfad):
        print('lade', url)
        urllib.request.urlretrieve(url, pfad + '.teil')
        os.rename(pfad + '.teil', pfad)
    return pfad


def pruefsumme(pfad):
    h = hashlib.sha256()
    with open(pfad, 'rb') as f:
        for block in iter(lambda: f.read(1 << 20), b''):
            h.update(block)
    return 'SHA-256:' + h.hexdigest()


def boards_txt(kern_boards):
    zeilen = kern_boards.splitlines()
    menues = [z for z in zeilen if z.startswith('menu.')]
    aus = []
    for z in zeilen:
        if not z.startswith(QUELLE + '.'):
            continue
        schluessel, _, wert = z.partition('=')
        teile = schluessel.split('.')
        # nur die Aufteilungen für 16 MB; die erste ist die Voreinstellung (siehe unten)
        if len(teile) > 3 and teile[1] == 'menu' and teile[2] == 'flash' and not teile[3].startswith('16777216_'):
            continue
        schluessel = BOARD + schluessel[len(QUELLE):]
        if schluessel == BOARD + '.name':
            wert = 'PicoBoy Color Plus'
        elif schluessel == BOARD + '.build.usb_product':
            wert = '"PicoBoy Color Plus"'
        elif schluessel == BOARD + '.build.usb_manufacturer':
            wert = '"PicoBoy"'
        aus.append(schluessel + '=' + wert)
        if schluessel == BOARD + '.menu.uploadmethod.default.upload.tool.network':
            p = BOARD + '.menu.uploadmethod.pbconpc'
            aus += [p + '=PBConPC (Emulator)',
                    p + '.build.ram_length=512k',
                    p + '.build.debugscript=picoprobe_cmsis_dap.tcl',
                    p + '.upload.maximum_data_size=524288',
                    p + '.upload.tool=pbcplus_pbconpc',
                    p + '.upload.tool.default=pbcplus_pbconpc',
                    p + '.upload.require_upload_port=false',
                    p + '.upload.wait_for_upload_port=false']
    if not aus:
        raise SystemExit('Generic RP2350 fehlt im Kern')
    # Flash-Menü: 8 MB Sketch / 8 MB Dateisystem zuerst, das macht sie zur Voreinstellung
    praefix = BOARD + '.menu.flash.'
    flash = [z for z in aus if z.startswith(praefix)]
    if not any(z.startswith(praefix + FLASH) for z in flash):
        raise SystemExit('Flash-Einstellung ' + FLASH + ' fehlt im Kern')
    erst = [z for z in flash if z.startswith(praefix + FLASH + '=') or z.startswith(praefix + FLASH + '.')]
    rest = [z for z in flash if z not in erst]
    pos = aus.index(flash[0])
    aus = [z for z in aus if not z.startswith(praefix)]
    aus[pos:pos] = erst + rest
    kopf = ['# PicoBoy Color Plus – aus „Generic RP2350“ des Arduino-Pico-Kerns erzeugt', '']
    return '\n'.join(kopf + menues + [''] + aus) + '\n'


def platform_txt(kern_platform, version, kern_version):
    aus = []
    for z in kern_platform.splitlines():
        if z.startswith('name='):
            z = 'name=PicoBoy Color Plus'
        elif z.startswith('version='):
            z = 'version=' + version
        aus.append(z)
    aus += ['',
            '# PicoBoy Color Plus: Upload-Methode „PBConPC (Emulator)“ startet den Sketch im Emulator',
            'tools.pbcplus_pbconpc.upload.protocol=pbconpc',
            'tools.pbcplus_pbconpc.upload.params.verbose=',
            'tools.pbcplus_pbconpc.upload.params.quiet=',
            'tools.pbcplus_pbconpc.upload.pattern=/bin/sh "{runtime.tools.pbconpc.path}/pbconpc-start.sh" "{build.path}/{build.project_name}.uf2"',
            'tools.pbcplus_pbconpc.upload.pattern.windows="{runtime.tools.pbconpc.path}/pbconpc-start.exe" "{build.path}/{build.project_name}.uf2"']
    return '\n'.join(aus) + '\n'


def hinweise(version, kern_version):
    return open(os.path.join(WURZEL, 'HINWEISE-PAKET.md'), encoding='utf-8').read() \
        .replace('{VERSION}', version).replace('{KERN}', kern_version)


def bibliotheken_laden():
    """Die Bibliotheken, die die Beispiele brauchen, in festen Versionen aus dem
    Arduino-Bibliotheksindex. Ergebnis: {Ordnername: ZIP-Pfad}."""
    import gzip
    index_gz = laden(BIB_INDEX, 'library_index.json.gz')
    index = json.loads(gzip.open(index_gz).read())['libraries']
    ergebnis = {}
    for name, version in BIBLIOTHEKEN:
        b = next((b for b in index if b['name'] == name and b['version'] == version), None)
        if not b:
            raise SystemExit(f'{name} {version} fehlt im Bibliotheksindex')
        pfad = laden(b['url'], b['archiveFileName'])
        if pruefsumme(pfad) != b['checksum']:
            raise SystemExit(f'Prüfsumme von {name} stimmt nicht')
        ergebnis[name.replace(' ', '_')] = pfad
    return ergebnis


def ordner_ins_zip(aus, quelle, ziel_praefix):
    for wurzel, _, dateien in os.walk(quelle):
        for d in sorted(dateien):
            pfad = os.path.join(wurzel, d)
            rel = os.path.relpath(pfad, quelle).replace(os.sep, '/')
            info = zipfile.ZipInfo(ziel_praefix + rel, (2026, 1, 1, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = 0o644 << 16
            with open(pfad, 'rb') as f:
                aus.writestr(info, f.read())


def plattform_bauen(args, kern_zip):
    name = f'pbcplus-rp2040-{args.version}'
    ziel = os.path.join(DIST, name + '.zip')
    bibs = bibliotheken_laden()
    with zipfile.ZipFile(kern_zip) as ein, zipfile.ZipFile(ziel, 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as aus:
        # eigene Beispiele, Sensor-Bibliothek des Buchs und Adafruit-Bibliotheken
        for ordner in ('PicoBoyColorPlus', 'STK8BA58'):
            ordner_ins_zip(aus, os.path.join(WURZEL, 'bibliothek', ordner), f'{name}/libraries/{ordner}/')
        for ordner, zip_pfad in bibs.items():
            with zipfile.ZipFile(zip_pfad) as bz:
                oben = bz.namelist()[0].split('/')[0] + '/'
                for info in bz.infolist():
                    rel = info.filename[len(oben):]
                    if not rel or info.is_dir() or f'libraries/{ordner}/{rel}'.startswith(OHNE_BEISPIELE):
                        continue
                    neu = zipfile.ZipInfo(f'{name}/libraries/{ordner}/{rel}', info.date_time)
                    neu.compress_type = zipfile.ZIP_DEFLATED
                    neu.external_attr = 0o644 << 16
                    aus.writestr(neu, bz.read(info))
        oben = ein.namelist()[0].split('/')[0] + '/'
        for info in ein.infolist():
            rel = info.filename[len(oben):]
            teile = rel.split('/')
            if not rel or teile[0] in ('docs', 'tests'):
                continue
            if teile[0] == 'libraries' and len(teile) > 1 and teile[1] in OHNE_BIBLIOTHEKEN:
                continue
            if rel.startswith(OHNE_BEISPIELE):
                continue
            daten = None if info.is_dir() else ein.read(info)
            if rel == 'boards.txt':
                daten = boards_txt(daten.decode('utf-8')).encode('utf-8')
            elif rel == 'platform.txt':
                daten = platform_txt(daten.decode('utf-8'), args.version, args.kern).encode('utf-8')
            neu = zipfile.ZipInfo(name + '/' + rel, info.date_time)
            neu.external_attr = info.external_attr
            neu.compress_type = zipfile.ZIP_DEFLATED
            if info.is_dir():
                aus.writestr(neu, b'')
            else:
                aus.writestr(neu, daten)
        info = zipfile.ZipInfo(name + '/PBCPLUS-HINWEISE.md', (2026, 1, 1, 0, 0, 0))
        info.compress_type = zipfile.ZIP_DEFLATED
        info.external_attr = 0o644 << 16
        aus.writestr(info, hinweise(args.version, args.kern))
    return ziel


def ins_tar(tar, pfad, name, ausfuehrbar=False):
    info = tar.gettarinfo(pfad, name)
    if ausfuehrbar:
        info.mode |= 0o755
    if info.isreg():
        with open(pfad, 'rb') as f:
            tar.addfile(info, f)
    else:
        tar.addfile(info)


def werkzeug_bauen(args):
    """PBConPC als Arduino-Werkzeug: Windows (ZIP), Linux x86-64 (tar.gz), sonst nur der Hinweis."""
    v = args.pbconpc
    skript = os.path.join(WURZEL, 'werkzeug-pbconpc', 'pbconpc-start.sh')
    lizenz = os.path.join(WURZEL, 'werkzeug-pbconpc', 'LIZENZEN.md')
    starter = os.path.join(CACHE, 'pbconpc-start.exe')
    subprocess.run(['x86_64-w64-mingw32-gcc', '-municode', '-O2', '-s', '-Wall', '-o', starter,
                    os.path.join(WURZEL, 'werkzeug-pbconpc', 'pbconpc-start.c')], check=True)
    archive = {}

    # Windows: Inhalt der Release-ZIP von PBConPC in pbconpc/PBConPC/
    win_zip = laden(PBCONPC.format(v=v, datei='PBConPC-windows.zip'), f'PBConPC-{v}-windows.zip')
    ziel = os.path.join(DIST, f'pbconpc-{v}-windows.zip')
    with zipfile.ZipFile(win_zip) as ein, zipfile.ZipFile(ziel, 'w', zipfile.ZIP_DEFLATED) as aus:
        namen = [n for n in ein.namelist() if not n.endswith('/')]
        oben = os.path.commonprefix([n.split('/')[0] + '/' for n in namen]) if all('/' in n for n in namen) else ''
        for n in namen:
            aus.writestr('pbconpc/PBConPC/' + n[len(oben):], ein.read(n))
        aus.write(starter, 'pbconpc/pbconpc-start.exe')
        aus.write(lizenz, 'pbconpc/LIZENZEN.md')
    archive['windows'] = ziel

    # Linux x86-64: das AppImage entpackt (ohne FUSE lauffähig) in pbconpc/PBConPC/
    appimage = laden(PBCONPC.format(v=v, datei=f'PBConPC-{v}-x86_64.AppImage'), f'PBConPC-{v}-x86_64.AppImage')
    os.chmod(appimage, 0o755)
    entpackt = os.path.join(CACHE, f'appimage-{v}')
    if not os.path.isdir(entpackt):
        subprocess.run([appimage, '--appimage-extract'], cwd=CACHE, check=True, stdout=subprocess.DEVNULL)
        os.rename(os.path.join(CACHE, 'squashfs-root'), entpackt)
    ziel = os.path.join(DIST, f'pbconpc-{v}-x86_64-linux.tar.gz')
    with tarfile.open(ziel, 'w:gz') as tar:
        tar.add(entpackt, 'pbconpc/PBConPC')
        ins_tar(tar, skript, 'pbconpc/pbconpc-start.sh', ausfuehrbar=True)
        ins_tar(tar, lizenz, 'pbconpc/LIZENZEN.md')
    archive['linux'] = ziel

    # alle anderen Systeme: nur das Startskript, das auf die fehlende Fassung hinweist
    ziel = os.path.join(DIST, f'pbconpc-{v}-andere.tar.gz')
    with tarfile.open(ziel, 'w:gz') as tar:
        ins_tar(tar, skript, 'pbconpc/pbconpc-start.sh', ausfuehrbar=True)
        ins_tar(tar, lizenz, 'pbconpc/LIZENZEN.md')
    archive['andere'] = ziel
    return archive


def eintrag(host, pfad, basis):
    return {'host': host, 'url': f'{basis}/{os.path.basename(pfad)}', 'archiveFileName': os.path.basename(pfad),
            'checksum': pruefsumme(pfad), 'size': str(os.path.getsize(pfad))}


def main():
    a = argparse.ArgumentParser()
    a.add_argument('--version', required=True)
    a.add_argument('--kern', required=True, help='Version von Arduino-Pico, z. B. 6.4.0')
    a.add_argument('--pbconpc', required=True, help='Version von PBConPC, z. B. 1.1.0')
    a.add_argument('--basis-url', required=True, help='wo die Archive später liegen')
    args = a.parse_args()
    os.makedirs(DIST, exist_ok=True)

    earle = json.load(open(laden(EARLE_INDEX, 'package_rp2040_index.json'), encoding='utf-8'))['packages'][0]
    kern = next(p for p in earle['platforms'] if p['version'] == args.kern)
    kern_zip = laden(kern['url'], kern['archiveFileName'])
    if pruefsumme(kern_zip) != kern['checksum']:
        raise SystemExit('Prüfsumme des Kerns stimmt nicht')

    plattform = plattform_bauen(args, kern_zip)
    archive = werkzeug_bauen(args)

    # Earles Werkzeuge unverändert übernehmen, aber als Teil dieses Pakets
    werkzeuge = []
    abh = []
    for d in kern['toolsDependencies']:
        t = next(t for t in earle['tools'] if t['name'] == d['name'] and t['version'] == d['version'])
        werkzeuge.append(t)
        abh.append({'packager': 'pbcplus', 'name': d['name'], 'version': d['version']})
    hosts = sorted({s['host'] for t in werkzeuge for s in t['systems']})
    systeme = []
    for h in hosts:
        if h in WINDOWS:
            systeme.append(eintrag(h, archive['windows'], args.basis_url))
        elif h == LINUX64:
            systeme.append(eintrag(h, archive['linux'], args.basis_url))
        else:
            systeme.append(eintrag(h, archive['andere'], args.basis_url))
    werkzeuge.append({'name': 'pbconpc', 'version': args.pbconpc, 'systems': systeme})
    abh.append({'packager': 'pbcplus', 'name': 'pbconpc', 'version': args.pbconpc})

    index = {'packages': [{
        'name': 'pbcplus',
        'maintainer': 'Jan Schulz',
        'websiteURL': 'https://pbcplus.de/programmieren/arduino/',
        'help': {'online': 'https://pbcplus.de/programmieren/arduino/'},
        'platforms': [{
            'name': 'PicoBoy Color Plus',
            'architecture': 'rp2040',
            'version': args.version,
            'category': 'Contributed',
            'help': {'online': 'https://pbcplus.de/programmieren/arduino/'},
            'url': f'{args.basis_url}/{os.path.basename(plattform)}',
            'archiveFileName': os.path.basename(plattform),
            'checksum': pruefsumme(plattform),
            'size': str(os.path.getsize(plattform)),
            'boards': [{'name': 'PicoBoy Color Plus'}],
            'toolsDependencies': abh,
        }],
        'tools': werkzeuge,
    }]}
    pfad = os.path.join(DIST, 'package_pbcplus_index.json')
    json.dump(index, open(pfad, 'w', encoding='utf-8'), indent=2, ensure_ascii=False)
    for p in [plattform, *archive.values(), pfad]:
        print(f'{os.path.getsize(p) / 1e6:8.1f} MB  {os.path.basename(p)}')


if __name__ == '__main__':
    main()
