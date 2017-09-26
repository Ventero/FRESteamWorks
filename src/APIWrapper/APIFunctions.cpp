/*
 *  APIFunctions.cpp
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012-2014 Ventero. All rights reserved.
 */

#include "APIFunctions.h"
#include "SteamWrapper.h"

#include <types/amfbool.hpp>
#include <types/amfbytearray.hpp>
#include <types/amfdouble.hpp>
#include <types/amfinteger.hpp>
#include <types/amfnull.hpp>
#include <types/amfstring.hpp>
#include <serializer.hpp>

#include <cstdlib>
// For the low-level fd hackery in RestartAppIfNecessary.
#include <fcntl.h>
#include <unistd.h>

using namespace amf;

extern SteamWrapper* g_Steam;
extern void steamWarningMessageHook(int severity, const char* msg);

/*
 * general functions
 */

bool AIRSteam_Init() {
	if (!g_Steam) return false;
	if (g_Steam->Initialized()) return true;

	if (!g_Steam->Initialize()) {
		return false;
	}

#ifdef WHITELIST
	uint32 appId = g_Steam->GetAppID();
	// WHITELIST is a comma separated list of app ids
	uint32 whitelist[] = { WHITELIST };
	auto found = std::find(std::begin(whitelist), std::end(whitelist), appId);
	if (found == std::end(whitelist))
		exit(1);
#endif

#ifdef DEBUG
	g_Steam->SetWarningMessageHook(steamWarningMessageHook);
#endif

	return true;
}

void AIRSteam_Shutdown() {
	SteamAPI_Shutdown();
}

std::nullptr_t AIRSteam_RunCallbacks() {
	SteamAPI_RunCallbacks();
	return nullptr;
}

uint64 AIRSteam_GetUserID() {
	if (!g_Steam) return 0;

	return g_Steam->GetUserID().ConvertToUint64();
}

uint32 AIRSteam_GetAppID() {
	if (!g_Steam) return 0;

	return g_Steam->GetAppID();
}

std::string AIRSteam_GetAvailableGameLanguages() {
	if (!g_Steam) return "";

	return g_Steam->GetAvailableGameLanguages();
}

std::string AIRSteam_GetCurrentGameLanguage() {
	if (!g_Steam) return "";

	return g_Steam->GetCurrentGameLanguage();
}

std::string AIRSteam_GetPersonaName() {
	if (!g_Steam) return "";

	return g_Steam->GetPersonaName();
}

bool AIRSteam_RestartAppIfNecessary() {
	uint32 appID = g_Steam->get_int();

	// The call to SteamAPI_RestartAppIfNecessary below might fork the process
	// to launch the Steam client in a subprocess. As that subprocess inherits
	// all file descriptors from this parent process, its stdout/stderr fds are
	// actually pipes to the AIR runtime this process was started from. If the
	// AIR application terminates in response to RestartAppIfNecessary (because
	// it's restarting the application through the Steam client), these pipes
	// might be closed at any point. If any process (including child processes)
	// then tries to write to them, it will receive a SIGFPE and is terminated.
	// As the Steam client may be started inside a child process, it might be
	// killed as soon as it tries to print anything, e.g. debug output.

	// To work around this issue, we re-open stdout/stderr to /dev/null before
	// calling RestartAppIfNecessary.
	int fd = open("/dev/null", O_RDONLY);

	// However, since we actually still need to communicate with the AIR runtime
	// to pass on the return value of this function, we have to restore the
	// stdout/stderr fds again *after* the call to RestartAppIfNecessary.
	int out = dup(STDOUT_FILENO);
	int err = dup(STDERR_FILENO);

	// Redirect stdout/stderr to /dev/null for the child process ...
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	// ... do the actual API call ...
	bool ret = SteamAPI_RestartAppIfNecessary(appID);

	// ... and restore the original stdout/stderr pipes for the parent process.
	dup2(out, STDOUT_FILENO);
	dup2(err, STDERR_FILENO);

	return ret;
}

/*
 * stats / achievements
 */

bool AIRSteam_RequestStats() {
	bool ret = false;

	if (g_Steam) ret = g_Steam->RequestStats();
	SteamAPI_RunCallbacks();

	return ret;
}

bool AIRSteam_SetAchievement() {
	std::string name = g_Steam->get_string();
	bool ret = false;
	if (g_Steam && !name.empty()) {
		ret = g_Steam->SetAchievement(name);
	}

	SteamAPI_RunCallbacks();
	return ret;
}

bool AIRSteam_ClearAchievement() {
	std::string name = g_Steam->get_string();
	if(!g_Steam || name.empty()) return false;

	return g_Steam->ClearAchievement(name);
}

bool AIRSteam_IsAchievement() {
	std::string name = g_Steam->get_string();
	if (!g_Steam || name.empty()) return false;

	return g_Steam->IsAchievement(name);
}

