#include "bunui.h"

void Bun::DarkTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 4.0f;
    style.FrameRounding = 4.0f;

    style.Colors[ImGuiCol_Button] = ImVec4(0.196f, 0.196f, 0.196f, 0.400f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.235f, 0.235f, 0.235f, 1.000f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.273f, 0.274f, 0.275f, 1.000f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.196f, 0.196f, 0.196f, 0.400f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.235f, 0.235f, 0.235f, 1.000f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.273f, 0.274f, 0.275f, 1.000f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.803f, 0.833f, 0.869f, 1.000f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.137f, 0.137f, 0.137f, 1.000f);

}

bool Bun::Button(const char* name, const ImVec2& size, const ImVec2& pos) {
    ImGui::PushID(name);

    static std::unordered_map<ImGuiID, float> pMap; /* i added maps so we can have multiple buttons */
    static std::unordered_map<ImGuiID, float> cMap;

    ImGuiID id = ImGui::GetID(name);

    float& progress = pMap[id];
    float& click = cMap[id];

    /* animation & colors */
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO();
    ImVec4 background = style.Colors[ImGuiCol_Button];
    ImVec4 hover = style.Colors[ImGuiCol_ButtonHovered];
    ImVec4 active = style.Colors[ImGuiCol_ButtonActive];
    float hoverSpeed = 5.0f;
    float clickSpeed = 9.0f;

    /* drawing */
    ImDrawList* list = ImGui::GetWindowDrawList();
    //ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 tSize = ImGui::CalcTextSize(name);
    ImVec2 tPos = ImVec2(pos.x + (size.x - tSize.x) * 0.5f, pos.y + (size.y - tSize.y) * 0.5f);

    bool b = ImGui::InvisibleButton(name, size);
    bool hovered = ImGui::IsItemHovered();

    progress += (hovered ? 1.0f : -1.0f) * io.DeltaTime * hoverSpeed;
    progress = ImClamp(progress, 0.0f, 1.0f);

    if (b) click = 1.0f;
    if (click > 0.0f) {
        click -= io.DeltaTime * clickSpeed;
        if (click < 0.0f) click = 0.0f;
    }

    ImVec4 color = ImLerp(background, hover, progress);

    list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::ColorConvertFloat4ToU32(color), 2.0f);

    if (click > 0.0f) {
        list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::ColorConvertFloat4ToU32(active), 2.0f);
    }

    list->AddText(tPos, IM_COL32(255, 255, 255, 255), name);

    ImGui::PopID();

    return b;
}

void Bun::NavigationBar(const char* name, const ImVec2& size) {
    ImGui::PushID(name);

    /* colors */
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 background = style.Colors[ImGuiCol_Button]; /* just gonna use the button's color for the background, change to whatever you want tho */

    /* drawing */
    ImDrawList* list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();

    list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::ColorConvertFloat4ToU32(background), 2.0f);

    ImGui::PopID();
}

bool Bun::NavigationButton(const char* name, const ImVec2& size) {
    ImGui::PushID(name);

    ImDrawList* list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 p = ImVec2(pos.x + 10, pos.y + 10);
    bool b = Button(name, size, p);
    pos.y += 45; /* move cursor for the next button, feel free to tweak this if it's too much for your button's size */

    //ImGui::SetCursorScreenPos(pos); feel free to uncomment this if you want, i'm using imgui::spacing() rn
    ImGui::PopID();

    return b;
}

void Bun::NavigationPage(const char* name, const ImVec2& size, std::function<void()> fn) {
    ImGui::PushID(name);

    /* colors */
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 background = style.Colors[ImGuiCol_Button];

    /* drawing */
    ImDrawList* list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();

    list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::ColorConvertFloat4ToU32(background), 2.0f);
    ImGui::SetCursorScreenPos(ImVec2(pos.x + 10, pos.y + 10));
    ImGui::BeginGroup();
    fn();
    ImGui::EndGroup();
    ImGui::PopID();
}
