/*
 *  APIWrapper.cpp
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

#include "APIWrapper.h"

CLISteam* g_Steam = NULL;

Serializer serializer;

void CLISteam::DispatchEvent(char* code, char* level) {
	// we abuse std::cerr for event dispatching, that way it doesn't interfere
	// with the normal communication on stdout
	std::cerr << "__event__<" << code << "," << level << ">" << std::endl;
}

void sendData(Serializer& serializer) {
	std::vector<u8> data = serializer.data();
	if(data.size() > 1024) {
		// due to output buffering, large outputs need to be sent via a tempfile
		sendDataTempFile(serializer);
		return;
	}

	serializer.clear();

	// first, send a length prefix
	std::vector<u8> length = AmfInteger(data.size()).serialize();
	std::copy(length.begin(), length.end(), std::ostream_iterator<u8>(std::cout));
	// send actual data
	std::copy(data.begin(), data.end(), std::ostream_iterator<u8>(std::cout));
}

void sendDataTempFile(Serializer& serializer) {
	std::vector<u8> data = serializer.data();
	serializer.clear();

	// send length via stdout
	std::vector<u8> length = AmfInteger(data.size()).serialize();
	std::copy(length.begin(), length.end(), std::ostream_iterator<u8>(std::cout));

	std::string filename = std::tmpnam(nullptr);
	std::fstream tmpfile(filename, std::ios::out | std::ios::binary);
	std::copy(data.begin(), data.end(), std::ostream_iterator<u8>(tmpfile));
	tmpfile.close();
	send(filename);
}

template<class T>
void sendItem(T item) {
	Serializer serializer;
	serializer << item;
	sendData(serializer);
}

void send(bool value) {
	sendItem(AmfBool(value));
}

void send(int32 value) {
	sendItem(AmfInteger(value));
}

void send(uint32 value) {
	sendItem(AmfInteger(value));
}

void send(uint64 value) {
	sendItem(AmfString(std::to_string(value)));
}

void send(float value) {
	sendItem(AmfDouble(value));
}

void send(std::string value) {
	sendItem(AmfString(value));
}

void send(std::nullptr_t) {
	sendItem(AmfNull());
}

// TODO: replace this mess with AMF
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

std::string get_string() {
	std::string item;
	std::getline(std::cin, item);

	size_t length = std::stoi(item);
	if (length < 1) return "";
	char* buf = new char[length];
	std::cin.read(buf, length);
	// remove trailing newline
	std::string result(buf, length - 1);

	delete[] buf;
	return result;
}

uint64 get_uint64() {
	std::string str = get_string();
	std::istringstream ss(str);

	uint64 val;
	if(!(ss >> val)) return 0;

	return val;
}

std::vector<std::string> get_array() {
	int length = get_int();
	std::vector<std::string> v;
	if (length < 1) return v;

	for(int i = 0; i < length; ++i) {
		std::string val = get_string();
		v.push_back(val);
	}

	return v;
}

#ifdef DEBUG
void steamWarningMessageHook(int severity, const char* msg) {
	std::fstream f("/tmp/steam.log", std::ios::out | std::ios::app);
	f << "Severity " << severity << ": " << msg << std::endl;
}
#else
void steamWarningMessageHook(int severity, const char* msg) {
	// silently ignore
}
#endif

/*
 * general functions
 */

void AIRSteam_Init() {
	if (g_Steam) {
		send(true);
		return;
	}

	if (!SteamAPI_Init()) {
		send(false);
		return;
	}

	g_Steam = new CLISteam();
	send(true);

#ifdef WHITELIST
	uint32 appId = SteamUtils()->GetAppID();
	// WHITELIST is a comma separated list of app ids
	uint32 whitelist[] = { WHITELIST };
	for (auto id : whitelist) {
		if(id == appId) return;
	}

	exit(1);
#endif

#ifdef DEBUG
	SteamUtils()->SetWarningMessageHook(steamWarningMessageHook);
#endif
}

void AIRSteam_RunCallbacks() {
	SteamAPI_RunCallbacks();
}

void AIRSteam_GetUserID() {
	if (!g_Steam) return send("");

	send(g_Steam->GetUserID());
}

void AIRSteam_GetAppID() {
	if (!g_Steam) return send("");

	send(g_Steam->GetAppID());
}

void AIRSteam_GetPersonaName() {
	if (!g_Steam) return send("");

	send(g_Steam->GetPersonaName());
}