bool AIRSteam_IndicateAchievementProgress() {
	std::string name = g_Steam->get_string();
	uint32 current_progress = g_Steam->get_int();
	uint32 max_progress = g_Steam->get_int();
	if (!g_Steam || name.empty()) return false;

	return g_Steam->IndicateAchievementProgress(name, current_progress,
		max_progress);
}

int32 AIRSteam_GetStatInt() {
	std::string name = g_Steam->get_string();
	if (!g_Steam || name.empty()) return 0;

	int32 value = 0;
	g_Steam->GetStat(name, &value);
	return value;
}

float AIRSteam_GetStatFloat() {
	std::string name = g_Steam->get_string();
	if (!g_Steam || name.empty()) return 0.0f;

	float value = 0.0f;
	g_Steam->GetStat(name, &value);
	return value;
}

bool AIRSteam_SetStatInt() {
	std::string name = g_Steam->get_string();
	int32 value = g_Steam->get_int();
	if (!g_Steam || name.empty()) return false;

	return g_Steam->SetStat(name, value);
}

bool AIRSteam_SetStatFloat() {
	std::string name = g_Steam->get_string();
	float value = g_Steam->get_float();
	if(!g_Steam || name.empty()) return false;

	return g_Steam->SetStat(name, value);
}

bool AIRSteam_StoreStats() {
	if(!g_Steam) return false;

	return g_Steam->StoreStats();
}

bool AIRSteam_ResetAllStats() {
	bool achievementsToo = g_Steam->get_bool();
	if(!g_Steam) return false;

	return g_Steam->ResetAllStats(achievementsToo);
}

bool AIRSteam_RequestGlobalStats() {
	int days = g_Steam->get_int();
	if (!g_Steam) return false;

	return g_Steam->RequestGlobalStats(days);
}

// Since Flash doesn't have support for 64bit integer types, we just always
// return a double, which gets sent to Flash as an AmfDouble (aka Number).
double AIRSteam_GetGlobalStatInt() {
	std::string name = g_Steam->get_string();
	if (!g_Steam || name.empty()) return 0.;

	int64 value = 0;
	g_Steam->GetGlobalStat(name, &value);

	return static_cast<double>(value);
}

double AIRSteam_GetGlobalStatFloat() {
	std::string name = g_Steam->get_string();
	if (!g_Steam || name.empty()) return 0.0;

	double value = 0.0;
	g_Steam->GetGlobalStat(name, &value);

	return value;
}

// See above for why double is used.
AmfArray AIRSteam_GetGlobalStatHistoryInt() {
	AmfArray array;

	std::string name = g_Steam->get_string();
	uint32 days = g_Steam->get_int();
	if (!g_Steam || name.empty() || days == 0)
		return array;

	auto history = g_Steam->GetGlobalStatHistoryInt(name, days);
	for (int64 entry : history) {
		array.push_back(AmfDouble(static_cast<double>(entry)));
	}

	return array;
}

AmfArray AIRSteam_GetGlobalStatHistoryFloat() {
	AmfArray array;

	std::string name = g_Steam->get_string();
	uint32 days = g_Steam->get_int();
	if (!g_Steam || name.empty() || days == 0)
		return array;

	auto history = g_Steam->GetGlobalStatHistoryFloat(name, days);
	for (double entry : history) {
		array.push_back(AmfDouble(entry));
	}

	return array;
}

/*
 * leaderboards
 */

bool AIRSteam_FindLeaderboard() {
	std::string name = g_Steam->get_string();
	if (!g_Steam || name.empty()) return false;

	return g_Steam->FindLeaderboard(name);
}

bool AIRSteam_FindOrCreateLeaderboard() {
	std::string name = g_Steam->get_string();
	uint32 sort = g_Steam->get_int();
	uint32 display = g_Steam->get_int();
	if (!g_Steam || name.empty()) return false;

	return g_Steam->FindOrCreateLeaderboard(name,
		ELeaderboardSortMethod(sort), ELeaderboardDisplayType(display));
}

SteamLeaderboard_t AIRSteam_FindLeaderboardResult() {
	if (!g_Steam) return 0;

	return g_Steam->FindLeaderboardResult();
}

std::string AIRSteam_GetLeaderboardName() {
	SteamLeaderboard_t handle = g_Steam->get_uint64();
	if (!g_Steam || handle == 0) return "";

	return g_Steam->GetLeaderboardName(handle);
}

int32 AIRSteam_GetLeaderboardEntryCount() {
	SteamLeaderboard_t handle = g_Steam->get_uint64();
	if (!g_Steam || handle == 0) return 0;

	return g_Steam->GetLeaderboardEntryCount(handle);
}

uint32 AIRSteam_GetLeaderboardSortMethod() {
	SteamLeaderboard_t handle = g_Steam->get_uint64();
	if (!g_Steam || handle == 0) return 0;

	return g_Steam->GetLeaderboardSortMethod(handle);
}

