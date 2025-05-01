#include "watcher.h"

std::string Watcher::imageKey = "";
std::string Watcher::gameName = "";

std::string Watcher::getLogFile() {
    std::filesystem::path logs = robloxp / "Logs";
    std::filesystem::file_time_type latest;
    std::string file;

    for (auto f : std::filesystem::directory_iterator(logs)) {
        if (f.is_regular_file() && f.path().extension() == ".log") {
            auto time = std::filesystem::last_write_time(f);

            if (time > latest) {
                file = f.path().string();
                latest = time;
            }
        }
    }

    return file;
}

std::string Watcher::getServerId(const std::string& file) {
    std::string line;
    std::string id;

    std::regex rx(R"(\[FLog::Network\] serverId: ([\d\.]+)\|(\d+))");

    while (true) {
        std::ifstream ifs(file.c_str());

        while (std::getline(ifs, line)) {
            std::smatch matches;

            if (std::regex_search(line, matches, rx)) {
                id = matches[1];
                return id;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

std::string Watcher::getUniverseId(const std::string& file) {
    std::string line;
    std::string id;

    std::regex rx(R"(universeid:(\d+))");

    while (true) {
        std::ifstream ifs(file.c_str());

        while (std::getline(ifs, line)) {
            std::smatch matches;

            if (std::regex_search(line, matches, rx)) {
                id = matches[1];
                return id;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Watcher::init() {
    std::string first = getLogFile();

    while (watchingLogs) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::string current = getLogFile();

        if (current != first) {
            Logger::log(Logger::INFO, "Got latest log file!");
            std::this_thread::sleep_for(std::chrono::seconds(4));

            /* regex */
            serverIp = getServerId(current);
            std::string universeId = getUniverseId(current);

            if (config.discordRPC) {
                /* get the game's icon */
                std::string imageUrl = "https://thumbnails.roblox.com/v1/games/icons?universeIds=" + universeId + "&size=150x150&format=Png&isCircular=false";
                HttpResponse imageResponse = Http::newRequest(imageUrl, "GET");
                json img = json::parse(imageResponse.content);
                imageKey = img["data"][0]["imageUrl"].get<std::string>();

                /* get the game's name */
                std::string gameUrl = "https://games.roblox.com/v1/games?universeIds=" + universeId;
                HttpResponse gameResponse = Http::newRequest(gameUrl, "GET");
                json j = json::parse(gameResponse.content);
                gameName = j["data"][0]["name"].get<std::string>();
            }

            if (config.serverRegion) {
                /* get the server's location */
                std::string url = "http://ip-api.com/json/" + serverIp;
                HttpResponse res = Http::newRequest(url, "GET");
                json j = json::parse(res.content);
                std::string a = j["country"].get<std::string>();
                std::string b = j["city"].get<std::string>();
                std::string w = a + ", " + b;
                std::wstring wstr(w.begin(), w.end());
                sendNotification(L"Server Region", wstr);
            }

            watchingLogs = false;
        }
    }
}
