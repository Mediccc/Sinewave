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

/*

@author: ChatGPT

*/
void extractFile(const std::string& zipPath, const std::string& outputDir) {
    mz_zip_archive zip = {};
    if (!mz_zip_reader_init_file(&zip, zipPath.c_str(), 0)) {
        Logger::log(Logger::ERR, "Couldn't open zip file!");
        return;
    }

    int fileCount = (int)mz_zip_reader_get_num_files(&zip);
    for (int i = 0; i < fileCount; ++i) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip, i, &file_stat)) {
            continue;
        }

        std::string filePath = outputDir + "/" + file_stat.m_filename;

        if (mz_zip_reader_is_file_a_directory(&zip, i)) {
            std::filesystem::create_directories(filePath);
        }
        else {
            std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());

            size_t uncomp_size = 0;
            void* p = mz_zip_reader_extract_to_heap(&zip, i, &uncomp_size, 0);
            if (!p) {
                continue;
            }

            std::ofstream ofs(filePath, std::ios::binary);
            ofs.write((const char*)p, uncomp_size);
            ofs.close();

            mz_free(p);
        }
    }

    mz_zip_reader_end(&zip);
}

std::filesystem::path getDesktop() {
    wchar_t* desktopPath;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &desktopPath);
    std::filesystem::path p;

    if (SUCCEEDED(hr)) {
        p = desktopPath;
        CoTaskMemFree(desktopPath);
    }

    return p;
}

void checkVersion() {
    HttpResponse response = Http::newRequest("https://raw.githubusercontent.com/Mediccc/Sinewave/refs/heads/master/Sinewave/version.json", "GET");
    json j = json::parse(response.content);
    std::string version = j["version"].get<std::string>();

    std::ifstream ifs("version.json");
    json f = json::parse(ifs);
    std::string current = f["version"].get<std::string>();

    if (version != current) {
        int box = MessageBoxA(NULL, "It looks like you're using an outdated version of Sinewave.\nWould you like to install the new version?", "Sinewave", MB_YESNO | MB_ICONQUESTION);
        if (box == IDYES) {
            ShellExecute(0, 0, L"https://github.com/Mediccc/Sinewave/releases", 0, 0, SW_SHOW);
            /* i know deleting the Sinewave directory isn't the most convenient, but there's nothing to it really.. fflags will be removed soon anyways */
            /* so there's no point in saving the user's fflags presets here, there's a backup/save presets option for that lol */
            std::filesystem::path desktop = getDesktop();
            std::filesystem::path p = desktop / "Release.zip";
            std::filesystem::remove_all(sinewave);

            /* download and extract */
            Http::downloadFile("https://github.com/Mediccc/Sinewave/releases/download/v1.0.9/Sinewave.v1.0.9.zip", p.string());
            extractFile(p.string(), desktop.string());

            /* run new version */
            STARTUPINFOA si;
            PROCESS_INFORMATION pi;

            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&pi, sizeof(pi));

            std::string p2 = std::filesystem::path(desktop / "Release" / "Sinewave.exe").string();
            if (CreateProcessA(p2.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                ExitProcess(0);
            }
        }
    }
}

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