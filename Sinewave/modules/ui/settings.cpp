#include "settings.h"

void initSettings() {
    if (ImGui::BeginTabItem("Settings")) {

        if (ImGui::Checkbox("Debug Mode", &config.debugMode)) {
            Config::saveConfig();
            MessageBoxA(NULL, "Please restart Sinewave to configure Debug Mode!", "Sinewave", MB_OK | MB_ICONINFORMATION);
        }

        if (ImGui::Checkbox("Create desktop shortcut on install/update", &config.createShortcut)) {
            Config::saveConfig();
        }
        ImGui::EndTabItem();
    }
}
