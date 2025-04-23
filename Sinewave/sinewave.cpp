#include <iostream>
#include "modules/bootstrapper.h"
#include "modules/main/gui.h"
#include "modules/main/launcher.h"
#include "modules/config.h"

/*

todo:
    - probably make a custom ImGui widget for a table/list
    - I would just use an ImGui table but I don't like the look of them + they get out of my frames
    - fflag presets
    - better fflag editor
    - better discord rpc (allowing game developers to change things like game name etc for the rpc)
    - channel switcher
    - option to cancel updates
    - i wanna make a forcefield color switcher i just like the idea of it

*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    SetConsoleTitleA("Sinewave");
    std::string version = Bootstrapper::init();

    Config::loadConfig();

    if (config.multiRoblox) {
        CreateMutex(NULL, TRUE, L"ROBLOX_singletonMutex");
        Logger::log(Logger::SUCCESS, "Multi Roblox enabled!");
    }

    if (config.debugMode) {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
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
        launchGUI();
    }
}