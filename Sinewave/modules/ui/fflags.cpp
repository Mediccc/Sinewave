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
            flags.push_back(FFlag{ key, value.get<std::string>() });
        }
    }
}

void initFflags() {
    if (ImGui::BeginTabItem("FFlags")) {
        ImGui::InputText("FFlag", &fflag);
        ImGui::InputText("FFlag Value", &fflagValue);
        if (ImGui::Button("Add FFlag")) {
            setFflag(fflag, fflagValue);
            updateFFlags();
        }

        if (ImGui::Button("Reset FFlag File")) {
            std::ofstream ofs(fflags);
            ofs << "{}";
            ofs.close();

            flags.clear();
            updateFFlags();
        }

        if (ImGui::BeginTable("Flags", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < flags.size(); i++) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText(("##name" + std::to_string(i)).c_str(), &flags[i].name);

                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText(("##value" + std::to_string(i)).c_str(), &flags[i].value);
            }

            ImGui::EndTable();
        }

        if (ImGui::Button("Save Modified List")) {
            json j;

            for (auto flag : flags) {
                j[flag.name] = flag.value;
            }

            std::ofstream ofs(fflags);
            ofs << j.dump(4);
            ofs.close();
        }

        ImGui::SameLine();
        ImGui::Text("(you only need to click this if you modified the FFlag list)");

        ImGui::EndTabItem();
    }
}