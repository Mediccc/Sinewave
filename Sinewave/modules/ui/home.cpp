#include "home.h"
void initHome() {
    ImGui::Spacing();
    ImGui::Text("Thank you for using Sinewave :)");
    ImGui::Spacing();
    ImGui::Text("Please start games from the Roblox website.");
    ImGui::Spacing();

    Bun::Section("Section", ImVec2(455, 98), []() {
        if (Bun::Button("Github Page")) {
            ShellExecute(0, 0, L"https://github.com/Mediccc/Sinewave", 0, 0, SW_SHOW);
        }

        ImGui::Spacing();

        if (Bun::Button("Roblox Site")) {
            ShellExecute(0, 0, L"https://www.roblox.com/", 0, 0, SW_SHOW);
        }
    });
    /*
    float samples[100];
    for (int n = 0; n < 100; n++)
        samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
    ImGui::PlotLines("##sinewave", samples, 100);

    ImGui::Dummy(ImVec2(5, 5));
    if (Bun::Button("Github Page")) {
        ShellExecute(0, 0, L"https://github.com/Mediccc/Sinewave", 0, 0, SW_SHOW);
    }

    ImGui::Spacing();

    if (Bun::Button("Roblox Site")) {
        ShellExecute(0, 0, L"https://www.roblox.com/", 0, 0, SW_SHOW);
    }

    ImGui::Spacing();*/
}
