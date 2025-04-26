#include "misc.h"

char* appdata = getenv("APPDATA");
char* localAppdata = getenv("LOCALAPPDATA");
std::filesystem::path sinewave = std::filesystem::path(appdata) / "Sinewave";
std::filesystem::path fflags = sinewave / "Settings" / "fflags.json";
std::filesystem::path robloxp = std::filesystem::path(localAppdata) / "Roblox";
std::atomic<bool> watchingLogs(true);
std::atomic<bool> discordUpdate(true);
std::mutex discord;
DiscordRichPresence discordPresence;

void sendNotification(const std::wstring& title, const std::wstring& subtitle) {
    if (!WinToast::isCompatible()) {
        std::wcerr << L"Error, your system is not supported!" << std::endl;
    }

    WinToast::instance()->setAppName(L"Sinewave");
    const auto aumi = WinToast::configureAUMI(L"SinewaveApp", L"Sinewave", L"SINEWAVE100", L"20250403");
    WinToast::instance()->setAppUserModelId(aumi);

    if (!WinToast::instance()->initialize()) {
        std::wcout << L"Error, could not initialize the lib!" << std::endl;
    }

    CustomHandler* handler = new CustomHandler;
    WinToastTemplate templ(WinToastTemplate::Text02);
    templ.setTextField(title, WinToastTemplate::FirstLine);
    templ.setTextField(subtitle, WinToastTemplate::SecondLine);
    const auto toast_id = WinToast::instance()->showToast(templ, handler);
    if (toast_id < 0) {
        std::wcout << L"Error: Could not launch your toast notification!" << std::endl;
    }
    else {
        std::wcout << L"Toast sent successfully with ID: " << toast_id << std::endl;
    }
}

void updatePresence() {
    while (discordUpdate) {
        {
            std::lock_guard<std::mutex> lock(discord);
            if (!Watcher::imageKey.empty()) {
                discordPresence.largeImageKey = Watcher::imageKey.c_str();
                discordPresence.state = ("Playing " + Watcher::gameName).c_str();
            }
            Discord_UpdatePresence(&discordPresence);
        }
        std::this_thread::sleep_for(std::chrono::seconds(15));
    }
}

void copyDirectoryContents(const std::filesystem::path& from, const std::filesystem::path& to) {
    for (const auto& entry : std::filesystem::directory_iterator(from)) {
        const auto& src = entry.path();
        const auto dest = to / src.filename();

        if (entry.is_regular_file()) {
            std::filesystem::copy_file(src, dest, std::filesystem::copy_options::overwrite_existing);
        }
    }
}

void deleteDirectoryContents(const std::filesystem::path& dir) {
    for (const auto& entry : std::filesystem::directory_iterator(dir))
        std::filesystem::remove_all(entry.path());
}

void setKey(HKEY hKey, const std::string& key, const std::string& valueName, const std::string& valueData) {
    HKEY h = nullptr;

    if (RegOpenKeyExA(hKey, key.c_str(), 0, KEY_SET_VALUE, &h) != ERROR_SUCCESS) {
        Logger::log(Logger::ERR, "Could not open registry key " + key);
        return;
    }

    LSTATUS status;
    if (valueName.empty()) {
        status = RegSetValueA(h, NULL, REG_SZ, valueData.c_str(), valueData.size() + 1);
    }
    else {
        status = RegSetValueExA(h, valueName.c_str(), 0, REG_SZ, (const BYTE*)valueData.c_str(), valueData.size() + 1);
    }

    if (status != ERROR_SUCCESS) {
        Logger::log(Logger::ERR, "Could not set registry key!");
        return;
    }
    else {
        Logger::log(Logger::SUCCESS, "Set registry key!");
    }

    RegCloseKey(h);
}

void setRobloxReg() {
    /* set registry values */
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string pathh = exePath;

    setKey(HKEY_CURRENT_USER, "Software\\ROBLOX Corporation\\Environments\\roblox-player", "clientExe", pathh);
    setKey(HKEY_CURRENT_USER, "Software\\ROBLOX Corporation\\Environments\\roblox-player\\Capabilities", "ApplicationIcon", pathh);
    setKey(HKEY_CLASSES_ROOT, "roblox\\DefaultIcon", "", pathh);
    std::string launcherPath = "\"" + pathh + "\" \"%1\"";
    setKey(HKEY_CLASSES_ROOT, "roblox-player\\shell\\open\\command", "", launcherPath);
    setKey(HKEY_CLASSES_ROOT, "roblox\\shell\\open\\command", "", launcherPath);
    setKey(HKEY_CLASSES_ROOT, "roblox-player\\DefaultIcon", "", pathh);
}

/* this is better than using system */
bool terminateProcess(const wchar_t* processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;

    PROCESSENTRY32W entry{ sizeof(entry) };
    bool terminated = false;

    for (BOOL ok = Process32FirstW(snapshot, &entry); ok; ok = Process32NextW(snapshot, &entry)) {
        if (_wcsicmp(entry.szExeFile, processName) == 0) {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
            if (hProcess) {
                if (TerminateProcess(hProcess, 0))
                    terminated = true;
                CloseHandle(hProcess);
            }
        }
    }

    CloseHandle(snapshot);
    return terminated;
}