// this is a void function even though the actual API function is a bool,
// because it's not immediately called when FRESteamWorks.useCrashHandler
// is called, as the APIWrapper process is not actually be running at that
// point in time. instead, this function is called as soon as the binary
// has started.
void AIRSteam_UseCrashHandler() {
	uint32 appID = get_int();
	std::string version = get_string();
	std::string date = get_string();
	std::string time = get_string();
	if (!g_Steam) return;

	SteamAPI_SetBreakpadAppID(appID);
	SteamAPI_UseBreakpadCrashHandler(version.c_str(), date.c_str(), time.c_str(),
		false, NULL, NULL);
}

/*
 * stats / achievements
 */

void AIRSteam_RequestStats() {
	bool ret = false;

	if (g_Steam) ret = g_Steam->RequestStats();
	SteamAPI_RunCallbacks();

	send(ret);
}

void AIRSteam_SetAchievement() {
	std::string name = get_string();
	bool ret = false;
	if (g_Steam && !name.empty()) {
		ret = g_Steam->SetAchievement(name);
	}

	SteamAPI_RunCallbacks();
	send(ret);
}

void AIRSteam_ClearAchievement() {
	std::string name = get_string();
	if(!g_Steam || name.empty()) return send(false);

	send(g_Steam->ClearAchievement(name));
}

void AIRSteam_IsAchievement() {
	std::string name = get_string();
	if (!g_Steam || name.empty()) return send(false);

	send(g_Steam->IsAchievement(name));
}

void AIRSteam_GetStatInt() {
	std::string name = get_string();
	if(g_Steam || name.empty()) return send(0);

	int32 value;
	g_Steam->GetStat(name, &value);
	send(value);
}

void AIRSteam_GetStatFloat() {
	std::string name = get_string();
	if (g_Steam || name.empty()) return send(0.0f);

	float value = 0.0f;
	g_Steam->GetStat(name, &value);
	send(value);
}

void AIRSteam_SetStatInt() {
	std::string name = get_string();
	int32 value = get_int();
	if (!g_Steam || name.empty()) return send(false);

	send(g_Steam->SetStat(name, value));
}

void AIRSteam_SetStatFloat() {
	std::string name = get_string();
	float value = get_float();
	if(!g_Steam || name.empty()) return send(false);

	send(g_Steam->SetStat(name, value));
}

void AIRSteam_StoreStats() {
	if(!g_Steam) return send(false);

	send(g_Steam->StoreStats());
}

void AIRSteam_ResetAllStats() {
	bool achievementsToo = get_bool();
	if(!g_Steam) return send(false);

	send(g_Steam->ResetAllStats(achievementsToo));
}

/*
 * remote storage
 */

void AIRSteam_GetFileCount() {
	if(!g_Steam) return send(0);

	send(g_Steam->GetFileCount());
}

void AIRSteam_GetFileSize() {
	std::string name = get_string();
	if(!g_Steam || name.empty()) return send(0);

	send(g_Steam->GetFileSize(name));
}

void AIRSteam_FileExists() {
	std::string name = get_string();
	if(!g_Steam || name.empty()) return send(false);

	send(g_Steam->FileExists(name));
}

void AIRSteam_FileWrite() {
	std::string name = get_string();
	std::string data = get_string();
	if(!g_Steam || name.empty()) return send(false);

	send(g_Steam->FileWrite(name, data.c_str(), data.length()));
}

void AIRSteam_FileRead() {
	std::string name = get_string();
	if(!g_Steam || name.empty()) return send(false);

	char* data = NULL;
	int32 size = g_Steam->FileRead(name, &data);
	if (size == 0) return send(false);

	send(true);
	Serializer serializer;
	serializer << AmfByteArray(data, data + size);
	sendDataTempFile(serializer);
	delete[] data;
}

void AIRSteam_FileDelete() {
	std::string name = get_string();
	if(!g_Steam || name.empty()) return send(false);

	send(g_Steam->FileDelete(name));
}

void AIRSteam_FileShare() {
	std::string name = get_string();
	if(!g_Steam || name.empty()) return send(false);

	send(g_Steam->FileShare(name));
}

void AIRSteam_FileShareResult() {
	if(!g_Steam) return send(k_UGCHandleInvalid);

	send(g_Steam->FileShareResult());
}

