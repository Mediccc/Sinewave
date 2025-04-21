#include "home.h"

void initHome() {
    ImGui::Text("Thank you for using Sinewave :)");
    ImGui::Spacing();
    ImGui::Text("Please start games from the Roblox website.");
    ImGui::Spacing();
    if (Bun::Button("Github Page")) {
        ShellExecute(0, 0, L"https://github.com/Mediccc/Sinewave", 0, 0, SW_SHOW);
    }
}
