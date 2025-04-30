#include "settings.h"

void initSettings() {
    static bool showe;
    Bun::Section("Preferences", ImVec2(455, 124), []() {
        if (ImGui::BunCheckbox("Debug Mode", &config.debugMode)) {
            Config::saveConfig();
            MessageBoxA(NULL, "Please restart Sinewave to configure Debug Mode!", "Sinewave", MB_OK | MB_ICONINFORMATION);
        }

        ImGui::Spacing();

        if (ImGui::BunCheckbox("Create desktop shortcut on install/update", &config.createShortcut)) {
            Config::saveConfig();
        }

        ImGui::Spacing();

        if (Bun::Button("Themes")) {
            showe = true;
        }
    }, "Preferences");

    ImGui::Dummy(ImVec2(18, 18));

    /* todo: add custom themes */
    if (showe) {
        ImGui::SetNextWindowSize(ImVec2(300, 300));
        ImGui::Begin("Themes", &showe, ImGuiWindowFlags_NoResize);
        Bun::DarkTheme();

        /* colors */
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4 background = style.Colors[ImGuiCol_Button];

        /* drawing */
        ImDrawList* list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size(284, 258);

        list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::ColorConvertFloat4ToU32(background), 2.0f);

        if (Bun::NavigationButton("Default", ImVec2(264, 35))) {
            config.theme = "Default";
            Config::saveConfig();
        };
        ImGui::End();
    }

    Bun::Section("Other", ImVec2(455, 98), []() {
        if (Bun::Button("Fix Registry")) {
            setRobloxReg();
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
    }, "Other");

    ImGui::Dummy(ImVec2(20, 20));

    ImGui::Spacing();
}