void AIRSteam_IsCloudEnabledForApp() {
	if(!g_Steam) return send(false);

	send(g_Steam->IsCloudEnabledForApp());
}

void AIRSteam_SetCloudEnabledForApp() {
	bool enabled = get_bool();
	if(!g_Steam) return send(false);

	send(g_Steam->SetCloudEnabledForApp(enabled));
}

void AIRSteam_GetQuota() {
	if(!g_Steam) return send(nullptr);

	int32 total, avail;
	if(!g_Steam->GetQuota(&total, &avail)) return send(nullptr);

	AmfArray array;
	array.push_back(AmfInteger(total));
	array.push_back(AmfInteger(avail));

	Serializer serializer;
	serializer << array;
	sendData(serializer);
}

/*
 * ugc / workshop
 */
void AIRSteam_UGCDownload() {
	UGCHandle_t handle = get_uint64();
	int32 priority = get_int();
	if(!g_Steam || handle == 0) return send(false);

	send(g_Steam->UGCDownload(handle, priority));
}

void AIRSteam_UGCRead() {
	UGCHandle_t handle = get_uint64();
	int32 size = get_int();
	uint32 offset = get_int();

	if (!g_Steam || handle == 0 || size < 0) return send(false);

	char* data = nullptr;
	int32 result = 0;
	if (size > 0) {
		result = g_Steam->UGCRead(handle, size, offset, &data);
	}

	if(result == 0) return send(false);

	send(true);

	Serializer serializer;
	serializer << AmfByteArray(data, data + result);
	sendDataTempFile(serializer);
	delete[] data;
}

void AIRSteam_GetUGCDownloadProgress() {
	UGCHandle_t handle = get_uint64();
	if (!g_Steam || handle == 0) return send(nullptr);

	int32 downloaded, expected;
	if(!g_Steam->GetUGCDownloadProgress(handle, &downloaded, &expected))
		return send(nullptr);

	AmfArray array;
	array.push_back(AmfInteger(downloaded));
	array.push_back(AmfInteger(expected));

	Serializer serializer;
	serializer << array;
	sendData(serializer);
}

void AIRSteam_GetUGCDownloadResult() {
	UGCHandle_t handle = get_uint64();
	if (!g_Steam || handle == 0) return send(nullptr);

	auto details = g_Steam->GetUGCDownloadResult(handle);
	if (!details) return send(nullptr);

	AmfObjectTraits traits("com.amanitadesign.steam.DownloadUGCResult", false, false);
	AmfObject obj(traits);

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("fileHandle", AmfString(std::to_string(details->m_hFile)));
	obj.addSealedProperty("appID", AmfInteger(details->m_nAppID));
	obj.addSealedProperty("size", AmfInteger(details->m_nSizeInBytes));
	obj.addSealedProperty("fileName", AmfString(details->m_pchFileName));
	obj.addSealedProperty("owner", AmfString(std::to_string(details->m_ulSteamIDOwner)));

	sendItem(obj);
}

void AIRSteam_PublishWorkshopFile() {
	std::string name = get_string();
	std::string preview = get_string();
	uint32 appId = get_int();
	std::string title = get_string();
	std::string description = get_string();
	uint32 visibility = get_int();

	std::vector<std::string> tags = get_array();
	SteamParamStringArray_t tagArray;
	createParamStringArray(tags, &tagArray);

	uint32 fileType = get_int();

	send(g_Steam->PublishWorkshopFile(name, preview, appId, title, description,
		ERemoteStoragePublishedFileVisibility(visibility), &tagArray,
		EWorkshopFileType(fileType)));

	delete[] tagArray.m_ppStrings;
}

void AIRSteam_PublishWorkshopFileResult() {
	if (!g_Steam) return send("0");

	send(g_Steam->PublishWorkshopFileResult());
}

void AIRSteam_DeletePublishedFile() {
	PublishedFileId_t handle = get_uint64();
	if (!g_Steam || handle == 0) return send(false);

	send(g_Steam->DeletePublishedFile(handle));
}

void AIRSteam_GetPublishedFileDetails() {
	PublishedFileId_t handle = get_uint64();
	int32 maxAge = get_int();
	if (!g_Steam || handle == 0) return send(false);

	send(g_Steam->GetPublishedFileDetails(handle, maxAge));
}

