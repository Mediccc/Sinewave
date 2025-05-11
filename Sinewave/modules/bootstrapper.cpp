#include "bootstrapper.h"

std::istringstream manifest;
std::vector<std::string> packagesV;

/* this is hardcoded */
std::unordered_map<std::string, std::string> packages = {
	{"RobloxApp.zip", ""},
	{"redist.zip", ""},
	{"shaders.zip", "shaders/"},
	{"ssl.zip", "ssl/"},
	{"WebView2.zip", ""},
	{"WebView2RuntimeInstaller.zip", "WebView2RuntimeInstaller/"},

	{"content-avatar.zip", "content/avatar/"},
	{"content-configs.zip", "content/configs/"},
	{"content-fonts.zip", "content/fonts/"},
	{"content-sky.zip", "content/sky/"},
	{"content-sounds.zip", "content/sounds/"},
	{"content-textures2.zip", "content/textures/"},
	{"content-models.zip", "content/models/"},

	{"content-textures3.zip", "PlatformContent/pc/textures/"},
	{"content-terrain.zip", "PlatformContent/pc/terrain/"},
	{"content-platform-fonts.zip", "PlatformContent/pc/fonts/"},

	{"extracontent-luapackages.zip", "ExtraContent/LuaPackages/"},
	{"extracontent-translations.zip", "ExtraContent/translations/"},
	{"extracontent-models.zip", "ExtraContent/models/"},
	{"extracontent-textures.zip", "ExtraContent/textures/"},
	{"extracontent-places.zip", "ExtraContent/places/"}
};

