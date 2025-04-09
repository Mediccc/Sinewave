#include "bootstrapper.h"

static void setKey(HKEY hKey, const std::string& key, const std::string& valueName, const std::string& valueData) {
	HKEY h = nullptr;

	if (RegOpenKeyExA(hKey, key.c_str(), 0, KEY_SET_VALUE, &h) != ERROR_SUCCESS) {
		Logger::log(Logger::ERR, "Could not open registry key " + key);
		return;
	}

	LSTATUS status;
	if (valueName.empty()) {
		status = RegSetValueA(h, NULL, REG_SZ, valueData.c_str(), valueData.size() + 1);
	}
	else {
		status = RegSetValueExA(h, valueName.c_str(), 0, REG_SZ, (const BYTE*)valueData.c_str(), valueData.size() + 1);
	}

	if (status != ERROR_SUCCESS) {
		Logger::log(Logger::ERR, "Could not set registry key!");
		return;
	}
	else {
		Logger::log(Logger::SUCCESS, "Set registry key!");
	}

	RegCloseKey(h);
}

void Bootstrapper::initDirectories() {
	char* appdata = std::getenv("APPDATA");
	if (appdata) {
		auto path = std::filesystem::path(appdata) / "Sinewave";
		if (!std::filesystem::is_directory(path)) {
			std::filesystem::create_directory(path);
			std::filesystem::create_directory(path / "Roblox");
			std::filesystem::create_directory(path / "Settings");
			std::filesystem::create_directory(path / "Assets");
			std::filesystem::create_directory(path / "Assets" / "KeyboardMouse");
			
			std::ofstream ofs;
			ofs.open(path / "Settings" / "settings.json");
			if (ofs.is_open()) {
				config.discordRPC = true;
				config.createShortcut = true;
				json j = config;

				ofs << j;
				ofs.close();
			}

			ofs.open(path / "Settings" / "fflags.json");
			if (ofs.is_open()) {
				ofs << "{}";
				ofs.close();
			}

			Logger::log(Logger::SUCCESS, "Created directories!");
		}
	}
}

std::string Bootstrapper::init() {
	/* get the latest client version */
	HttpResponse version = Http::newRequest("https://clientsettingscdn.roblox.com/v1/client-version/WindowsPlayer", "GET");
	json j = json::parse(version.content);
	
	/* install Roblox */
	char* appdata = std::getenv("APPDATA");
	char* localAppdata = std::getenv("LOCALAPPDATA");
	auto path = std::filesystem::path(appdata) / "Sinewave";
	std::string currentVersion = j["clientVersionUpload"].get<std::string>();

	if (!std::filesystem::exists(std::filesystem::path(localAppdata) / "Roblox" / "Versions" / currentVersion) || !std::filesystem::exists(sinewave)) {
		if (!std::filesystem::exists(sinewave)) {
			Bootstrapper::initDirectories();
		}
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		Logger::log(Logger::INFO, "Fetched latest Roblox version! Installing update..");

		auto filePath = std::filesystem::path(path / "Roblox" / "RobloxPlayerInstaller.exe");
		Http::downloadFile("https://setup.rbxcdn.com/" + currentVersion + "-RobloxPlayerInstaller.exe", filePath.string());

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = FALSE;

		if (CreateProcess(filePath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
			WaitForSingleObject(pi.hProcess, INFINITE);

			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			system("taskkill /F /IM RobloxPlayerBeta.exe");

			/* set registry values */
			char exePath[MAX_PATH];
			GetModuleFileNameA(NULL, exePath, MAX_PATH);
			std::string pathh = exePath;

			setKey(HKEY_CURRENT_USER, "Software\\ROBLOX Corporation\\Environments\\roblox-player", "clientExe", pathh);
			setKey(HKEY_CURRENT_USER, "Software\\ROBLOX Corporation\\Environments\\roblox-player\\Capabilities", "ApplicationIcon", pathh);
			setKey(HKEY_CLASSES_ROOT, "roblox\\DefaultIcon", "", pathh);
			std::string launcherPath = "\"" + pathh + "\" \"%1\""; 
			setKey(HKEY_CLASSES_ROOT, "roblox-player\\shell\\open\\command", "", launcherPath);
			setKey(HKEY_CLASSES_ROOT, "roblox\\shell\\open\\command", "", launcherPath);
			setKey(HKEY_CLASSES_ROOT, "roblox-player\\DefaultIcon", "", pathh);

			auto ixp = std::filesystem::path(std::filesystem::path(localAppdata) / "Roblox" / "ClientSettings" / "IxpSettings.json").string();
			std::ofstream ofs(ixp);
			ofs << "{}";
			ofs.close();

			auto destination = std::filesystem::path(sinewave / "Roblox" / "content");
			if (!std::filesystem::exists(destination)) {
				std::filesystem::copy(std::filesystem::path(robloxp / "Versions" / currentVersion.c_str() / "content"), std::filesystem::path(sinewave / "Roblox" / "content"), std::filesystem::copy_options::recursive);
			}

			auto destination2 = std::filesystem::path(sinewave / "Roblox" / "ContentBackup");
			if (!std::filesystem::exists(destination2)) {
				std::filesystem::copy(std::filesystem::path(robloxp / "Versions" / currentVersion.c_str() / "content"), std::filesystem::path(sinewave / "Roblox" / "ContentBackup"), std::filesystem::copy_options::recursive);
			}

			Logger::log(Logger::SUCCESS, "Finished installing Roblox!");
			Logger::log(Logger::INFO, "Installing assets..");
			
			std::vector<std::string> cursors = { "ArrowCursor.png", "ArrowFarCursor.png", "IBeamCursor.png" };
			std::string base = "https://raw.githubusercontent.com/Mediccc/assets/main/cursors/";
			for (auto c : cursors) {
				Http::downloadFile(base + c, std::filesystem::path(sinewave / "Assets" / "KeyboardMouse" / c.c_str()).string());
			}

			Http::downloadFile("https://raw.githubusercontent.com/Mediccc/assets/refs/heads/main/sounds/ouch.ogg", std::filesystem::path(sinewave / "Assets" / "ouch.ogg").string());

			Logger::log(Logger::SUCCESS, "Finished installing assets!");

			if (config.createShortcut) {
				HRESULT hr = CoInitialize(NULL);
				if (!SUCCEEDED(hr)) {
					Logger::log(Logger::ERR, "Couldn't initialize the COM library!");
				}

				wchar_t* desktopPath;
				SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &desktopPath);

				std::filesystem::path desktop = desktopPath;
				std::filesystem::path shortcutPath = desktop / L"Sinewave.lnk";
				HRESULT result = CreateLink(
					std::filesystem::path(exePath).c_str(),
					shortcutPath.string().c_str(),
					L"Sinewave"
				);

				if (!SUCCEEDED(result)) {
					Logger::log(Logger::ERR, "Couldn't create shortcut!");
				}
			}

			MessageBoxA(NULL, "Finished installing Sinewave!", "Sinewave", MB_OK | MB_ICONINFORMATION);

			FreeConsole();
			ExitProcess(0);
		}
	}
	else {
		Logger::log(Logger::INFO, "Roblox is up to date!");
	}

	return currentVersion;
}
