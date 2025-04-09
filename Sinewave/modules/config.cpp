#include "config.h"

Settings config;

void Config::loadConfig() {
    std::ifstream ifs(sinewave / "Settings" / "settings.json");
    json j = json::parse(ifs);
    config = j.get<Settings>();

    ifs.close();
}

void Config::saveConfig() {
    std::ofstream ofs(sinewave / "Settings" / "settings.json");
    json j = config;
    ofs << j;

    ofs.close();
}