void AIRSteam_GetPublishedFileDetailsResult() {
	PublishedFileId_t file = get_uint64();
	if (!g_Steam || file == 0) return send(nullptr);

	auto details = g_Steam->GetPublishedFileDetailsResult(file);
	if (!details) return send(nullptr);

	AmfObjectTraits traits("com.amanitadesign.steam.FileDetailsResult", false, false);
	AmfObject obj(traits);

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("file", AmfString(std::to_string(details->m_nPublishedFileId)));
	obj.addSealedProperty("creatorAppID", AmfInteger(details->m_nCreatorAppID));
	obj.addSealedProperty("consumerAppID", AmfInteger(details->m_nConsumerAppID));
	obj.addSealedProperty("title", AmfString(details->m_rgchTitle));
	obj.addSealedProperty("description", AmfString(details->m_rgchDescription));
	obj.addSealedProperty("fileHandle", AmfString(std::to_string(details->m_hFile)));
	obj.addSealedProperty("previewFileHandle", AmfString(std::to_string(details->m_hPreviewFile)));
	obj.addSealedProperty("owner", AmfString(std::to_string(details->m_ulSteamIDOwner)));
	obj.addSealedProperty("timeCreated", AmfInteger(details->m_rtimeCreated));
	obj.addSealedProperty("timeUpdated", AmfInteger(details->m_rtimeUpdated));
	obj.addSealedProperty("visibility", AmfInteger(details->m_eVisibility));
	obj.addSealedProperty("banned", AmfBool(details->m_bBanned));
	obj.addSealedProperty("tags", AmfString(details->m_rgchTags));
	obj.addSealedProperty("tagsTruncated", AmfBool(details->m_bTagsTruncated));
	obj.addSealedProperty("fileName", AmfString(details->m_pchFileName));
	obj.addSealedProperty("fileSize", AmfInteger(details->m_nFileSize));
	obj.addSealedProperty("previewFileSize", AmfInteger(details->m_nPreviewFileSize));
	obj.addSealedProperty("url", AmfString(details->m_rgchURL));
	obj.addSealedProperty("fileType", AmfInteger(details->m_eFileType));

	sendItem(obj);
}

void AIRSteam_EnumerateUserPublishedFiles() {
	uint32 startIndex = get_int();
	if (!g_Steam) return send(false);

	return send(g_Steam->EnumerateUserPublishedFiles(startIndex));
}

void AIRSteam_EnumerateUserPublishedFilesResult() {
	if (!g_Steam) return send(nullptr);

	auto details = g_Steam->EnumerateUserPublishedFilesResult();
	if (!details) return send(nullptr);

	AmfObjectTraits traits("com.amanitadesign.steam.UserFilesResult", false, false);
	AmfObject obj(traits);

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("resultsReturned", AmfInteger(details->m_nResultsReturned));
	obj.addSealedProperty("totalResults", AmfInteger(details->m_nTotalResultCount));

	AmfArray ids;
	for (int32 i = 0; i < details->m_nResultsReturned; ++i)
		ids.push_back(AmfString(std::to_string(details->m_rgPublishedFileId[i])));

	obj.addSealedProperty("publishedFileId", ids);

	sendItem(obj);
}

void AIRSteam_EnumeratePublishedWorkshopFiles() {
	uint32 type = get_int();
	uint32 start = get_int();
	uint32 count = get_int();
	uint32 days = get_int();

	std::vector<std::string> tags = get_array();
	std::vector<std::string> userTags = get_array();

	SteamParamStringArray_t tagArray, userTagArray;
	createParamStringArray(tags, &tagArray);
	createParamStringArray(userTags, &userTagArray);

	if (!g_Steam) return send(false);

	send(g_Steam->EnumeratePublishedWorkshopFiles(
		EWorkshopEnumerationType(type), start, count, days, &tagArray, &userTagArray));

	delete[] tagArray.m_ppStrings;
	delete[] userTagArray.m_ppStrings;
}

void AIRSteam_EnumeratePublishedWorkshopFilesResult() {
	if (!g_Steam) return send(nullptr);

	auto details = g_Steam->EnumeratePublishedWorkshopFilesResult();
	if (!details) return send(nullptr);

	AmfObjectTraits traits("com.amanitadesign.steam.WorkshopFilesResult", false, false);
	AmfObject obj(traits);

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("resultsReturned", AmfInteger(details->m_nResultsReturned));
	obj.addSealedProperty("totalResults", AmfInteger(details->m_nTotalResultCount));

	AmfArray ids, scores;
	for (int32 i = 0; i < details->m_nResultsReturned; ++i) {
		ids.push_back(AmfString(std::to_string(details->m_rgPublishedFileId[i])));
		scores.push_back(AmfDouble(details->m_rgScore[i]));
	}

	obj.addSealedProperty("publishedFileId", ids);
	obj.addSealedProperty("score", scores);

	sendItem(obj);
}

