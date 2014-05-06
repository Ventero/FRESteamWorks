/*
 *  APIWrapper.cpp
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

#include "APIWrapper.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>

#include "amf-cpp/types/amfbool.hpp"
#include "amf-cpp/types/amfdouble.hpp"
#include "amf-cpp/types/amfinteger.hpp"
#include "amf-cpp/serializer.hpp"

using namespace amf;

CLISteam* g_Steam = NULL;

void CLISteam::DispatchEvent(char* code, char* level) {
	// we abuse std::cerr for event dispatching, that way it doesn't interfere
	// with the normal communication on stdout
	std::cerr << "__event__<" << code << "," << level << ">" << std::endl;
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

bool AIRSteam_Init() {
	if (g_Steam) return true;

	if (!SteamAPI_Init()) return false;

	g_Steam = new CLISteam();

#ifdef WHITELIST
	uint32 appId = SteamUtils()->GetAppID();
	// WHITELIST is a comma separated list of app ids
	uint32 whitelist[] = { WHITELIST };
	bool found = false;
	for (auto id : whitelist) {
		if(id == appId) {
			found = true;
			break;
		}
	}

	if (!found) exit(1);
#endif

#ifdef DEBUG
	SteamUtils()->SetWarningMessageHook(steamWarningMessageHook);
#endif

	return true;
}

std::nullptr_t AIRSteam_RunCallbacks() {
	SteamAPI_RunCallbacks();
	return nullptr;
}

uint64 AIRSteam_GetUserID() {
	if (!g_Steam) return 0;;

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

// this is a void function even though the actual API function is a bool,
// because it's not immediately called when FRESteamWorks.useCrashHandler
// is called, as the APIWrapper process is not actually be running at that
// point in time. instead, this function is called as soon as the binary
// has started.
std::nullptr_t AIRSteam_UseCrashHandler() {
	uint32 appID = get_int();
	std::string version = get_string();
	std::string date = get_string();
	std::string time = get_string();
	if (!g_Steam) return nullptr;

	SteamAPI_SetBreakpadAppID(appID);
	SteamAPI_UseBreakpadCrashHandler(version.c_str(), date.c_str(), time.c_str(),
		false, NULL, NULL);

	return nullptr;
}

bool AIRSteam_RestartAppIfNecessary() {
	uint32 appID = get_int();
	bool ret = SteamAPI_RestartAppIfNecessary(appID);

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
	std::string name = get_string();
	bool ret = false;
	if (g_Steam && !name.empty()) {
		ret = g_Steam->SetAchievement(name);
	}

	SteamAPI_RunCallbacks();
	return ret;
}

bool AIRSteam_ClearAchievement() {
	std::string name = get_string();
	if(!g_Steam || name.empty()) return false;

	return g_Steam->ClearAchievement(name);
}

bool AIRSteam_IsAchievement() {
	std::string name = get_string();
	if (!g_Steam || name.empty()) return false;

	return g_Steam->IsAchievement(name);
}

int32 AIRSteam_GetStatInt() {
	std::string name = get_string();
	if (!g_Steam || name.empty()) return 0;

	int32 value = 0;
	g_Steam->GetStat(name, &value);
	return value;
}

float AIRSteam_GetStatFloat() {
	std::string name = get_string();
	if (!g_Steam || name.empty()) return 0.0f;

	float value = 0.0f;
	g_Steam->GetStat(name, &value);
	return value;
}

bool AIRSteam_SetStatInt() {
	std::string name = get_string();
	int32 value = get_int();
	if (!g_Steam || name.empty()) return false;

	return g_Steam->SetStat(name, value);
}

bool AIRSteam_SetStatFloat() {
	std::string name = get_string();
	float value = get_float();
	if(!g_Steam || name.empty()) return false;

	return g_Steam->SetStat(name, value);
}

bool AIRSteam_StoreStats() {
	if(!g_Steam) return false;

	return g_Steam->StoreStats();
}

bool AIRSteam_ResetAllStats() {
	bool achievementsToo = get_bool();
	if(!g_Steam) return false;

	return g_Steam->ResetAllStats(achievementsToo);
}

bool AIRSteam_RequestGlobalStats() {
	int days = get_int();
	if (!g_Steam) return false;

	return g_Steam->RequestGlobalStats(days);
}

// Since Flash doesn't have support for 64bit integer types, we just always
// return a double, which gets sent to Flash as an AmfDouble (aka Number).
double AIRSteam_GetGlobalStatInt() {
	std::string name = get_string();
	if (!g_Steam || name.empty()) return 0.;

	int64 value = 0;
	g_Steam->GetGlobalStat(name, &value);

	return static_cast<double>(value);
}

double AIRSteam_GetGlobalStatFloat() {
	std::string name = get_string();
	if (!g_Steam || name.empty()) return 0.0;

	double value = 0.0;
	g_Steam->GetGlobalStat(name, &value);

	return value;
}

// See above for why double is used.
AmfArray AIRSteam_GetGlobalStatHistoryInt() {
	AmfArray array;

	std::string name = get_string();
	uint32 days = get_int();
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

	std::string name = get_string();
	uint32 days = get_int();
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
	std::string name = get_string();
	if (!g_Steam || name.empty()) return false;

	return g_Steam->FindLeaderboard(name);
}

bool AIRSteam_FindOrCreateLeaderboard() {
	std::string name = get_string();
	uint32 sort = get_int();
	uint32 display = get_int();
	if (!g_Steam || name.empty()) return false;

	return g_Steam->FindOrCreateLeaderboard(name,
		ELeaderboardSortMethod(sort), ELeaderboardDisplayType(display));
}

SteamLeaderboard_t AIRSteam_FindLeaderboardResult() {
	if (!g_Steam) return 0;

	return g_Steam->FindLeaderboardResult();
}

std::string AIRSteam_GetLeaderboardName() {
	SteamLeaderboard_t handle = get_uint64();
	if (!g_Steam || handle == 0) return "";

	return g_Steam->GetLeaderboardName(handle);
}

int32 AIRSteam_GetLeaderboardEntryCount() {
	SteamLeaderboard_t handle = get_uint64();
	if (!g_Steam || handle == 0) return 0;

	return g_Steam->GetLeaderboardEntryCount(handle);
}

uint32 AIRSteam_GetLeaderboardSortMethod() {
	SteamLeaderboard_t handle = get_uint64();
	if (!g_Steam || handle == 0) return 0;

	return g_Steam->GetLeaderboardSortMethod(handle);
}

uint32 AIRSteam_GetLeaderboardDisplayType() {
	SteamLeaderboard_t handle = get_uint64();
	if (!g_Steam || handle == 0) return 0;

	return g_Steam->GetLeaderboardDisplayType(handle);
}

bool AIRSteam_UploadLeaderboardScore() {
	SteamLeaderboard_t handle = get_uint64();
	uint32 method = get_int();
	int32 score = get_int();
	std::vector<int32> details = get_array<int32>(get_int);

	if (!g_Steam) return false;

	return g_Steam->UploadLeaderboardScore(handle,
		ELeaderboardUploadScoreMethod(method), score,
		details.data(), details.size());
}

AmfObject AIRSteam_UploadLeaderboardScoreResult() {
	AmfObjectTraits traits("com.amanitadesign.steam.UploadLeaderboardScoreResult", false, false);
	AmfObject obj(traits);

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
	SteamLeaderboard_t handle = get_uint64();
	uint32 request = get_int();
	int rangeStart = get_int();
	int rangeEnd = get_int();

	if (!g_Steam) return false;

	return g_Steam->DownloadLeaderboardEntries(handle,
		ELeaderboardDataRequest(request), rangeStart, rangeEnd);
}

AmfArray AIRSteam_DownloadLeaderboardEntriesResult() {
	AmfArray array;

	int32 numDetails = get_int();
	if (!g_Steam) return array;

	auto entries = g_Steam->DownloadLeaderboardEntriesResult(numDetails);
	if (entries.empty()) return array;

	for (size_t i = 0; i < entries.size(); ++i) {
		AmfObjectTraits traits("com.amanitadesign.steam.LeaderboardEntry", false, false);
		AmfObject obj(traits);

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
	std::string name = get_string();
	if(!g_Steam || name.empty()) return 0;

	return g_Steam->GetFileSize(name);
}

bool AIRSteam_FileExists() {
	std::string name = get_string();
	if(!g_Steam || name.empty()) return false;

	return g_Steam->FileExists(name);
}

bool AIRSteam_FileWrite() {
	std::string name = get_string();
	std::string data = get_bytearray();
	if(!g_Steam || name.empty()) return false;

	return g_Steam->FileWrite(name, data.c_str(), data.length());
}

bool AIRSteam_FileRead() {
	std::string name = get_string();
	if(!g_Steam || name.empty()) return false;

	char* data = NULL;
	int32 size = g_Steam->FileRead(name, &data);
	if (size == 0) return false;

	send(true);
	Serializer serializer;
	serializer << AmfByteArray(data, data + size);
	sendDataTempFile(serializer);
	delete[] data;

	return true;
}

bool AIRSteam_FileDelete() {
	std::string name = get_string();
	if(!g_Steam || name.empty()) return false;

	return g_Steam->FileDelete(name);
}

bool AIRSteam_FileShare() {
	std::string name = get_string();
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
	bool enabled = get_bool();
	if(!g_Steam) return false;

	return g_Steam->SetCloudEnabledForApp(enabled);
}

AmfArray AIRSteam_GetQuota() {
	AmfArray array;

	if(!g_Steam) return array;

	int32 total, avail;
	if(!g_Steam->GetQuota(&total, &avail)) return array;

	array.push_back(AmfInteger(total));
	array.push_back(AmfInteger(avail));

	return array;
}

/*
 * ugc / workshop
 */