uint32 AIRSteam_GetLeaderboardDisplayType() {
	SteamLeaderboard_t handle = g_Steam->get_uint64();
	if (!g_Steam || handle == 0) return 0;

	return g_Steam->GetLeaderboardDisplayType(handle);
}

bool AIRSteam_UploadLeaderboardScore() {
	SteamLeaderboard_t handle = g_Steam->get_uint64();
	uint32 method = g_Steam->get_int();
	int32 score = g_Steam->get_int();
	std::vector<int32> details = g_Steam->get_int_array();

	if (!g_Steam) return false;

	return g_Steam->UploadLeaderboardScore(handle,
		ELeaderboardUploadScoreMethod(method), score,
		details.data(), static_cast<int>(details.size()));
}

AmfObject AIRSteam_UploadLeaderboardScoreResult() {
	AmfObject obj("com.amanitadesign.steam.UploadLeaderboardScoreResult", false, false);

	if (!g_Steam) return obj;

	auto details = g_Steam->UploadLeaderboardScoreResult();
	if (!details) return obj;

	obj.addSealedProperty("success", AmfBool(details->m_bSuccess));
	obj.addSealedProperty("leaderboardHandle", AmfString(std::to_string(details->m_hSteamLeaderboard)));
	obj.addSealedProperty("score", AmfInteger(details->m_nScore));
	obj.addSealedProperty("scoreChanged", AmfBool(details->m_bScoreChanged));
	obj.addSealedProperty("newGlobalRank", AmfInteger(details->m_nGlobalRankNew));
	obj.addSealedProperty("previousGlobalRank", AmfInteger(details->m_nGlobalRankPrevious));

	return obj;
}

bool AIRSteam_DownloadLeaderboardEntries() {
	SteamLeaderboard_t handle = g_Steam->get_uint64();
	uint32 request = g_Steam->get_int();
	int rangeStart = g_Steam->get_int();
	int rangeEnd = g_Steam->get_int();

	if (!g_Steam) return false;

	return g_Steam->DownloadLeaderboardEntries(handle,
		ELeaderboardDataRequest(request), rangeStart, rangeEnd);
}

AmfArray AIRSteam_DownloadLeaderboardEntriesResult() {
	AmfArray array;

	int32 numDetails = g_Steam->get_int();
	if (!g_Steam) return array;

	auto entries = g_Steam->DownloadLeaderboardEntriesResult(numDetails);
	if (entries.empty()) return array;

	for (size_t i = 0; i < entries.size(); ++i) {
		AmfObject obj("com.amanitadesign.steam.LeaderboardEntry", false, false);

		LeaderboardEntry_t *e = &entries[i].entry;
		obj.addSealedProperty("userID", AmfString(std::to_string(e->m_steamIDUser.ConvertToUint64())));
		obj.addSealedProperty("globalRank", AmfInteger(e->m_nGlobalRank));
		obj.addSealedProperty("score", AmfInteger(e->m_nScore));
		obj.addSealedProperty("numDetails", AmfInteger(e->m_cDetails));

		AmfArray details;
		int32 dets = e->m_cDetails;
		if (numDetails < dets) dets = numDetails;
		for (int d = 0; d < dets; ++d) {
			details.push_back(AmfInteger(entries[i].details[d]));
		}
		obj.addSealedProperty("details", details);

		array.push_back(obj);
	}

	return array;
}

/*
 * remote storage
 */

int32 AIRSteam_GetFileCount() {
	if(!g_Steam) return 0;

	return g_Steam->GetFileCount();
}

int32 AIRSteam_GetFileSize() {
	std::string name = g_Steam->get_string();
	if(!g_Steam || name.empty()) return 0;

	return g_Steam->GetFileSize(name);
}

bool AIRSteam_FileExists() {
	std::string name = g_Steam->get_string();
	if(!g_Steam || name.empty()) return false;

	return g_Steam->FileExists(name);
}

bool AIRSteam_FileWrite() {
	std::string name = g_Steam->get_string();
	std::string data = g_Steam->get_bytearray();
	if(!g_Steam || name.empty()) return false;

	return g_Steam->FileWrite(name, data.c_str(), static_cast<int32>(data.length()));
}

bool AIRSteam_FileRead() {
	std::string name = g_Steam->get_string();
	if(!g_Steam || name.empty()) return false;

	char* data = NULL;
	int32 size = g_Steam->FileRead(name, &data);
	if (size == 0) return false;

	g_Steam->send(true);
	g_Steam->sendBuffer(AmfByteArray(data, data + size));
	delete[] data;

	return true;
}

bool AIRSteam_FileDelete() {
	std::string name = g_Steam->get_string();
	if(!g_Steam || name.empty()) return false;

	return g_Steam->FileDelete(name);
}

