#include "home.h"

void initHome() {
    if (ImGui::BeginTabItem("Home")) {
        ImGui::Text("Thank you for using Sinewave!");
        ImGui::Text("Please start games from the website. :)");

        ImGui::EndTabItem();
    }
}
