#!/bin/sh
# Startet einen Sketch im Emulator PBConPC (Upload-Methode „PBConPC (Emulator)“).
# Linux und macOS. Ein vorher auf diesem Weg gestartetes PBConPC wird zuerst
# beendet; PBConPC läuft danach unabhängig weiter, die IDE wartet nicht darauf.
uf2="$1"
hier="$(cd "$(dirname "$0")" && pwd)"
if [ -x "$hier/PBConPC/AppRun" ]; then
    programm="$hier/PBConPC/AppRun"          # mitgeliefert (Linux, x86-64)
elif command -v pbconpc >/dev/null 2>&1; then
    programm="$(command -v pbconpc)"         # selbst installiert, z. B. als .deb
else
    echo "PBConPC gibt es für diesen Rechner noch nicht oder es wurde nicht gefunden." >&2
    echo "Mehr dazu: https://pbcplus.de/geraet/am-pc/" >&2
    exit 1
fi
merker="${TMPDIR:-/tmp}/pbcplus-pbconpc-$(id -u).pid"
if [ -f "$merker" ]; then
    alt="$(cat "$merker")"
    if [ -n "$alt" ] && [ -r "/proc/$alt/cmdline" ] && tr '\0' ' ' < "/proc/$alt/cmdline" | grep -q -- "--flash none"; then
        kill "$alt" 2>/dev/null
    fi
fi
nohup "$programm" --flash none "$uf2" >/dev/null 2>&1 &
echo $! > "$merker"
echo "PBConPC gestartet."
