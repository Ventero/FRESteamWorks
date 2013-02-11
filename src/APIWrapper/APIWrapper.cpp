#include "APIWrapper.h"

CLISteam* g_Steam = NULL;

void CLISteam::DispatchEvent(char* code, char* level) {
	// we abuse std::cerr for event dispatching, that way it doesn't interfere
	// with the normal communication on stdout
	std::cerr << "__event__<" << code << "," << level << ">" << std::endl;
}

void send(bool value) {
	std::cout << (value ? "t" : "f") << std::flush;
};

void send(int32 value) {
	std::cout << value << std::flush;
}

void send(float value) {
	std::cout << value << std::flush;
}

void send(std::string value) {
	std::cout << value << std::flush;
}

bool get_bool() {
	std::string item;
	std::getline(std::cin, item);
	return item == "true";
}

int32 get_int() {
	std::string item;
	std::getline(std::cin, item);
	return std::stoi(item);
}

float get_float() {
	std::string item;
	std::getline(std::cin, item);
	return std::stof(item);
}

std::string get_string(bool all) {
	std::string result;
	if(all) {
		std::string item;
		std::getline(std::cin, item);

		size_t length = std::stoi(item);
		char* buf = new char[length];
		std::cin.read(buf, length);
		// remove trailing newline
		result = std::string(buf, length - 1);
		delete buf;
	} else {
		std::getline(std::cin, result);
	}

	return result;
}

bool Init() {
	if(g_Steam) return true;

	if(!SteamAPI_Init()) return false;

	g_Steam = new CLISteam();
	return true;
}

bool RequestStats() {
	bool ret = false;

	if (g_Steam) ret = g_Steam->RequestStats();
	SteamAPI_RunCallbacks();

	return ret;
}

bool SetAchievement(std::string name) {
	bool ret = false;
	if (g_Steam && !name.empty()) {
		ret = g_Steam->SetAchievement(name.c_str());
	}

	SteamAPI_RunCallbacks();
	return ret;
}

bool ClearAchievement(std::string name) {
	if(!g_Steam || name.empty()) return false;

	return g_Steam->ClearAchievement(name.c_str());
}

bool IsAchievement(std::string name) {
	if(!g_Steam || name.empty()) return false;

	return g_Steam->IsAchievement(name.c_str());
}

int32 GetStatInt(std::string name) {
	if(!g_Steam || name.empty()) return 0;

	int32 value;
	g_Steam->GetStat(name.c_str(), &value);
	return value;
}

float GetStatFloat(std::string name) {
	if(!g_Steam || name.empty()) return 0.0f;

	float value = 0.0f;
	g_Steam->GetStat(name.c_str(), &value);
	return value;
}

bool SetStatInt(std::string name, int32 value) {
	if(!g_Steam || name.empty()) return false;;

	return g_Steam->SetStat(name.c_str(), value);
}

bool SetStatFloat(std::string name, float value) {
	if(!g_Steam || name.empty()) return false;

	return g_Steam->SetStat(name.c_str(), value);
}

bool StoreStats() {
	if(!g_Steam) return false;

	return g_Steam->StoreStats();
}

bool ResetAllStats(bool achievementsToo) {
	if(!g_Steam) return false;

	return g_Steam->ResetAllStats(achievementsToo);
}

int32 GetFileCount() {
	if(!g_Steam) return 0;

	return SteamRemoteStorage()->GetFileCount();
}

int32 GetFileSize(std::string name) {
	if(!g_Steam || name.empty()) return 0;

	return SteamRemoteStorage()->GetFileSize(name.c_str());
}

bool FileExists(std::string name) {
	if(!g_Steam || name.empty()) return false;

	return SteamRemoteStorage()->FileExists(name.c_str());
}

bool FileWrite(std::string name, const void* data, size_t length) {
	if(!g_Steam || name.empty()) return false;

	return SteamRemoteStorage()->FileWrite(name.c_str(), data, length);
}

