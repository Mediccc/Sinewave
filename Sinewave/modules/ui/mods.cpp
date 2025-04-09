#include "mods.h"

void initMods() {
    if (ImGui::BeginTabItem("Mods")) {

        if (ImGui::CollapsingHeader("Fun")) {
            if (ImGui::Checkbox("Discord Rich Presence", &config.discordRPC)) {
                Config::saveConfig();
            }

            if (ImGui::Checkbox("Multiple Roblox", &config.multiRoblox)) {
                Config::saveConfig();
            }
            
            ImGui::SameLine();
            ImGui::Text("(allows you to open multiple Roblox instances)");

            if (ImGui::Checkbox("Show Server Region", &config.serverRegion)) {
                Config::saveConfig();
            }
        }

        if (ImGui::CollapsingHeader("Customization")) {
            if (ImGui::Checkbox("Old Roblox Cursors", &config.oldCursors)) {
                Config::saveConfig();

                deleteDirectoryContents(sinewave / "Roblox" / "content" / "textures" / "Cursors" / "KeyboardMouse");
                std::filesystem::path src = config.oldCursors ? sinewave / "Assets" / "KeyboardMouse" : sinewave / "Roblox" / "ContentBackup" / "textures" / "Cursors" / "KeyboardMouse";
                std::filesystem::copy(src, std::filesystem::path(sinewave / "Roblox" / "content" / "textures" / "Cursors" / "KeyboardMouse"));
            }

            if (ImGui::Checkbox("Old Oof Sound", &config.oofSound)) {
                Config::saveConfig();

                std::filesystem::path src = config.oofSound ? sinewave / "Assets" / "ouch.ogg" : sinewave / "Roblox" / "ContentBackup" / "sounds" / "ouch.ogg";
                std::filesystem::remove(sinewave / "Roblox" / "content" / "sounds" / "ouch.ogg");
                std::filesystem::copy_file(src, std::filesystem::path(sinewave / "Roblox" / "content" / "sounds" / "ouch.ogg"));
            }
        }

        ImGui::EndTabItem();
    }
}