bool AIRSteam_UGCDownload() {
	UGCHandle_t handle = get_uint64();
	uint32 priority = get_int();
	if(!g_Steam || handle == 0) return false;

	return g_Steam->UGCDownload(handle, priority);
}

bool AIRSteam_UGCRead() {
	UGCHandle_t handle = get_uint64();
	int32 size = get_int();
	uint32 offset = get_int();

	if (!g_Steam || handle == 0 || size < 0) return false;

	char* data = nullptr;
	int32 result = 0;
	if (size > 0) {
		result = g_Steam->UGCRead(handle, size, offset, &data);
	}

	if(result == 0) return false;

	send(true);
	Serializer serializer;
	serializer << AmfByteArray(data, data + result);
	sendDataTempFile(serializer);
	delete[] data;

	return true;
}

AmfArray AIRSteam_GetUGCDownloadProgress() {
	AmfArray array;

	UGCHandle_t handle = get_uint64();
	if (!g_Steam || handle == 0) return array;

	int32 downloaded, expected;
	if(!g_Steam->GetUGCDownloadProgress(handle, &downloaded, &expected))
		return array;

	array.push_back(AmfInteger(downloaded));
	array.push_back(AmfInteger(expected));

	return array;
}

AmfObject AIRSteam_GetUGCDownloadResult() {
	AmfObjectTraits traits("com.amanitadesign.steam.DownloadUGCResult", false, false);
	AmfObject obj(traits);

	UGCHandle_t handle = get_uint64();
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
	std::string name = get_string();
	std::string preview = get_string();
	uint32 appId = get_int();
	std::string title = get_string();
	std::string description = get_string();
	uint32 visibility = get_int();

	std::vector<std::string> tags = get_string_array();
	SteamParamStringArray_t tagArray;
	createParamStringArray(tags, &tagArray);

	uint32 fileType = get_int();

	if (!g_Steam) return false;

	bool ret = g_Steam->PublishWorkshopFile(name, preview, appId, title, description,
		ERemoteStoragePublishedFileVisibility(visibility), &tagArray,
		EWorkshopFileType(fileType));

	delete[] tagArray.m_ppStrings;

	return ret;
}

