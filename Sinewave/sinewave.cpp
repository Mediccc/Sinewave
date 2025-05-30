#include <iostream>
#include "modules/bootstrapper.h"
#include "modules/main/gui.h"
#include "modules/main/launcher.h"
#include "modules/config.h"

/*

    thinking about it
    i could really clean some of my code
    and even make it better

*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    std::string version = Bootstrapper::init();

    checkFFlags();
    Config::loadConfig();

    if (config.multiRoblox) {
        CreateMutex(NULL, TRUE, L"ROBLOX_singletonMutex");
        Logger::log(Logger::SUCCESS, "Multi Roblox enabled!");
    }

    if (config.debugMode) {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        SetConsoleTitleA("Sinewave");
        Logger::log(Logger::SUCCESS, "Debug mode on!");
    }

    if (config.discordRPC) {
        DiscordEventHandlers handlers;
        memset(&handlers, 0, sizeof(handlers));

        handlers.ready = Discord::handleDiscordReady;
        handlers.disconnected = Discord::handleDiscordDisconnected;
        handlers.errored = Discord::handleDiscordError;
        handlers.joinGame = Discord::handleDiscordJoin;
        handlers.spectateGame = Discord::handleDiscordSpectate;
        handlers.joinRequest = Discord::handleDiscordJoinRequest;

        Discord_Initialize("1358195021362499734", &handlers, 1, "730");
        memset(&discordPresence, 0, sizeof(discordPresence));
    }

    if (strlen(lpCmdLine) > 0) {
        startLauncher(lpCmdLine, version);
    }
    else {
        checkVersion();
        launchGUI();
    }
}