#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "misc.h"

using json = nlohmann::json;

struct Settings {
	bool multiRoblox;
	bool discordRPC;
	bool debugMode;
	bool serverRegion;
	bool oldCursors;
	bool old2006Cursors;
	bool createShortcut;
	bool oofSound;
};

inline void to_json(json& j, const Settings& s) {
	j = json{
		{"MultiRoblox", s.multiRoblox},
		{"DiscordRPC", s.discordRPC},
		{"DebugMode", s.debugMode},
		{"ServerRegion", s.serverRegion},
		{"OldCursors", s.oldCursors},
		{"Old2006Cursors", s.old2006Cursors},
		{"CreateShortcut", s.createShortcut},
		{"OofSound", s.oofSound}
	};
}

inline void from_json(const json& j, Settings& s) {
	j.at("MultiRoblox").get_to(s.multiRoblox);
	j.at("DiscordRPC").get_to(s.discordRPC);
	j.at("DebugMode").get_to(s.debugMode);
	j.at("ServerRegion").get_to(s.serverRegion);
	j.at("OldCursors").get_to(s.oldCursors);
	j.at("Old2006Cursors").get_to(s.old2006Cursors);
	j.at("CreateShortcut").get_to(s.createShortcut);
	j.at("OofSound").get_to(s.oofSound);
}

extern Settings config;

class Config
{
public:
	static void loadConfig();

	static void saveConfig();
};