bool FileRead(std::string name, std::string& result) {
	if(!g_Steam || name.empty()) return false;

	int32 size = SteamRemoteStorage()->GetFileSize(name.c_str());
	char* data = (char*)malloc(size);
	bool ret = SteamRemoteStorage()->FileRead(name.c_str(), data, size);
	result = std::string(data, size);
	free(data);

	return ret;
}

bool FileDelete(std::string name) {
	if(!g_Steam || name.empty()) return false;

	return SteamRemoteStorage()->FileDelete(name.c_str());
}

bool IsCloudEnabledForApp() {
	if(!g_Steam) return false;

	return SteamRemoteStorage()->IsCloudEnabledForApp();
}

bool SetCloudEnabledForApp(bool enabled) {
	if(!g_Steam) return false;

	SteamRemoteStorage()->SetCloudEnabledForApp(enabled);

	return (enabled == SteamRemoteStorage()->IsCloudEnabledForApp());
}

void callAPI(APIFunc id) {
	switch(id) {
		case AIRSteam_Init:
			{
				bool res = Init();
				send(res);

#ifdef WHITELIST
				if(res) {
					uint32 appId = SteamUtils()->GetAppID();
					// WHITELIST is a comma separated list of app ids
					uint32 whitelist[] = { WHITELIST };
					// size_t len = sizeof(whitelist) / sizeof(whitelist[0]);
					// for(size_t i = 0; i < len; ++i) {
					// 	if(whitelist[i] == appId) return;
					// }
					for (auto id : whitelist) {
						if(id == appId) return;
					}

					exit(1);
				}
#endif
			}
			return;

		case AIRSteam_RunCallbacks:
			SteamAPI_RunCallbacks();
			return;

		case AIRSteam_RequestStats:
			send(RequestStats());
			return;

		case AIRSteam_SetAchievement:
			send(SetAchievement(get_string(false)));
			return;

		case AIRSteam_ClearAchievement:
			send(ClearAchievement(get_string(false)));
			return;

		case AIRSteam_IsAchievement:
			send(IsAchievement(get_string(false)));
			return;

		case AIRSteam_GetStatInt:
			send(GetStatInt(get_string(false)));
			return;

		case AIRSteam_GetStatFloat:
			send(GetStatFloat(get_string(false)));
			return;

		case AIRSteam_SetStatInt:
			{
				std::string name = get_string(false);
				int32 value = get_int();
				send(SetStatInt(name, value));
			}
			return;

		case AIRSteam_SetStatFloat:
			{
				std::string name = get_string(false);
				float value = get_float();
				send(SetStatFloat(name, value));
			}
			return;

		case AIRSteam_StoreStats:
			send(StoreStats());
			return;

		case AIRSteam_ResetAllStats:
			send(ResetAllStats(get_bool()));
			return;

		// remote storage
		case AIRSteam_GetFileCount:
			send(GetFileCount());
			return;

		case AIRSteam_GetFileSize:
			send(GetFileSize(get_string(false)));
			return;

		case AIRSteam_FileExists:
			send(FileExists(get_string(false)));
			return;

		case AIRSteam_FileWrite:
			{
				std::string name = get_string(false);
				std::string data = get_string(true);
				send(FileWrite(name, data.c_str(), data.length()));
			}
			return;

		case AIRSteam_FileRead:
			{
				std::string data;
				bool res = FileRead(get_string(false), data);
				send(res);
				if (res) send(data);
			}
			return;

		case AIRSteam_FileDelete:
			send(FileDelete(get_string(false)));
			return;

		case AIRSteam_IsCloudEnabledForApp:
			send(IsCloudEnabledForApp());
			return;

		case AIRSteam_SetCloudEnabledForApp:
			send(SetCloudEnabledForApp(get_bool()));
			return;

		default:
			// silently ignore, can't print to stderr because it'd interfere
			// with the event handling ...
			break;
	}
}

int main(int argc, char** argv) {
	while(std::cin.good()) {
		std::string buf;
		std::getline(std::cin, buf);

		if(buf.empty()) break;

		APIFunc func = APIFunc(std::stoi(buf));
		callAPI(func);
	}
	SteamAPI_Shutdown();
}