bool AIRSteam_FileShare() {
	std::string name = g_Steam->get_string();
	if(!g_Steam || name.empty()) return false;

	return g_Steam->FileShare(name);
}

UGCHandle_t AIRSteam_FileShareResult() {
	if(!g_Steam) return k_UGCHandleInvalid;

	return g_Steam->FileShareResult();
}

bool AIRSteam_IsCloudEnabledForApp() {
	if(!g_Steam) return false;

	return g_Steam->IsCloudEnabledForApp();
}

bool AIRSteam_SetCloudEnabledForApp() {
	bool enabled = g_Steam->get_bool();
	if(!g_Steam) return false;

	return g_Steam->SetCloudEnabledForApp(enabled);
}

AmfArray AIRSteam_GetQuota() {
	AmfArray array;

	if(!g_Steam) return array;

	uint64 total, avail;
	if(!g_Steam->GetQuota(&total, &avail)) return array;

	array.push_back(AmfString(std::to_string(total)));
	array.push_back(AmfString(std::to_string(avail)));

	return array;
}

/*
 * ugc / workshop
 */
bool AIRSteam_UGCDownload() {
	UGCHandle_t handle = g_Steam->get_uint64();
	uint32 priority = g_Steam->get_int();
	if(!g_Steam || handle == 0) return false;

	return g_Steam->UGCDownload(handle, priority);
}

bool AIRSteam_UGCRead() {
	UGCHandle_t handle = g_Steam->get_uint64();
	int32 size = g_Steam->get_int();
	uint32 offset = g_Steam->get_int();

	if (!g_Steam || handle == 0 || size < 0) return false;

	char* data = nullptr;
	int32 result = 0;
	if (size <= 0) return false;

	result = g_Steam->UGCRead(handle, size, offset, &data);
	if(result <= 0) {
		delete[] data;
		return false;
	}

	g_Steam->send(true);
	g_Steam->sendBuffer(AmfByteArray(data, data + result));
	delete[] data;

	return true;
}

AmfArray AIRSteam_GetUGCDownloadProgress() {
	AmfArray array;

	UGCHandle_t handle = g_Steam->get_uint64();
	if (!g_Steam || handle == 0) return array;

	int32 downloaded, expected;
	if(!g_Steam->GetUGCDownloadProgress(handle, &downloaded, &expected))
		return array;

	array.push_back(AmfInteger(downloaded));
	array.push_back(AmfInteger(expected));

	return array;
}

AmfObject AIRSteam_GetUGCDownloadResult() {
	AmfObject obj("com.amanitadesign.steam.DownloadUGCResult", false, false);

	UGCHandle_t handle = g_Steam->get_uint64();
	if (!g_Steam || handle == 0) return obj;

	auto details = g_Steam->GetUGCDownloadResult(handle);
	if (!details) return obj;

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("fileHandle", AmfString(std::to_string(details->m_hFile)));
	obj.addSealedProperty("appID", AmfInteger(details->m_nAppID));
	obj.addSealedProperty("size", AmfInteger(details->m_nSizeInBytes));
	obj.addSealedProperty("fileName", AmfString(details->m_pchFileName));
	obj.addSealedProperty("owner", AmfString(std::to_string(details->m_ulSteamIDOwner)));

	return obj;
}

bool AIRSteam_PublishWorkshopFile() {
	std::string name = g_Steam->get_string();
	std::string preview = g_Steam->get_string();
	uint32 appId = g_Steam->get_int();
	std::string title = g_Steam->get_string();
	std::string description = g_Steam->get_string();
	uint32 visibility = g_Steam->get_int();

	std::vector<std::string> tags = g_Steam->get_string_array();
	SteamParamStringArray_t tagArray;
	createParamStringArray(tags, &tagArray);

	uint32 fileType = g_Steam->get_int();

	if (!g_Steam) return false;

	bool ret = g_Steam->PublishWorkshopFile(name, preview, appId, title, description,
		ERemoteStoragePublishedFileVisibility(visibility), &tagArray,
		EWorkshopFileType(fileType));

	delete[] tagArray.m_ppStrings;

	return ret;
}

PublishedFileId_t AIRSteam_PublishWorkshopFileResult() {
	if (!g_Steam) return 0;

	return g_Steam->PublishWorkshopFileResult();
}

bool AIRSteam_DeletePublishedFile() {
	PublishedFileId_t handle = g_Steam->get_uint64();
	if (!g_Steam || handle == 0) return false;

	return g_Steam->DeletePublishedFile(handle);
}

bool AIRSteam_GetPublishedFileDetails() {
	PublishedFileId_t handle = g_Steam->get_uint64();
	uint32 maxAge = g_Steam->get_int();
	if (!g_Steam || handle == 0) return false;

	return g_Steam->GetPublishedFileDetails(handle, maxAge);
}

