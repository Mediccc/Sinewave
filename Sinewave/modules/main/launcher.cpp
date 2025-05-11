#include "launcher.h"
#include <windivert.h>

DWORD id;
bool freeze = false;
bool lag = false;
HANDLE wHandle;
WINDIVERT_ADDRESS addr;
char packet[65535];
UINT packetLen;

void initWDV() {
    std::string filter = "ip.DstAddr == " + serverIp + " || ip.SrcAddr == " + serverIp;
    wHandle = WinDivertOpen(filter.c_str(), WINDIVERT_LAYER_NETWORK, 0, 0);
    if (wHandle == INVALID_HANDLE_VALUE) {
        Logger::log(Logger::ERR, "Lag Switch Error: INVALID_HANDLE_VALUE\n");
        MessageBoxA(NULL, "INVALID_HANDLE_VALUE", "Sinewave", MB_OK | MB_ICONERROR);
        exit(1);
    }
}

/* i could also use ntsuspendprocess for this */
void suspend() {
    HANDLE snHandle = NULL;
    BOOL rvBool = FALSE;
    THREADENTRY32 te32 = { 0 };

    snHandle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snHandle == INVALID_HANDLE_VALUE) return;
    te32.dwSize = sizeof(THREADENTRY32);
    if (Thread32First(snHandle, &te32)) {
        do {
            if (te32.th32OwnerProcessID == id) {
                HANDLE h = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (h) {
                    if (!freeze) {
                        SuspendThread(h);
                    }
                    else {
                        ResumeThread(h);
                    }
                    CloseHandle(h);
                }
            }
        } while (Thread32Next(snHandle, &te32));
    }
}

void lagger() {
    while (true) {
        if (GetAsyncKeyState(config.packetKeybind) & 1) {
            lag = !lag;
            Logger::log(Logger::INFO, "Lag Switch is now " + std::string(lag ? "enabled" : "disabled") + "!");
        }

        if (!WinDivertRecv(wHandle, packet, sizeof(packet), &packetLen, &addr)) {
            Logger::log(Logger::ERR, "Packet receive error!");
            continue;
        }

        if (lag) {
            Sleep(100);
        }

        if (!WinDivertSend(wHandle, packet, packetLen, &packetLen, &addr)) {
            Logger::log(Logger::ERR, "Packet send error!");
            continue;
        }
    }
}

void freezer() {
    while (true) {
        if (GetAsyncKeyState(config.freezeKeybind) & 0x8000) {
            if (!freeze) {
                suspend();
                freeze = true;
            }
        }
        else {
            if (freeze) {
                suspend();
                freeze = false;
            }
        }
        Sleep(1);
    }
}

BOOL IsElevated() {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

void startLauncher(LPSTR cmd, const std::string& version) {
    json j;
    std::ifstream ifs(fflags);
    ifs >> j;
    ifs.close();

    std::ofstream ofs(robloxp / "ClientSettings" / "IxpSettings.json");
    ofs << j.dump(4);
    ofs.close();

    /* replace content directory */
    deleteDirectoryContents(robloxp / "Versions" / version.c_str() / "content");
    std::filesystem::copy(std::filesystem::path(sinewave / "Roblox" / "content"), std::filesystem::path(robloxp / "Versions" / version.c_str() / "content"), std::filesystem::copy_options::recursive);

    /* start watcher */
    std::thread logW(Watcher::init);
    logW.detach();

    /* start Roblox */
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    auto filePath = std::filesystem::path(std::filesystem::path(localAppdata) / "Roblox" / "Versions" / version.c_str() / "RobloxPlayerBeta.exe").string();

    CreateProcessA(filePath.c_str(), cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    Logger::log(Logger::SUCCESS, "Started Roblox!");
    while (watchingLogs == true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (config.discordRPC) {
        discordPresence.state = ("Playing " + Watcher::gameName).c_str();
        discordPresence.smallImageKey = "sinewave";
        discordPresence.largeImageKey = Watcher::imageKey.c_str();
        Discord_UpdatePresence(&discordPresence);

        std::thread rpc(updatePresence);
        rpc.detach();
    }

    if (config.enableFreeze) {
        HWND hwnd = FindWindowA(NULL, "Roblox");
        if (hwnd == NULL) {
            MessageBoxA(NULL, "Roblox window not found!", "Sinewave Freezer", MB_OK);
        }
        else {
            if (config.freezeKeybind != -1) {
                Logger::log(Logger::SUCCESS, "Roblox Freezer started!");
                GetWindowThreadProcessId(hwnd, &id);
                std::thread freezeRoblox(freezer);
                freezeRoblox.detach();
            }
            else {
                MessageBoxA(NULL, "It looks like you don't have a keybind set for the freeze switch!\n\nPlease click the button next to the freeze switch checkmark and then press any key to set the keybind!", "Sinewave", MB_OK | MB_ICONWARNING);
            }
        }
    }
    
    if (config.enablePacketLagger) {
        if (IsElevated()) {
            Logger::log(Logger::INFO, "Process is elevated");
            if (config.packetKeybind != -1) {
                initWDV();
                std::thread lagRoblox(lagger);
                lagRoblox.detach();
            }
            else {
                MessageBoxA(NULL, "It looks like you don't have a keybind set for the lag switch!\n\nPlease click the button next to the lag switch checkmark and then press any key to set the keybind!", "Sinewave", MB_OK | MB_ICONWARNING);
            }
        }
        else {
            MessageBoxA(NULL, "It looks like the lag switch is enabled, but Sinewave doesn't have admin permissions.\n\nGo to Sinewave.exe, right click Properties & then Compatiblity\n\nAfter that, turn on Run this program as administrator\n\nYou can keep playing, but the lag switch won't work.", "Sinewave", MB_OK | MB_ICONWARNING);
        }
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    //FreeConsole();
    //ExitProcess(0);
}
