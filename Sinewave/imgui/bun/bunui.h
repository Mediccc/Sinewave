#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include <functional>
#include <windows.h>
#include <iostream>
#include "../modules/config.h"

namespace Bun {
    static int selectedPage;
    static int selectedTab;
    static bool waitingKeybind;
    static int keyb;
    static ImVec4 bg;

    void DarkTheme();

    bool Button(const char* name, const ImVec2& size = ImVec2(110, 35), const ImVec2& pos = ImGui::GetCursorScreenPos());
    void Checkbox();
    void Slider();
    void Section(const char* name, const ImVec2& size, std::function<void()> fn, const char* text = nullptr);
    bool Keybind(const char* name, int* key, const ImVec2& size);
    void NavigationBar(const char* name, const ImVec2& size);
    bool NavigationButton(const char* name, const ImVec2& size);

    /* we draw each rect for a page, and after that we position the cursor inside of the rect, then call the function that draws the content/elements of the page */
    void NavigationPage(const char* name, const ImVec2& size, std::function<void()> fn);

    void TabBar();
    void TabButton();
    void Notification(); /* todo: just create a new window, position it on the bottom right, and sum other stuff */
}