#pragma once
#include <iostream>
#include <regex>
#include <wintoastlib.h>
#include <filesystem>
#include <discord/discord_rpc.h>
#include <fstream>
#include <thread>
#include <mutex>
#include <nlohmann/json.hpp>
#include <TlHelp32.h>
#include "logger.h"
#include "http.h"
#include "watcher.h"
#include <miniz.h>

#pragma warning(disable: 4996)

using json = nlohmann::json;
using namespace WinToastLib;

extern char* appdata;
extern char* localAppdata;
extern std::filesystem::path sinewave;
extern std::filesystem::path fflags;
extern std::filesystem::path robloxp;
extern std::string serverIp;
extern std::atomic<bool> discordUpdate;
extern std::mutex discord;
extern DiscordRichPresence discordPresence;

class CustomHandler : public IWinToastHandler {
public:
    void toastActivated() const {
    }

    void toastActivated(int actionIndex) const {
    }

    void toastActivated(std::wstring response) const {
    }

    void toastDismissed(WinToastDismissalReason state) const {
        switch (state) {
        case UserCanceled:
            break;
        case TimedOut:
            break;
        case ApplicationHidden:
            break;
        default:
            break;
        }
    }

    void toastFailed() const {
        std::wcout << L"Error showing current toast" << std::endl;
        exit(5);
    }
};

std::filesystem::path getDesktop();

void extractFile(const std::string& zipPath, const std::string& outputDir);

void checkVersion();

void sendNotification(const std::wstring& title, const std::wstring& subtitle);

void loadConfig();

void saveConfig();

/* https://developer.valvesoftware.com/wiki/Implementing_Discord_RPC */
class Discord {
public:
    static void handleDiscordReady(const DiscordUser* connectedUser) {
        Logger::log(Logger::SUCCESS, "Connected to Discord!");
    }

    static void handleDiscordDisconnected(int errcode, const char* message) {
        Logger::log(Logger::ERR, "Discord disconnected!");
    }

    static void handleDiscordError(int errcode, const char* message) {
        Logger::log(Logger::ERR, "Discord error!");
    }

    static void handleDiscordJoin(const char* secret) {
    }

    static void handleDiscordSpectate(const char* secret) {
    }

    static void handleDiscordJoinRequest(const DiscordUser* request) {
    }
};

void updatePresence();

void copyDirectoryContents(const std::filesystem::path& from, const std::filesystem::path& to);
void deleteDirectoryContents(const std::filesystem::path& dir);

void setKey(HKEY hKey, const std::string& key, const std::string& valueName, const std::string& valueData);
void setRobloxReg();
bool terminateProcess(const wchar_t* processName);