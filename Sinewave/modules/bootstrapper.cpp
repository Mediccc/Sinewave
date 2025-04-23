#include "bootstrapper.h"

void Bootstrapper::initDirectories() {
	char* appdata = std::getenv("APPDATA");
	if (appdata) {
		auto path = std::filesystem::path(appdata) / "Sinewave";
		if (!std::filesystem::is_directory(path)) {
			std::filesystem::create_directory(path);
			std::filesystem::create_directory(path / "Roblox");
			std::filesystem::create_directory(path / "Settings");
			std::filesystem::create_directory(path / "Assets");
			std::filesystem::create_directory(path / "Assets" / "Cursors");
			std::filesystem::create_directory(path / "Assets" / "Cursors" / "2014");
			std::filesystem::create_directory(path / "Assets" / "Cursors" / "2006");
			
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

			setRobloxReg();

			auto ixp = std::filesystem::path(std::filesystem::path(localAppdata) / "Roblox" / "ClientSettings" / "IxpSettings.json").string();
			std::ofstream ofs(ixp);
			ofs << "{}";
			ofs.close();

			/* todo: make this look better */

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
				Http::downloadFile(base + c, std::filesystem::path(sinewave / "Assets" / "Cursors" / "2014" / c.c_str()).string());
			}

			Http::downloadFile("https://raw.githubusercontent.com/Mediccc/assets/refs/heads/main/sounds/ouch.ogg", std::filesystem::path(sinewave / "Assets" / "ouch.ogg").string());
			Http::downloadFile("https://raw.githubusercontent.com/Mediccc/assets/refs/heads/main/cursors/2006/ArrowCursor.png", std::filesystem::path(sinewave / "Assets" / "Cursors" / "2006" / "ArrowCursor.png").string());
			Http::downloadFile("https://raw.githubusercontent.com/Mediccc/assets/refs/heads/main/cursors/2006/ArrowFarCursor.png", std::filesystem::path(sinewave / "Assets" / "Cursors" / "2006" / "ArrowFarCursor.png").string());

			Logger::log(Logger::SUCCESS, "Finished installing assets!");

			if (config.createShortcut) {
				HRESULT hr = CoInitialize(NULL);
				if (!SUCCEEDED(hr)) {
					Logger::log(Logger::ERR, "Couldn't initialize the COM library!");
				}

				char exePath[MAX_PATH];
				GetModuleFileNameA(NULL, exePath, MAX_PATH);

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
