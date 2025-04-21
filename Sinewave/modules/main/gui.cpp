#include "gui.h"

inline ImVec2 pageSize(472, 360);

/* helper function to handle our UI's pages */
void HandlePages() {
    if (Bun::selectedPage == 0) {
        ImGui::SetCursorPos(ImVec2(170, 34)); /* set the cursor pos for the page background */
        Bun::NavigationPage("HomePage", pageSize, []() {
            initHome();
        });
    }
    else if (Bun::selectedPage == 1) {
        ImGui::SetCursorPos(ImVec2(170, 34));
        Bun::NavigationPage("ModsPage", pageSize, []() {
            initMods();
        });
    }
    else if (Bun::selectedPage == 2) {
        ImGui::SetCursorPos(ImVec2(170, 34));
        Bun::NavigationPage("FFlagsPage", pageSize, []() {
            initFflags();
        });
    }
    else if (Bun::selectedPage == 3) {
        ImGui::SetCursorPos(ImVec2(170, 34));
        Bun::NavigationPage("SettingsPage", pageSize, []() {
            initSettings();
        });
    }
}

void NavigationExample() {
    /* set up the navigation bar and buttons */;
    Bun::NavigationBar("NavBar", ImVec2(150, 360));

    ImGui::Spacing();
    if (Bun::NavigationButton("Home", ImVec2(130, 35))) {
        Bun::selectedPage = 0;
    }

    ImGui::Spacing();

    if (Bun::NavigationButton("Mods", ImVec2(130, 35))) {
        Bun::selectedPage = 1;
    }

    ImGui::Spacing();

    if (Bun::NavigationButton("FFlags", ImVec2(130, 35))) {
        Bun::selectedPage = 2;
    }

    ImGui::Spacing();

    if (Bun::NavigationButton("Settings", ImVec2(130, 35))) {
        Bun::selectedPage = 3;
    }

    HandlePages();
}

void launchGUI() {
    GUI::initWindow();
    GUI::loadStyle();
    updateFFlags();

    // Main loop
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        static bool show = true;

        if (show) {
            ImGui::SetNextWindowPos({ 0, 0 });
            ImGui::SetNextWindowSize({ 650, 400 });

            ImGui::Begin("Sinewave | v1.0.2", &show, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            
            NavigationExample();

            ImGui::End();
        }
        else {
            exit(1);
        }

        GUI::render();
    }

    if (config.discordRPC) {
        discordUpdate = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Discord_Shutdown();
    }
    GUI::cleanup();
}