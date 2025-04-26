#include "settings.h"

void initSettings() {
    if (ImGui::BunCheckbox("Debug Mode", &config.debugMode)) {
        Config::saveConfig();
        MessageBoxA(NULL, "Please restart Sinewave to configure Debug Mode!", "Sinewave", MB_OK | MB_ICONINFORMATION);
    }

    ImGui::Spacing();

    if (ImGui::BunCheckbox("Create desktop shortcut on install/update", &config.createShortcut)) {
        Config::saveConfig();
    }

    ImGui::Spacing();

    if (Bun::Button("Fix Registry")) {
        setRobloxReg();
    }

    ImGui::Spacing();

    if (Bun::Button("Save Presets")) {
        /* get the path to the user's desktop */
        wchar_t* desktopPath;
        HRESULT hr = SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &desktopPath);

        /* if we successfully get the path, create a directory on the desktop and copy the contents of the presets directory to the new directory */
        if (SUCCEEDED(hr)) {
            std::filesystem::path desktop(desktopPath);
            std::filesystem::create_directory(desktop / "Sinewave Custom Presets");
            copyDirectoryContents(sinewave / "Settings" / "Presets", desktop / "Sinewave Custom Presets");

            CoTaskMemFree(desktopPath);
        }
        else {
            MessageBoxA(NULL, "Couldn't get desktop path!", "Sinewave", MB_OK | MB_ICONERROR);
        }
    }

    ImGui::Spacing();

    /* we run the roblox installer in order to replace the registry values, and after that we remove the Sinewave folder from AppData */
    /* maybe I should start commenting my code more */
    /* ok maybe instead of running the roblox installer i could just set the registry values myself */
    /* todo: only do that if someone complains about it for whatever reason */
    if (Bun::Button("Uninstall")) {
        int box = MessageBoxA(NULL, "Would you like to uninstall Sinewave?", "Sinewave", MB_OKCANCEL | MB_ICONQUESTION);
        if (box == IDOK) {
            if (std::filesystem::exists(sinewave)) {
                std::filesystem::path rbx = sinewave / "Roblox" / "RobloxPlayerInstaller.exe";

                /* basically just copy the functionality from bootstrapper.cpp */
                STARTUPINFO si;
                PROCESS_INFORMATION pi;

                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                ZeroMemory(&pi, sizeof(pi));

                if (CreateProcess(rbx.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                    WaitForSingleObject(pi.hProcess, INFINITE);

                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);

                    terminateProcess(L"RobloxPlayerBeta.exe");

                    if (std::filesystem::remove_all(sinewave)) {
                        MessageBoxA(NULL, "Uninstalled Sinewave! Running Sinewave again will make it reinstall itself.", "Sinewave", MB_OK | MB_ICONINFORMATION);
                        Logger::log(Logger::SUCCESS, "Uninstalled Sinewave!");
                    }
                    else {
                        MessageBoxA(NULL, "Couldn't delete the Sinewave folder from AppData!", "Sinewave", MB_OK | MB_ICONERROR);
                        Logger::log(Logger::ERR, "Failed to get the Sinewave directory from AppData.");
                    }
                }
                else {

                }
            }
        }
    }
}