AmfObject AIRSteam_GetPublishedFileDetailsResult() {
	AmfObject obj("com.amanitadesign.steam.FileDetailsResult", false, false);

	PublishedFileId_t file = g_Steam->get_uint64();
	if (!g_Steam || file == 0) return obj;

	auto details = g_Steam->GetPublishedFileDetailsResult(file);
	if (!details) return obj;

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

	return obj;
}

bool AIRSteam_EnumerateUserPublishedFiles() {
	uint32 startIndex = g_Steam->get_int();
	if (!g_Steam) return false;

	return g_Steam->EnumerateUserPublishedFiles(startIndex);
}

AmfObject AIRSteam_EnumerateUserPublishedFilesResult() {
	AmfObject obj("com.amanitadesign.steam.UserFilesResult", false, false);

	if (!g_Steam) return obj;

	auto details = g_Steam->EnumerateUserPublishedFilesResult();
	if (!details) return obj;

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("resultsReturned", AmfInteger(details->m_nResultsReturned));
	obj.addSealedProperty("totalResults", AmfInteger(details->m_nTotalResultCount));

	AmfArray ids;
	for (int32 i = 0; i < details->m_nResultsReturned; ++i)
		ids.push_back(AmfString(std::to_string(details->m_rgPublishedFileId[i])));

	obj.addSealedProperty("publishedFileId", ids);

	return obj;
}

bool AIRSteam_EnumeratePublishedWorkshopFiles() {
	uint32 type = g_Steam->get_int();
	uint32 start = g_Steam->get_int();
	uint32 count = g_Steam->get_int();
	uint32 days = g_Steam->get_int();

	std::vector<std::string> tags = g_Steam->get_string_array();
	std::vector<std::string> userTags = g_Steam->get_string_array();

	SteamParamStringArray_t tagArray, userTagArray;
	createParamStringArray(tags, &tagArray);
	createParamStringArray(userTags, &userTagArray);

	if (!g_Steam) return false;

	bool ret = g_Steam->EnumeratePublishedWorkshopFiles(
		EWorkshopEnumerationType(type), start, count, days, &tagArray, &userTagArray);

	delete[] tagArray.m_ppStrings;
	delete[] userTagArray.m_ppStrings;

	return ret;
}

AmfObject AIRSteam_EnumeratePublishedWorkshopFilesResult() {
	AmfObject obj("com.amanitadesign.steam.WorkshopFilesResult", false, false);

	if (!g_Steam) return obj;

	auto details = g_Steam->EnumeratePublishedWorkshopFilesResult();
	if (!details) return obj;

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

	return obj;
}

bool AIRSteam_EnumerateUserSubscribedFiles() {
	uint32 startIndex = g_Steam->get_int();
	if (!g_Steam) return false;

	return g_Steam->EnumerateUserSubscribedFiles(startIndex);
}

AmfObject AIRSteam_EnumerateUserSubscribedFilesResult() {
	AmfObject obj("com.amanitadesign.steam.SubscribedFilesResult", false, false);

	if (!g_Steam) return obj;

	auto details = g_Steam->EnumerateUserSubscribedFilesResult();
	if (!details) return obj;

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

	return obj;
}

bool AIRSteam_EnumerateUserSharedWorkshopFiles() {
	uint64 steamID = g_Steam->get_uint64();
	uint32 start = g_Steam->get_int();

	std::vector<std::string> required = g_Steam->get_string_array();
	std::vector<std::string> excluded = g_Steam->get_string_array();

	SteamParamStringArray_t requiredArray, excludedArray;
	createParamStringArray(required, &requiredArray);
	createParamStringArray(excluded, &excludedArray);

	if (!g_Steam) return false;

	bool ret = g_Steam->EnumerateUserSharedWorkshopFiles(steamID, start,
		&requiredArray, &excludedArray);

	delete[] requiredArray.m_ppStrings;
	delete[] excludedArray.m_ppStrings;

	return ret;
}

AmfObject AIRSteam_EnumerateUserSharedWorkshopFilesResult() {
	AmfObject obj("com.amanitadesign.steam.UserFilesResult", false, false);

	if (!g_Steam) return obj;

	auto details = g_Steam->EnumerateUserSharedWorkshopFilesResult();
	if (!details) return obj;

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("resultsReturned", AmfInteger(details->m_nResultsReturned));
	obj.addSealedProperty("totalResults", AmfInteger(details->m_nTotalResultCount));

	AmfArray ids;
	for (int32 i = 0; i < details->m_nResultsReturned; ++i) {
		ids.push_back(AmfString(std::to_string(details->m_rgPublishedFileId[i])));
	}

	obj.addSealedProperty("publishedFileId", ids);

	return obj;
}

