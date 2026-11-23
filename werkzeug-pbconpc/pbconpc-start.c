// pbconpc-start.exe: startet einen Sketch im Emulator PBConPC (Windows).
// Aufruf durch die Arduino IDE bei der Upload-Methode „PBConPC (Emulator)“:
//     pbconpc-start.exe DATEI.uf2
// Ein vorher auf diesem Weg gestartetes PBConPC wird zuerst beendet, damit
// sich keine Fenster stapeln. PBConPC läuft danach unabhängig weiter; die IDE
// wartet nicht darauf.
//
// Übersetzen: x86_64-w64-mingw32-gcc -municode -O2 -s -o pbconpc-start.exe pbconpc-start.c
#include <windows.h>
#include <stdio.h>
#include <wchar.h>

int wmain(int argc, wchar_t **argv) {
    if (argc < 2) {
        fprintf(stderr, "Aufruf: pbconpc-start DATEI.uf2\n");
        return 2;
    }

    // PBConPC liegt neben diesem Programm im Ordner PBConPC
    wchar_t ordner[MAX_PATH], exe[MAX_PATH];
    GetModuleFileNameW(NULL, ordner, MAX_PATH);
    wchar_t *strich = wcsrchr(ordner, L'\\');
    if (strich) *strich = 0;
    swprintf(exe, MAX_PATH, L"%ls\\PBConPC\\pbconpc.exe", ordner);
    if (GetFileAttributesW(exe) == INVALID_FILE_ATTRIBUTES) {
        fprintf(stderr, "PBConPC wurde nicht gefunden. Download: https://pbcplus.de/geraet/am-pc/\n");
        return 1;
    }

    // ein vorher hiermit gestartetes PBConPC beenden (nur genau dieses Programm)
    wchar_t temp[MAX_PATH], merker[MAX_PATH];
    GetTempPathW(MAX_PATH, temp);
    swprintf(merker, MAX_PATH, L"%lspbcplus-pbconpc.pid", temp);
    FILE *f = _wfopen(merker, L"r");
    if (f) {
        unsigned long alt = 0;
        if (fscanf(f, "%lu", &alt) == 1 && alt) {
            HANDLE h = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD)alt);
            if (h) {
                wchar_t name[MAX_PATH];
                DWORD n = MAX_PATH;
                if (QueryFullProcessImageNameW(h, 0, name, &n) && _wcsicmp(name, exe) == 0) {
                    TerminateProcess(h, 0);
                    WaitForSingleObject(h, 2000);
                }
                CloseHandle(h);
            }
        }
        fclose(f);
    }

    // PBConPC unabhängig von der IDE starten (keine geerbten Handles)
    wchar_t befehl[4 * MAX_PATH];
    swprintf(befehl, 4 * MAX_PATH, L"\"%ls\" --flash none \"%ls\"", exe, argv[1]);
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof si);
    si.cb = sizeof si;
    if (!CreateProcessW(exe, befehl, NULL, NULL, FALSE, DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP,
                        NULL, ordner, &si, &pi)) {
        fprintf(stderr, "PBConPC konnte nicht gestartet werden (Fehler %lu).\n", (unsigned long)GetLastError());
        return 1;
    }
    f = _wfopen(merker, L"w");
    if (f) {
        fprintf(f, "%lu", (unsigned long)pi.dwProcessId);
        fclose(f);
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    printf("PBConPC gestartet.\n");
    return 0;
}
