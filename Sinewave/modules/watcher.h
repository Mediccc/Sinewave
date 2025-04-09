#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>
#include <regex>
#include "misc.h"
#include "config.h"

extern std::atomic<bool> watchingLogs;

class Watcher {
private:
	static std::string getLogFile();
	static std::string getServerId(const std::string& file);
	static std::string getUniverseId(const std::string& file);
public:
	static std::string imageKey;
	static std::string gameName;

	static void init();
};