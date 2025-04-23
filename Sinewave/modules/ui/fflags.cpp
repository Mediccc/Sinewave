#include "fflags.h"

std::vector<FFlag> flags;
std::string fflag;
std::string fflagValue;

void setFflag(const std::string& fflag, const std::string& value) {
    json j;
    std::ifstream ifs(fflags);
    ifs >> j;
    ifs.close();

    j[fflag] = value;
    std::ofstream ofs(fflags);
    ofs << j.dump(4);
    ofs.close();
}

void removeFflag(const std::string& fflag) {
    json j;
    std::ifstream ifs(fflags);
    ifs >> j;
    ifs.close();

    j.erase(fflag);
    std::ofstream ofs(fflags);
    ofs << j.dump(4);
    ofs.close();
}

void updateFFlags() {
    std::ifstream ifs(fflags);

    json j = json::parse(ifs);
    for (auto& [key, value] : j.items()) {
        bool exists = false;
        for (const auto& flag : flags) {
            if (flag.name == key) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            if (value.is_string()) {
                flags.push_back(FFlag{ key, value.get<std::string>() });
            }
            else if (value.is_number_integer()) {
                flags.push_back(FFlag{ key, std::to_string(value.get<int>()) });
            }
            else if (value.is_number_float()) {
                flags.push_back(FFlag{ key, std::to_string(value.get<float>()) });
            }
            else if (value.is_boolean()) {
                flags.push_back(FFlag{ key, value.get<bool>() ? "true" : "false" });
            }
            else {
                flags.push_back(FFlag{ key, "Unsupported value type" });
            }
        }
    }
}

std::string openFileDialog() {
    std::string path;

    /* initialize COM */
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileOpenDialog* pFileOpen;

        /* create the FileOpenDialog object */
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr)) {
            /* show the Open dialog box */
            hr = pFileOpen->Show(NULL);

            /* get the file name from the dialog box */
            if (SUCCEEDED(hr)) {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);

                if (SUCCEEDED(hr)) {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    /* display the file name to the user */
                    if (SUCCEEDED(hr))
                    {
                        /* convert std::wstring to std::string */
                        std::wstring ws(pszFilePath);
                        path = std::string(ws.begin(), ws.end());

                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }

    return path;
}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void initFflags() {
    ImGui::SetNextItemWidth(250);
    ImGui::InputText("FFlag", &fflag);
    ImGui::Spacing();
    ImGui::SetNextItemWidth(250);
    ImGui::InputText("FFlag Value", &fflagValue);

    ImGui::Spacing();

    if (Bun::Button("Add")) {
        setFflag(fflag, fflagValue);
        updateFFlags();
    }

    ImGui::SameLine();

    if (Bun::Button("Remove")) {
        removeFflag(fflag);

        for (auto it = flags.begin(); it != flags.end(); ++it) {
            if (it->name == fflag) {
                flags.erase(it);
                break;
            }
        }
    }

    ImGui::SameLine();

    if (Bun::Button("Import")) {
        std::string selected = openFileDialog();

        if (!selected.empty()) {
            std::filesystem::path path = selected;
            std::filesystem::remove_all(fflags);
            std::filesystem::copy_file(path, fflags);
            std::rename(selected.c_str(), "fflags.json"); /* i forgot to add this */
            updateFFlags();
        }
    }

    ImGui::Spacing();

    if (Bun::Button("Reset File")) {
        std::ofstream ofs(fflags);
        ofs << "{}";
        ofs.close();

        flags.clear();
        updateFFlags();
    }

    ImGui::SameLine();
    
    if (Bun::Button("Open File")) {
        ShellExecute(0, L"open", fflags.c_str(), 0, 0, SW_SHOW);
    }

    ImGui::Spacing();

    static bool showe;
    ImGui::BunCheckbox("Show FFlag Presets Window", &showe);
    if (showe) {
        ImGui::SetNextWindowSize(ImVec2(300, 300));
        ImGui::Begin("FFlag Presets", &showe, ImGuiWindowFlags_NoResize);
        Bun::DarkTheme();

        /* colors */
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4 background = style.Colors[ImGuiCol_Button];

        /* drawing */
        ImDrawList* list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size(284, 258);

        list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::ColorConvertFloat4ToU32(background), 2.0f);
        if (Bun::NavigationButton("Disable Roblox Telemetry", ImVec2(264, 35))) {
            std::vector<std::string> telemetry = {"FFlagDebugDisableTelemetryEphemeralCounter", "FFlagDebugDisableTelemetryEphemeralStat", "FFlagDebugDisableTelemetryEventIngest", "FFlagDebugDisableTelemetryPoint", "FFlagDebugDisableTelemetryV2Counter", "FFlagDebugDisableTelemetryV2Event", "FFlagDebugDisableTelemetryV2Stat"};
            for (auto flag : telemetry) {
                setFflag(flag, "True");
                updateFFlags();
            }
        };

        ImGui::Spacing();

        if (Bun::NavigationButton("Disable Player Shadows", ImVec2(264, 35))) {
            setFflag("FIntRenderShadowIntensity", "0");
            updateFFlags();
        }
        ImGui::End();
    }
}