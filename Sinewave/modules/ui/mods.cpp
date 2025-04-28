#include "mods.h"

void initMods() {
    Bun::Section("Misc", ImVec2(455, 115), []() {
        if (ImGui::BunCheckbox("Discord Rich Presence", &config.discordRPC)) {
            Config::saveConfig();
        }

        ImGui::Spacing();

        if (ImGui::BunCheckbox("Multi Roblox", &config.multiRoblox)) {
            Config::saveConfig();
        }

        ImGui::Spacing();

        if (ImGui::BunCheckbox("Show Server Region", &config.serverRegion)) {
            Config::saveConfig();
        }

        ImGui::Spacing();

        if (ImGui::BunCheckbox("Enable Client Freezing (Hold to freeze!)", &config.enableFreeze)) {
            MessageBoxA(NULL, "I don't know if this makes Hyperion mad or not.\nI am not responsible for any banned accounts.", "Sinewave", MB_OK | MB_ICONWARNING);
            Config::saveConfig();
        }

        ImGui::SameLine();

    }, "Misc");

    ImGui::Dummy(ImVec2(18, 18));

    Bun::Section("Customization", ImVec2(455, 115), []() {
        if (ImGui::BunCheckbox("2014 Cursors", &config.oldCursors)) {
            Config::saveConfig();

            deleteDirectoryContents(sinewave / "Roblox" / "content" / "textures" / "Cursors" / "KeyboardMouse");
            std::filesystem::path src = config.oldCursors ? sinewave / "Assets" / "Cursors" / "2014" : sinewave / "Roblox" / "ContentBackup" / "textures" / "Cursors" / "KeyboardMouse";
            copyDirectoryContents(src, std::filesystem::path(sinewave / "Roblox" / "content" / "textures" / "Cursors" / "KeyboardMouse"));
        }

        ImGui::Spacing();

        if (ImGui::BunCheckbox("2006 Cursors", &config.old2006Cursors)) {
            Config::saveConfig();

            deleteDirectoryContents(sinewave / "Roblox" / "content" / "textures" / "Cursors" / "KeyboardMouse");
            std::filesystem::path src = config.old2006Cursors ? sinewave / "Assets" / "Cursors" / "2006" : sinewave / "Roblox" / "ContentBackup" / "textures" / "Cursors" / "KeyboardMouse";
            copyDirectoryContents(src, std::filesystem::path(sinewave / "Roblox" / "content" / "textures" / "Cursors" / "KeyboardMouse"));
        }

        ImGui::Spacing();

        if (ImGui::BunCheckbox("Old Oof Sound", &config.oofSound)) {
            Config::saveConfig();

            std::filesystem::path src = config.oofSound ? sinewave / "Assets" / "ouch.ogg" : sinewave / "Roblox" / "ContentBackup" / "sounds" / "ouch.ogg";
            std::filesystem::remove(sinewave / "Roblox" / "content" / "sounds" / "ouch.ogg");
            std::filesystem::copy_file(src, std::filesystem::path(sinewave / "Roblox" / "content" / "sounds" / "ouch.ogg"));
        }
    }, "Mods");
}