PublishedFileId_t AIRSteam_PublishWorkshopFileResult() {
	if (!g_Steam) return 0;;

	return g_Steam->PublishWorkshopFileResult();
}

bool AIRSteam_DeletePublishedFile() {
	PublishedFileId_t handle = get_uint64();
	if (!g_Steam || handle == 0) return false;

	return g_Steam->DeletePublishedFile(handle);
}

bool AIRSteam_GetPublishedFileDetails() {
	PublishedFileId_t handle = get_uint64();
	uint32 maxAge = get_int();
	if (!g_Steam || handle == 0) return false;

	return g_Steam->GetPublishedFileDetails(handle, maxAge);
}

AmfObject AIRSteam_GetPublishedFileDetailsResult() {
	AmfObjectTraits traits("com.amanitadesign.steam.FileDetailsResult", false, false);
	AmfObject obj(traits);

	PublishedFileId_t file = get_uint64();
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
	uint32 startIndex = get_int();
	if (!g_Steam) return false;

	return g_Steam->EnumerateUserPublishedFiles(startIndex);
}

AmfObject AIRSteam_EnumerateUserPublishedFilesResult() {
	AmfObjectTraits traits("com.amanitadesign.steam.UserFilesResult", false, false);
	AmfObject obj(traits);

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
	uint32 type = get_int();
	uint32 start = get_int();
	uint32 count = get_int();
	uint32 days = get_int();

	std::vector<std::string> tags = get_string_array();
	std::vector<std::string> userTags = get_string_array();

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
	AmfObjectTraits traits("com.amanitadesign.steam.WorkshopFilesResult", false, false);
	AmfObject obj(traits);

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
	uint32 startIndex = get_int();
	if (!g_Steam) return false;

	return g_Steam->EnumerateUserSubscribedFiles(startIndex);
}

