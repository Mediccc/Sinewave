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
}
