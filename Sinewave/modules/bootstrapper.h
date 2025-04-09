#pragma once
#pragma warning(disable: 4996)
#include <iostream>
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <chrono>
#include <thread>
#include <tlhelp32.h>
#include <tchar.h>
#include "http.h"
#include "logger.h"
#include "misc.h"
#include "config.h"
#include <ShortcutProvider.h>

using json = nlohmann::json;

class Bootstrapper
{
public:
	static void initDirectories();
	static std::string init();
};

