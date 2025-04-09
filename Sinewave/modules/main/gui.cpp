#include "gui.h"

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
            ImGui::SetNextWindowSize({ 700, 450 });

            ImGui::Begin("Sinewave | v1.0", &show, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            if (ImGui::BeginTabBar("Sinewave")) {
                initHome();
                initMods();
                initFflags();
                initSettings();

                ImGui::EndTabBar();
            }

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