bool AIRSteam_EnumeratePublishedFilesByUserAction() {
	uint32 action = g_Steam->get_int();
	uint32 startIndex = g_Steam->get_int();
	if (!g_Steam) return false;

	return g_Steam->EnumeratePublishedFilesByUserAction(
		EWorkshopFileAction(action), startIndex);
}

AmfObject AIRSteam_EnumeratePublishedFilesByUserActionResult() {
	AmfObject obj("com.amanitadesign.steam.FilesByUserActionResult", false, false);

	if (!g_Steam) return obj;

	auto details = g_Steam->EnumeratePublishedFilesByUserActionResult();
	if (!details) return obj;

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

	return obj;
}

bool AIRSteam_SubscribePublishedFile() {
	PublishedFileId_t handle = g_Steam->get_uint64();
	if (!g_Steam || handle == 0)
		return false;

	return g_Steam->SubscribePublishedFile(handle);
}

bool AIRSteam_UnsubscribePublishedFile() {
	PublishedFileId_t handle = g_Steam->get_uint64();
	if (!g_Steam || handle == 0)
		return false;

	return g_Steam->UnsubscribePublishedFile(handle);
}

PublishedFileUpdateHandle_t AIRSteam_CreatePublishedFileUpdateRequest() {
	PublishedFileId_t file = g_Steam->get_uint64();
	if (!g_Steam || file == 0)
		return k_PublishedFileUpdateHandleInvalid;

	return g_Steam->CreatePublishedFileUpdateRequest(file);
}

bool AIRSteam_UpdatePublishedFileFile() {
	PublishedFileId_t handle = g_Steam->get_uint64();
	std::string file = g_Steam->get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || file.empty())
		return false;

	return g_Steam->UpdatePublishedFileFile(handle, file);
}

bool AIRSteam_UpdatePublishedFilePreviewFile() {
	PublishedFileId_t handle = g_Steam->get_uint64();
	std::string preview = g_Steam->get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || preview.empty())
		return false;

	return g_Steam->UpdatePublishedFilePreviewFile(handle, preview);
}

bool AIRSteam_UpdatePublishedFileTitle() {
	PublishedFileId_t handle = g_Steam->get_uint64();
	std::string title = g_Steam->get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || title.empty())
		return false;

	return g_Steam->UpdatePublishedFileTitle(handle, title);
}

bool AIRSteam_UpdatePublishedFileDescription() {
	PublishedFileId_t handle = g_Steam->get_uint64();
	std::string description = g_Steam->get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || description.empty())
		return false;

	return g_Steam->UpdatePublishedFileDescription(handle, description);
}

bool AIRSteam_UpdatePublishedFileSetChangeDescription() {
	PublishedFileId_t handle = g_Steam->get_uint64();
	std::string changeDesc = g_Steam->get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || changeDesc.empty())
		return false;

	return g_Steam->UpdatePublishedFileSetChangeDescription(handle, changeDesc);
}

bool AIRSteam_UpdatePublishedFileVisibility() {
	PublishedFileId_t handle = g_Steam->get_uint64();
	uint32 visibility = g_Steam->get_int();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid)
		return false;

	return g_Steam->UpdatePublishedFileVisibility(handle,
		ERemoteStoragePublishedFileVisibility(visibility));
}

bool AIRSteam_UpdatePublishedFileTags() {
	PublishedFileUpdateHandle_t handle = g_Steam->get_uint64();

	std::vector<std::string> tags = g_Steam->get_string_array();
	SteamParamStringArray_t tagArray;
	createParamStringArray(tags, &tagArray);

	if (!g_Steam) return false;

	bool ret = g_Steam->UpdatePublishedFileTags(handle, &tagArray);

	delete[] tagArray.m_ppStrings;

	return ret;
}

bool AIRSteam_CommitPublishedFileUpdate() {
	PublishedFileId_t handle = g_Steam->get_uint64();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid)
		return false;

	return g_Steam->CommitPublishedFileUpdate(handle);
}

bool AIRSteam_GetPublishedItemVoteDetails() {
	PublishedFileId_t file = g_Steam->get_uint64();
	if (!g_Steam || file == 0) return false;

	return g_Steam->GetPublishedItemVoteDetails(file);
}

AmfObject AIRSteam_GetPublishedItemVoteDetailsResult() {
	AmfObject obj("com.amanitadesign.steam.ItemVoteDetailsResult", false, false);

	if (!g_Steam) return obj;

	auto details = g_Steam->GetPublishedItemVoteDetailsResult();
	if (!details) return obj;

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("publishedFileId", AmfString(std::to_string(details->m_unPublishedFileId)));
	obj.addSealedProperty("votesFor", AmfInteger(details->m_nVotesFor));
	obj.addSealedProperty("votesAgainst", AmfInteger(details->m_nVotesAgainst));
	obj.addSealedProperty("reports", AmfInteger(details->m_nReports));
	obj.addSealedProperty("score", AmfDouble(details->m_fScore));

	return obj;
}