void AIRSteam_EnumerateUserSubscribedFiles() {
	uint32 startIndex = get_int();
	if (!g_Steam) return send(false);

	return send(g_Steam->EnumerateUserSubscribedFiles(startIndex));
}

void AIRSteam_EnumerateUserSubscribedFilesResult() {
	if (!g_Steam) return send(nullptr);

	auto details = g_Steam->EnumerateUserSubscribedFilesResult();
	if (!details) return send(nullptr);

	AmfObjectTraits traits("com.amanitadesign.steam.SubscribedFilesResult", false, false);
	AmfObject obj(traits);

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("resultsReturned", AmfInteger(details->m_nResultsReturned));
	obj.addSealedProperty("totalResults", AmfInteger(details->m_nTotalResultCount));

	AmfArray ids, timesSubscribed;
	for (int32 i = 0; i < details->m_nResultsReturned; ++i) {
		ids.push_back(AmfString(std::to_string(details->m_rgPublishedFileId[i])));
		timesSubscribed.push_back(AmfDouble(details->m_rgRTimeSubscribed[i]));
	}

	obj.addSealedProperty("publishedFileId", ids);
	obj.addSealedProperty("timeSubscribed", timesSubscribed);

	sendItem(obj);
}

void AIRSteam_EnumerateUserSharedWorkshopFiles() {
	uint64 steamID = get_uint64();
	uint32 start = get_int();

	std::vector<std::string> required = get_array();
	std::vector<std::string> excluded = get_array();

	SteamParamStringArray_t requiredArray, excludedArray;
	createParamStringArray(required, &requiredArray);
	createParamStringArray(excluded, &excludedArray);

	if (!g_Steam) return send(false);

	send(g_Steam->EnumerateUserSharedWorkshopFiles(steamID, start,
		&requiredArray, &excludedArray));

	delete[] requiredArray.m_ppStrings;
	delete[] excludedArray.m_ppStrings;
}

void AIRSteam_EnumerateUserSharedWorkshopFilesResult() {
	if (!g_Steam) return send(nullptr);

	auto details = g_Steam->EnumerateUserSharedWorkshopFilesResult();
	if (!details) return send(nullptr);

	AmfObjectTraits traits("com.amanitadesign.steam.UserFilesResult", false, false);
	AmfObject obj(traits);

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("resultsReturned", AmfInteger(details->m_nResultsReturned));
	obj.addSealedProperty("totalResults", AmfInteger(details->m_nTotalResultCount));

	AmfArray ids;
	for (int32 i = 0; i < details->m_nResultsReturned; ++i) {
		ids.push_back(AmfString(std::to_string(details->m_rgPublishedFileId[i])));
	}

	obj.addSealedProperty("publishedFileId", ids);

	sendItem(obj);
}

void AIRSteam_EnumeratePublishedFilesByUserAction() {
	uint32 action = get_int();
	uint32 startIndex = get_int();
	if (!g_Steam) return send(false);

	return send(g_Steam->EnumeratePublishedFilesByUserAction(
		EWorkshopFileAction(action), startIndex));
}

void AIRSteam_EnumeratePublishedFilesByUserActionResult() {
	if (!g_Steam) return send(nullptr);

	auto details = g_Steam->EnumeratePublishedFilesByUserActionResult();
	if (!details) return send(nullptr);

	AmfObjectTraits traits("com.amanitadesign.steam.FilesByUserActionResult", false, false);
	AmfObject obj(traits);

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("action", AmfInteger(details->m_eAction));
	obj.addSealedProperty("resultsReturned", AmfInteger(details->m_nResultsReturned));
	obj.addSealedProperty("totalResults", AmfInteger(details->m_nTotalResultCount));

	AmfArray ids, timesUpdated;
	for (int32 i = 0; i < details->m_nResultsReturned; ++i) {
		ids.push_back(AmfString(std::to_string(details->m_rgPublishedFileId[i])));
		timesUpdated.push_back(AmfInteger(details->m_rgRTimeUpdated[i]));
	}

	obj.addSealedProperty("publishedFileId", ids);
	obj.addSealedProperty("timeUpdated", timesUpdated);

	sendItem(obj);
}