AmfObject AIRSteam_EnumerateUserSubscribedFilesResult() {
	AmfObjectTraits traits("com.amanitadesign.steam.SubscribedFilesResult", false, false);
	AmfObject obj(traits);

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
	uint64 steamID = get_uint64();
	uint32 start = get_int();

	std::vector<std::string> required = get_string_array();
	std::vector<std::string> excluded = get_string_array();

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
	AmfObjectTraits traits("com.amanitadesign.steam.UserFilesResult", false, false);
	AmfObject obj(traits);

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
	uint32 action = get_int();
	uint32 startIndex = get_int();
	if (!g_Steam) return false;

	return g_Steam->EnumeratePublishedFilesByUserAction(
		EWorkshopFileAction(action), startIndex);
}

AmfObject AIRSteam_EnumeratePublishedFilesByUserActionResult() {
	AmfObjectTraits traits("com.amanitadesign.steam.FilesByUserActionResult", false, false);
	AmfObject obj(traits);

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
	PublishedFileId_t handle = get_uint64();
	if (!g_Steam || handle == 0)
		return false;

	return g_Steam->SubscribePublishedFile(handle);
}

bool AIRSteam_UnsubscribePublishedFile() {
	PublishedFileId_t handle = get_uint64();
	if (!g_Steam || handle == 0)
		return false;

	return g_Steam->UnsubscribePublishedFile(handle);
}

PublishedFileUpdateHandle_t AIRSteam_CreatePublishedFileUpdateRequest() {
	PublishedFileId_t file = get_uint64();
	if (!g_Steam || file == 0)
		return k_PublishedFileUpdateHandleInvalid;

	return g_Steam->CreatePublishedFileUpdateRequest(file);
}

bool AIRSteam_UpdatePublishedFileFile() {
	PublishedFileId_t handle = get_uint64();
	std::string file = get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || file.empty())
		return false;

	return g_Steam->UpdatePublishedFileFile(handle, file);
}

bool AIRSteam_UpdatePublishedFilePreviewFile() {
	PublishedFileId_t handle = get_uint64();
	std::string preview = get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || preview.empty())
		return false;

	return g_Steam->UpdatePublishedFilePreviewFile(handle, preview);
}

bool AIRSteam_UpdatePublishedFileTitle() {
	PublishedFileId_t handle = get_uint64();
	std::string title = get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || title.empty())
		return false;

	return g_Steam->UpdatePublishedFileTitle(handle, title);
}

bool AIRSteam_UpdatePublishedFileDescription() {
	PublishedFileId_t handle = get_uint64();
	std::string description = get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || description.empty())
		return false;

	return g_Steam->UpdatePublishedFileDescription(handle, description);
}

bool AIRSteam_UpdatePublishedFileSetChangeDescription() {
	PublishedFileId_t handle = get_uint64();
	std::string changeDesc = get_string();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid || changeDesc.empty())
		return false;

	return g_Steam->UpdatePublishedFileSetChangeDescription(handle, changeDesc);
}

bool AIRSteam_UpdatePublishedFileVisibility() {
	PublishedFileId_t handle = get_uint64();
	uint32 visibility = get_int();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid)
		return false;

	return g_Steam->UpdatePublishedFileVisibility(handle,
		ERemoteStoragePublishedFileVisibility(visibility));
}