void Bootstrapper::initDirectories() {
	char* appdata = std::getenv("APPDATA");
	if (appdata) {
		auto path = std::filesystem::path(appdata) / "Sinewave";
		if (!std::filesystem::is_directory(path)) {
			std::filesystem::create_directory(path);
			std::filesystem::create_directory(path / "Roblox");
			std::filesystem::create_directory(path / "Settings");
			std::filesystem::create_directory(path / "Settings" / "Presets");
			std::filesystem::create_directory(path / "Assets");
			std::filesystem::create_directory(path / "Assets" / "Cursors");
			std::filesystem::create_directory(path / "Assets" / "Cursors" / "2014");
			std::filesystem::create_directory(path / "Assets" / "Cursors" / "2006");
			
			std::ofstream ofs;
			ofs.open(path / "Settings" / "settings.json");
			if (ofs.is_open()) {
				config.discordRPC = true;
				config.createShortcut = true;
				config.freezeKeybind = -1;
				config.packetKeybind = -1;
				config.theme = "Default";
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

void fetchPackages() {
	std::string line;

	while (std::getline(manifest, line)) {
		if (line.ends_with(".zip")) {
			packagesV.push_back(line);
		}
	}
}

std::string Bootstrapper::init() {
	/* get the latest client version */
	HttpResponse version = Http::newRequest("https://clientsettingscdn.roblox.com/v1/client-version/WindowsPlayer", "GET");
	json j = json::parse(version.content);

	std::string currentVersion = j["clientVersionUpload"].get<std::string>();

	/* we assume the user already has roblox installed */
	if (!std::filesystem::exists(robloxp /  "Versions" / currentVersion) || !std::filesystem::exists(sinewave)) {
		/* init sinewave */
		Logger::log(Logger::INFO, "Fetched latest Roblox version! Installing update..");

		if (!std::filesystem::exists(sinewave)) {
			Bootstrapper::initDirectories();
		}

		deleteDirectoryContents(robloxp / "Versions");

		AllocConsole();
		SetConsoleTitleA("Sinewave Bootstrapper");
		freopen("CONOUT$", "w", stdout);

		/* download roblox */
		std::string base = "https://setup.rbxcdn.com/" + currentVersion;

		/*
		HttpResponse manifestRes = Http::newRequest(base + "-rbxPkgManifest.txt", "GET");
		manifest.str(manifestRes.content);
		fetchPackages();
		Logger::log(Logger::SUCCESS, "Fetched the manifest file!");*/

		/* create version directory in the roblox versions folder */
		std::filesystem::create_directory(robloxp / "Versions" / currentVersion);
		std::filesystem::path ver = robloxp / "Versions" / currentVersion;

		/* download each package, create the directory for it and extract it */
		for (auto it : packages) {
			std::string url = base + "-" + it.first;

			std::filesystem::create_directories(ver);

			Http::downloadFile(url, std::filesystem::path(ver / it.first).string());
			extractFile((ver / it.first).string(), (ver / it.second).string());

			std::filesystem::remove_all(ver / it.first);

			Logger::log(Logger::SUCCESS, "Finished downloading " + it.first + "!");
		}

		/* create AppSettings.xml file */
		std::ofstream ofs(std::filesystem::path(ver / "AppSettings.xml"));
		ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                  "<Settings>\n"
                  "    <ContentFolder>content</ContentFolder>\n"
                  "    <BaseUrl>http://www.roblox.com</BaseUrl>\n"
			"</Settings>";

		Logger::log(Logger::SUCCESS, "Wrote AppSettings.xml file!");
		ofs.close();

		/* set registry */
		setRobloxReg();

		/* write to the roblox's fflags file */
		auto ixp = std::filesystem::path(robloxp / "ClientSettings" / "IxpSettings.json").string();
		std::ofstream ofss(ixp);
		ofss << "{}";
		ofss.close();

		Logger::log(Logger::SUCCESS, "Wrote to IxpSettings.json file!");

		/* copy the content folder */
		auto destination = std::filesystem::path(sinewave / "Roblox" / "content");
		if (!std::filesystem::exists(destination)) {
			std::filesystem::copy(std::filesystem::path(robloxp / "Versions" / currentVersion.c_str() / "content"), std::filesystem::path(sinewave / "Roblox" / "content"), std::filesystem::copy_options::recursive);
		}

		auto destination2 = std::filesystem::path(sinewave / "Roblox" / "ContentBackup");
		if (!std::filesystem::exists(destination2)) {
			std::filesystem::copy(std::filesystem::path(robloxp / "Versions" / currentVersion.c_str() / "content"), std::filesystem::path(sinewave / "Roblox" / "ContentBackup"), std::filesystem::copy_options::recursive);
		}

		/* install the sinewave assets */
		Logger::log(Logger::SUCCESS, "Installing Sinewave assets..");

		std::vector<std::string> cursors = { "ArrowCursor.png", "ArrowFarCursor.png", "IBeamCursor.png" };
		std::string basee = "https://raw.githubusercontent.com/Mediccc/assets/main/cursors/";
		for (auto c : cursors) {
			Http::downloadFile(basee + c, std::filesystem::path(sinewave / "Assets" / "Cursors" / "2014" / c.c_str()).string());
		}

		Http::downloadFile("https://raw.githubusercontent.com/Mediccc/assets/refs/heads/main/sounds/ouch.ogg", std::filesystem::path(sinewave / "Assets" / "ouch.ogg").string());
		Http::downloadFile("https://raw.githubusercontent.com/Mediccc/assets/refs/heads/main/cursors/2006/ArrowCursor.png", std::filesystem::path(sinewave / "Assets" / "Cursors" / "2006" / "ArrowCursor.png").string());
		Http::downloadFile("https://raw.githubusercontent.com/Mediccc/assets/refs/heads/main/cursors/2006/ArrowFarCursor.png", std::filesystem::path(sinewave / "Assets" / "Cursors" / "2006" / "ArrowFarCursor.png").string());

		Logger::log(Logger::SUCCESS, "Finished installing Sinewave assets!");

		/* create shortcut */
		if (config.createShortcut) {
			HRESULT hr = CoInitialize(NULL);
			if (!SUCCEEDED(hr)) {
				Logger::log(Logger::ERR, "Couldn't initialize the COM library!");
			}

			char exePath[MAX_PATH];
			GetModuleFileNameA(NULL, exePath, MAX_PATH);

			std::filesystem::path desktop = getDesktop();
			std::filesystem::path shortcutPath = desktop / L"Sinewave.lnk";
			hr = CreateLink(
				std::filesystem::path(exePath).c_str(),
				shortcutPath.string().c_str(),
				L"Sinewave"
			);

			if (!SUCCEEDED(hr)) {
				Logger::log(Logger::ERR, "Couldn't create shortcut!");
			}
		}

		fclose(stdout);
		FreeConsole();
		MessageBoxA(NULL, "Please restart Sinewave!", "Sinewave", MB_OK | MB_ICONINFORMATION);
		ExitProcess(0);
	}
	else {
		Logger::log(Logger::INFO, "Roblox is up to date!");
	}

	return currentVersion;
}