void AIRSteam_SubscribePublishedFile() {
	PublishedFileId_t handle = get_uint64();
	if (!g_Steam || handle == 0)
		return send(false);

	send(g_Steam->SubscribePublishedFile(handle));
}

void AIRSteam_UnsubscribePublishedFile() {
	PublishedFileId_t handle = get_uint64();
	if (!g_Steam || handle == 0)
		return send(false);

	send(g_Steam->UnsubscribePublishedFile(handle));
}

void AIRSteam_CreatePublishedFileUpdateRequest() {
	PublishedFileId_t file = get_uint64();
	if (!g_Steam || file == 0)
		return send(k_PublishedFileUpdateHandleInvalid);

	send(g_Steam->CreatePublishedFileUpdateRequest(file));
}

void AIRSteam_UpdatePublishedFileFile() {
	PublishedFileId_t handle = get_uint64();
	std::string file = get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || file.empty())
		return send(false);

	send(g_Steam->UpdatePublishedFileFile(handle, file));
}

void AIRSteam_UpdatePublishedFilePreviewFile() {
	PublishedFileId_t handle = get_uint64();
	std::string preview = get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || preview.empty())
		return send(false);

	send(g_Steam->UpdatePublishedFilePreviewFile(handle, preview));
}

void AIRSteam_UpdatePublishedFileTitle() {
	PublishedFileId_t handle = get_uint64();
	std::string title = get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || title.empty())
		return send(false);

	send(g_Steam->UpdatePublishedFileTitle(handle, title));
}

void AIRSteam_UpdatePublishedFileDescription() {
	PublishedFileId_t handle = get_uint64();
	std::string description = get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || description.empty())
		return send(false);

	send(g_Steam->UpdatePublishedFileDescription(handle, description));
}

void AIRSteam_UpdatePublishedFileSetChangeDescription() {
	PublishedFileId_t handle = get_uint64();
	std::string changeDesc = get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || changeDesc.empty())
		return send(false);

	send(g_Steam->UpdatePublishedFileSetChangeDescription(handle, changeDesc));
}

void AIRSteam_UpdatePublishedFileVisibility() {
	PublishedFileId_t handle = get_uint64();
	uint32 visibility = get_int();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid)
		return send(false);

	send(g_Steam->UpdatePublishedFileVisibility(handle,
		ERemoteStoragePublishedFileVisibility(visibility)));
}

void AIRSteam_UpdatePublishedFileTags() {
	PublishedFileUpdateHandle_t handle = get_uint64();

	std::vector<std::string> tags = get_array();
	SteamParamStringArray_t tagArray;
	createParamStringArray(tags, &tagArray);

	if (!g_Steam) return send(false);

	send(g_Steam->UpdatePublishedFileTags(handle, &tagArray));

	delete[] tagArray.m_ppStrings;
}

void AIRSteam_CommitPublishedFileUpdate() {
	PublishedFileId_t handle = get_uint64();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid)
		return send(false);

	send(g_Steam->CommitPublishedFileUpdate(handle));
}

void AIRSteam_GetPublishedItemVoteDetails() {
	PublishedFileId_t file = get_uint64();
	if (!g_Steam || file == 0) return send(false);

	return send(g_Steam->GetPublishedItemVoteDetails(file));
}

void AIRSteam_GetPublishedItemVoteDetailsResult() {
	if (!g_Steam) return send(nullptr);

	auto details = g_Steam->GetPublishedItemVoteDetailsResult();
	if (!details) return send(nullptr);

	AmfObjectTraits traits("com.amanitadesign.steam.ItemVoteDetailsResult", false, false);
	AmfObject obj(traits);

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("publishedFileId", AmfString(std::to_string(details->m_unPublishedFileId)));
	obj.addSealedProperty("votesFor", AmfInteger(details->m_nVotesFor));
	obj.addSealedProperty("votesAgainst", AmfInteger(details->m_nVotesAgainst));
	obj.addSealedProperty("reports", AmfInteger(details->m_nReports));
	obj.addSealedProperty("score", AmfDouble(details->m_fScore));

	sendItem(obj);
}

void AIRSteam_GetUserPublishedItemVoteDetails() {
	PublishedFileId_t file = get_uint64();
	if (!g_Steam || file == 0) return send(false);

	return send(g_Steam->GetUserPublishedItemVoteDetails(file));
}