bool AIRSteam_UpdatePublishedFileTags() {
	PublishedFileUpdateHandle_t handle = get_uint64();

	std::vector<std::string> tags = get_string_array();
	SteamParamStringArray_t tagArray;
	createParamStringArray(tags, &tagArray);

	if (!g_Steam) return false;

	bool ret = g_Steam->UpdatePublishedFileTags(handle, &tagArray);

	delete[] tagArray.m_ppStrings;

	return ret;
}

bool AIRSteam_CommitPublishedFileUpdate() {
	PublishedFileId_t handle = get_uint64();
	if (!g_Steam || handle == k_PublishedFileUpdateHandleInvalid)
		return false;

	return g_Steam->CommitPublishedFileUpdate(handle);
}

bool AIRSteam_GetPublishedItemVoteDetails() {
	PublishedFileId_t file = get_uint64();
	if (!g_Steam || file == 0) return false;

	return g_Steam->GetPublishedItemVoteDetails(file);
}

AmfObject AIRSteam_GetPublishedItemVoteDetailsResult() {
	AmfObjectTraits traits("com.amanitadesign.steam.ItemVoteDetailsResult", false, false);
	AmfObject obj(traits);

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
	PublishedFileId_t file = get_uint64();
	if (!g_Steam || file == 0) return false;

	return g_Steam->GetUserPublishedItemVoteDetails(file);
}

AmfObject AIRSteam_GetUserPublishedItemVoteDetailsResult() {
	AmfObjectTraits traits("com.amanitadesign.steam.UserVoteDetails", false, false);
	AmfObject obj(traits);

	if (!g_Steam) return obj;

	auto details = g_Steam->GetUserPublishedItemVoteDetailsResult();
	if (!details) return obj;

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("publishedFileId", AmfString(std::to_string(details->m_nPublishedFileId)));
	obj.addSealedProperty("vote", AmfInteger(details->m_eVote));

	return obj;
}

bool AIRSteam_UpdateUserPublishedItemVote() {
	PublishedFileId_t file = get_uint64();
	bool upvote = get_bool();
	if (!g_Steam || file == 0) return false;

	return g_Steam->UpdateUserPublishedItemVote(file, upvote);
}

bool AIRSteam_SetUserPublishedFileAction() {
	PublishedFileId_t file = get_uint64();
	uint32 action = get_int();
	if (!g_Steam || file == 0) return false;

	return g_Steam->SetUserPublishedFileAction(file,
		EWorkshopFileAction(action));
}

/*
 * friends
 */

int AIRSteam_GetFriendCount() {
	uint32 flags = get_int();
	if (!g_Steam) return 0;

	return g_Steam->GetFriendCount(flags);
}

uint64 AIRSteam_GetFriendByIndex() {
	int index = get_int();
	uint32 flags = get_int();
	if (!g_Steam) return 0;

	return g_Steam->GetFriendByIndex(index, flags).ConvertToUint64();
}

std::string AIRSteam_GetFriendPersonaName() {
	uint64 steamId = get_uint64();
	if (!g_Steam) return "";

	return g_Steam->GetFriendPersonaName(CSteamID(steamId));
}

/*
 * authentication & ownership
 */

uint32 AIRSteam_GetAuthSessionTicket() {
	if (!g_Steam) return k_HAuthTicketInvalid;

	char* data = nullptr;
	uint32 length = 0;
	HAuthTicket ret = g_Steam->GetAuthSessionTicket(&data, &length);

	Serializer serializer;
	serializer << AmfByteArray(data, data + length);
	sendDataTempFile(serializer);

	delete[] data;

	return ret;
}

uint32 AIRSteam_GetAuthSessionTicketResult() {
	if (!g_Steam) return k_HAuthTicketInvalid;

	return g_Steam->GetAuthSessionTicketResult();
}

int AIRSteam_BeginAuthSession() {
	std::string data = get_bytearray();
	uint64 steamId = get_uint64();
	if (!g_Steam || data.empty()) return k_EBeginAuthSessionResultInvalidTicket;

	return g_Steam->BeginAuthSession(data.c_str(), data.length(), CSteamID(steamId));
}

