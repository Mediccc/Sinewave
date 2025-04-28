#include "launcher.h"

DWORD id;
bool freeze = false;

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
            Logger::log(Logger::SUCCESS, "Roblox Freezer started!");
            GetWindowThreadProcessId(hwnd, &id);
            std::thread freezeRoblox(freezer);
            freezeRoblox.detach();
        }
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    //FreeConsole();
    ExitProcess(0);
}