bool AIRSteam_GetUserPublishedItemVoteDetails() {
	PublishedFileId_t file = g_Steam->get_uint64();
	if (!g_Steam || file == 0) return false;

	return g_Steam->GetUserPublishedItemVoteDetails(file);
}

AmfObject AIRSteam_GetUserPublishedItemVoteDetailsResult() {
	AmfObject obj("com.amanitadesign.steam.UserVoteDetails", false, false);

	if (!g_Steam) return obj;

	auto details = g_Steam->GetUserPublishedItemVoteDetailsResult();
	if (!details) return obj;

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("publishedFileId", AmfString(std::to_string(details->m_nPublishedFileId)));
	obj.addSealedProperty("vote", AmfInteger(details->m_eVote));

	return obj;
}

bool AIRSteam_UpdateUserPublishedItemVote() {
	PublishedFileId_t file = g_Steam->get_uint64();
	bool upvote = g_Steam->get_bool();
	if (!g_Steam || file == 0) return false;

	return g_Steam->UpdateUserPublishedItemVote(file, upvote);
}

bool AIRSteam_SetUserPublishedFileAction() {
	PublishedFileId_t file = g_Steam->get_uint64();
	uint32 action = g_Steam->get_int();
	if (!g_Steam || file == 0) return false;

	return g_Steam->SetUserPublishedFileAction(file,
		EWorkshopFileAction(action));
}

/*
 * friends
 */

int AIRSteam_GetFriendCount() {
	uint32 flags = g_Steam->get_int();
	if (!g_Steam) return 0;

	return g_Steam->GetFriendCount(flags);
}

uint64 AIRSteam_GetFriendByIndex() {
	int index = g_Steam->get_int();
	uint32 flags = g_Steam->get_int();
	if (!g_Steam) return 0;

	return g_Steam->GetFriendByIndex(index, flags).ConvertToUint64();
}

std::string AIRSteam_GetFriendPersonaName() {
	uint64 steamId = g_Steam->get_uint64();
	if (!g_Steam) return "";

	return g_Steam->GetFriendPersonaName(CSteamID(steamId));
}

bool AIRSteam_GetSmallFriendAvatar() {
	uint64 steamId = g_Steam->get_uint64();

	Image image = g_Steam->GetSmallFriendAvatar(CSteamID(steamId));

	g_Steam->send(image.width);
	g_Steam->send(image.height);
	g_Steam->sendBuffer(AmfByteArray(image.argb_data()));

	return (image.data.size() != 0);
}

bool AIRSteam_GetMediumFriendAvatar() {
	uint64 steamId = g_Steam->get_uint64();

	Image image = g_Steam->GetMediumFriendAvatar(CSteamID(steamId));

	g_Steam->send(image.width);
	g_Steam->send(image.height);
	g_Steam->sendBuffer(AmfByteArray(image.argb_data()));

	return (image.data.size() != 0);
}

/*
 * authentication & ownership
 */

uint32 AIRSteam_GetAuthSessionTicket() {
	if (!g_Steam) return k_HAuthTicketInvalid;

	char* data = nullptr;
	uint32 length = 0;
	HAuthTicket ret = g_Steam->GetAuthSessionTicket(&data, &length);

	g_Steam->sendBuffer(AmfByteArray(data, data + length));
	delete[] data;

	return ret;
}

uint32 AIRSteam_GetAuthSessionTicketResult() {
	if (!g_Steam) return k_HAuthTicketInvalid;

	return g_Steam->GetAuthSessionTicketResult();
}

int AIRSteam_BeginAuthSession() {
	std::string data = g_Steam->get_bytearray();
	uint64 steamId = g_Steam->get_uint64();
	if (!g_Steam || data.empty()) return k_EBeginAuthSessionResultInvalidTicket;

	return g_Steam->BeginAuthSession(data.c_str(), static_cast<int>(data.length()), CSteamID(steamId));
}

bool AIRSteam_EndAuthSession() {
	uint64 steamId = g_Steam->get_uint64();
	if (!g_Steam) return false;

	return g_Steam->EndAuthSession(CSteamID(steamId));
}

bool AIRSteam_CancelAuthTicket() {
	uint32 ticketHandle = g_Steam->get_int();
	if (!g_Steam) return false;

	return g_Steam->CancelAuthTicket(ticketHandle);
}

int AIRSteam_UserHasLicenseForApp() {
	uint64 steamId = g_Steam->get_uint64();
	uint32 appId = g_Steam->get_int();
	if (!g_Steam) return k_EUserHasLicenseResultNoAuth;

	return g_Steam->UserHasLicenseForApp(CSteamID(steamId), appId);
}