bool AIRSteam_EndAuthSession() {
	uint64 steamId = get_uint64();
	if (!g_Steam) return false;

	return g_Steam->EndAuthSession(CSteamID(steamId));
}

bool AIRSteam_CancelAuthTicket() {
	uint32 ticketHandle = get_int();
	if (!g_Steam) return false;

	return g_Steam->CancelAuthTicket(ticketHandle);
}

int AIRSteam_UserHasLicenseForApp() {
	uint64 steamId = get_uint64();
	uint32 appId = get_int();
	if (!g_Steam) return k_EUserHasLicenseResultNoAuth;

	return g_Steam->UserHasLicenseForApp(CSteamID(steamId), appId);
}

/*
 * overlay
 */
bool AIRSteam_ActivateGameOverlay() {
	std::string dialog = get_string();
	if (!g_Steam || dialog.empty()) return false;

	return g_Steam->ActivateGameOverlay(dialog);
}

bool AIRSteam_ActivateGameOverlayToUser() {
	std::string dialog = get_string();
	uint64 steamId = get_uint64();
	if (!g_Steam || dialog.empty() || steamId == 0) return false;

	return g_Steam->ActivateGameOverlayToUser(dialog, CSteamID(steamId));
}

bool AIRSteam_ActivateGameOverlayToWebPage() {
	std::string url = get_string();
	if (!g_Steam || url.empty()) return false;

	return g_Steam->ActivateGameOverlayToWebPage(url);
}

bool AIRSteam_ActivateGameOverlayToStore() {
	uint32 appId = get_int();
	uint32 flag = get_int();
	if (!g_Steam) return false;

	return g_Steam->ActivateGameOverlayToStore(appId,
		EOverlayToStoreFlag(flag));
}

bool AIRSteam_ActivateGameOverlayInviteDialog() {
	uint64 lobbyId = get_uint64();
	if (!g_Steam || lobbyId == 0) return false;

	return g_Steam->ActivateGameOverlayInviteDialog(CSteamID(lobbyId));
}

bool AIRSteam_IsOverlayEnabled() {
	if (!g_Steam) return false;

	return g_Steam->IsOverlayEnabled();
}

/*
 * DLC / subscription
 */
bool AIRSteam_IsSubscribedApp() {
	uint32 appId = get_int();
	if (!g_Steam || appId == 0) return false;

	return g_Steam->IsSubscribedApp(appId);
}

bool AIRSteam_IsDLCInstalled() {
	uint32 appId = get_int();
	if (!g_Steam || appId == 0) return false;

	return g_Steam->IsDLCInstalled(appId);
}

int32 AIRSteam_GetDLCCount() {
	if (!g_Steam) return 0;

	return g_Steam->GetDLCCount();
}

bool AIRSteam_InstallDLC() {
	uint32 appId = get_int();
	if (!g_Steam || appId == 0) return false;

	return g_Steam->InstallDLC(appId);
}

bool AIRSteam_UninstallDLC() {
	uint32 appId = get_int();
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

uint64 AIRSteam_MicroTxnOrderIDResult() {
	if (!g_Steam) return 0;

	return g_Steam->MicroTxnOrderIDResult();
}

/*
 * other
 */

std::string AIRSteam_GetEnv() {
	std::string name = get_string();
	if (name.empty()) return "";

	const char* ret = std::getenv(name.c_str());
	return ret == nullptr ? "" : ret;
}

/*
 * Wrappers
 */

std::vector<std::function<void()>> apiFunctions {
#define X(a) []() { send(a()); },
#include "functions.h"
#undef X
};

int main(int argc, char** argv) {
	std::ios::sync_with_stdio(false);

	while(std::cin.good()) {
		std::string buf;
		std::getline(std::cin, buf);

		if(buf.empty()) break;

		unsigned int func;
		try {
			func = std::stoi(buf);

			if (func >= apiFunctions.size())
				continue;

			apiFunctions[func]();
		} catch (std::exception& e) {
			std::string msg("Exception: ");
			msg += e.what();
			msg += "\n";
			msg += buf;
			steamWarningMessageHook(2, msg.c_str());
			continue;
		} catch (...) {
			// shouldn't happen, just read on and hope for the best
			steamWarningMessageHook(2, "exception caught");
			continue;
		}
	}

	SteamAPI_Shutdown();
	delete g_Steam;
	return 0;
}
