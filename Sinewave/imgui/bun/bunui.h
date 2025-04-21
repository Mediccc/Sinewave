#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include <functional>

namespace Bun {
    static int selectedPage;
    static int selectedTab;

    void DarkTheme();

    bool Button(const char* name, const ImVec2& size = ImVec2(110, 35), const ImVec2& pos = ImGui::GetCursorScreenPos());
    void Checkbox();
    void Slider();
    void Section();

    void NavigationBar(const char* name, const ImVec2& size);
    bool NavigationButton(const char* name, const ImVec2& size);

    /* we draw each rect for a page, and after that we position the cursor inside of the rect, then call the function that draws the content/elements of the page */
    void NavigationPage(const char* name, const ImVec2& size, std::function<void()> fn);

    void TabBar();
    void TabButton();
    void Notification(); /* todo: just create a new window, position it on the bottom right, and sum other stuff */
}