void AIRSteam_GetUserPublishedItemVoteDetailsResult() {
	if (!g_Steam) return send(nullptr);

	auto details = g_Steam->GetUserPublishedItemVoteDetailsResult();
	if (!details) return send(nullptr);

	AmfObjectTraits traits("com.amanitadesign.steam.UserVoteDetails", false, false);
	AmfObject obj(traits);

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("publishedFileId", AmfString(std::to_string(details->m_nPublishedFileId)));
	obj.addSealedProperty("vote", AmfInteger(details->m_eVote));

	sendItem(obj);
}

void AIRSteam_UpdateUserPublishedItemVote() {
	PublishedFileId_t file = get_uint64();
	bool upvote = get_bool();
	if (!g_Steam || file == 0) return send(false);

	return send(g_Steam->UpdateUserPublishedItemVote(file, upvote));
}

void AIRSteam_SetUserPublishedFileAction() {
	PublishedFileId_t file = get_uint64();
	uint32 action = get_int();
	if (!g_Steam || file == 0) return send(false);

	return send(g_Steam->SetUserPublishedFileAction(file,
		EWorkshopFileAction(action)));
}

/*
 * overlay
 */
void AIRSteam_ActivateGameOverlay() {
	std::string dialog = get_string();
	if (!g_Steam || dialog.empty()) return send(false);

	send(g_Steam->ActivateGameOverlay(dialog));
}

void AIRSteam_ActivateGameOverlayToUser() {
	std::string dialog = get_string();
	uint64 steamId = get_uint64();
	if (!g_Steam || dialog.empty() || steamId == 0) return send(false);

	send(g_Steam->ActivateGameOverlayToUser(dialog, CSteamID(steamId)));
}

void AIRSteam_ActivateGameOverlayToWebPage() {
	std::string url = get_string();
	if (!g_Steam || url.empty()) return send(false);

	send(g_Steam->ActivateGameOverlayToWebPage(url));
}

void AIRSteam_ActivateGameOverlayToStore() {
	uint32 appId = get_int();
	uint32 flag = get_int();
	if (!g_Steam) return send(false);

	send(g_Steam->ActivateGameOverlayToStore(appId,
		EOverlayToStoreFlag(flag)));
}

void AIRSteam_ActivateGameOverlayInviteDialog() {
	uint64 lobbyId = get_uint64();
	if (!g_Steam || lobbyId == 0) return send(false);

	send(g_Steam->ActivateGameOverlayInviteDialog(CSteamID(lobbyId)));
}

void AIRSteam_IsOverlayEnabled() {
	if (!g_Steam) return send(false);

	send(g_Steam->IsOverlayEnabled());
}

/*
 * DLC / subscription
 */
void AIRSteam_IsSubscribedApp() {
	uint32 appId = get_int();
	if (!g_Steam || appId == 0) return send(false);

	send(g_Steam->IsSubscribedApp(appId));
}

void AIRSteam_IsDLCInstalled() {
	uint32 appId = get_int();
	if (!g_Steam || appId == 0) return send(false);

	send(g_Steam->IsDLCInstalled(appId));
}

void AIRSteam_GetDLCCount() {
	if (!g_Steam) return send(0);

	send(g_Steam->GetDLCCount());
}

void AIRSteam_InstallDLC() {
	uint32 appId = get_int();
	if (!g_Steam || appId == 0) return send(false);

	send(g_Steam->InstallDLC(appId));
}

void AIRSteam_UninstallDLC() {
	uint32 appId = get_int();
	if (!g_Steam || appId == 0) return send(false);

	send(g_Steam->UninstallDLC(appId));
}

void AIRSteam_DLCInstalledResult() {
	if (!g_Steam) return send(0);

	send(g_Steam->DLCInstalledResult());
}


int main(int argc, char** argv) {
	std::ios::sync_with_stdio(false);

	while(std::cin.good()) {
		std::string buf;
		std::getline(std::cin, buf);

		if(buf.empty()) break;

		unsigned int func;
		try {
			func = std::stoi(buf);
		} catch (...) {
			// just read on and hope for the best
			continue;
		}

		if (func >= apiFunctions.size())
			continue;

		apiFunctions[func]();
	}

	SteamAPI_Shutdown();
	delete g_Steam;
	return 0;
}
