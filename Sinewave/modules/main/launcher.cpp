#include "launcher.h"

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

    WaitForSingleObject(pi.hProcess, INFINITE);
    //FreeConsole();
    ExitProcess(0);
}