bool AIRSteam_RequestEncryptedAppTicket() {
	std::string data = g_Steam->get_bytearray();
	if (!g_Steam) return false;

	// non-const void *? Come on Valve, you already got it right in BeginAuthSession() ...
	void* buf = const_cast<char *>(data.data());
	return g_Steam->RequestEncryptedAppTicket(buf, static_cast<int>(data.length()));
}

bool AIRSteam_GetEncryptedAppTicket() {
	if (!g_Steam) return false;

	char* data = nullptr;
	uint32 length = 0;
	bool ret = g_Steam->GetEncryptedAppTicket(&data, &length);

	g_Steam->sendBuffer(AmfByteArray(data, data + length));
	delete[] data;

	return ret;
}

/*
 * overlay
 */
bool AIRSteam_ActivateGameOverlay() {
	std::string dialog = g_Steam->get_string();
	if (!g_Steam || dialog.empty()) return false;

	return g_Steam->ActivateGameOverlay(dialog);
}

bool AIRSteam_ActivateGameOverlayToUser() {
	std::string dialog = g_Steam->get_string();
	uint64 steamId = g_Steam->get_uint64();
	if (!g_Steam || dialog.empty() || steamId == 0) return false;

	return g_Steam->ActivateGameOverlayToUser(dialog, CSteamID(steamId));
}

bool AIRSteam_ActivateGameOverlayToWebPage() {
	std::string url = g_Steam->get_string();
	if (!g_Steam || url.empty()) return false;

	return g_Steam->ActivateGameOverlayToWebPage(url);
}

bool AIRSteam_ActivateGameOverlayToStore() {
	uint32 appId = g_Steam->get_int();
	uint32 flag = g_Steam->get_int();
	if (!g_Steam) return false;

	return g_Steam->ActivateGameOverlayToStore(appId,
		EOverlayToStoreFlag(flag));
}

bool AIRSteam_ActivateGameOverlayInviteDialog() {
	uint64 lobbyId = g_Steam->get_uint64();
	if (!g_Steam || lobbyId == 0) return false;

	return g_Steam->ActivateGameOverlayInviteDialog(CSteamID(lobbyId));
}

bool AIRSteam_IsOverlayEnabled() {
	if (!g_Steam) return false;

	return g_Steam->IsOverlayEnabled();
}

bool AIRSteam_SetOverlayNotificationPosition() {
	uint32 pos = g_Steam->get_int();
	if (!g_Steam) return false;

	return g_Steam->SetOverlayNotificationPosition(ENotificationPosition(pos));
}

/*
 * DLC / subscription
 */
bool AIRSteam_IsSubscribedApp() {
	uint32 appId = g_Steam->get_int();
	if (!g_Steam || appId == 0) return false;

	return g_Steam->IsSubscribedApp(appId);
}

bool AIRSteam_IsDLCInstalled() {
	uint32 appId = g_Steam->get_int();
	if (!g_Steam || appId == 0) return false;

	return g_Steam->IsDLCInstalled(appId);
}

int32 AIRSteam_GetDLCCount() {
	if (!g_Steam) return 0;

	return g_Steam->GetDLCCount();
}

bool AIRSteam_InstallDLC() {
	uint32 appId = g_Steam->get_int();
	if (!g_Steam || appId == 0) return false;

	return g_Steam->InstallDLC(appId);
}

bool AIRSteam_UninstallDLC() {
	uint32 appId = g_Steam->get_int();
	if (!g_Steam || appId == 0) return false;

	return g_Steam->UninstallDLC(appId);
}

bool AIRSteam_DLCInstalledResult() {
	if (!g_Steam) return 0;

	return g_Steam->DLCInstalledResult();
}

/*
 * Microtransaction
 */

// FIXME: sends either an object or null
std::nullptr_t AIRSteam_MicroTxnResult() {
	MicroTxnAuthorizationResponse_t res;
	if (!g_Steam || !g_Steam->MicroTxnResult(&res)) {
		g_Steam->send(AmfNull());
		return nullptr;
	}

	AmfObject obj("com.amanitadesign.steam.MicroTxnAuthorizationResponse", false, false);
	obj.addSealedProperty("appID", AmfInteger(res.m_unAppID));
	obj.addSealedProperty("orderID", AmfString(std::to_string(res.m_ulOrderID)));
	obj.addSealedProperty("authorized", AmfBool(res.m_bAuthorized));

	g_Steam->send(obj);
	return nullptr;
}

/*
 * other
 */

std::string AIRSteam_GetEnv() {
	std::string name = g_Steam->get_string();
	if (name.empty()) return "";

	const char* ret = ::getenv(name.c_str());
	return ret == nullptr ? "" : ret;
}

bool AIRSteam_SetEnv() {
	std::string name = g_Steam->get_string();
	std::string value = g_Steam->get_string();

	if (name.empty()) return false;

	int ret = ::setenv(name.c_str(), value.c_str(), 1);

	return ret == 0;
}
