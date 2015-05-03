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
#include <sstream>
// For the low-level fd hackery in RestartAppIfNecessary.
#include <fcntl.h>
#include <unistd.h>

using namespace amf;

extern SteamWrapper* g_Steam;
extern void steamWarningMessageHook(int severity, const char* msg);

/*
 * utility functions for argument conversion
 */

template<typename T>
bool arg_check(const AmfArray& args, size_t index = 0) {
	if (index >= args.dense.size())
		return false;

	return args.dense.at(index).asPtr<T>() != nullptr;
}

template<typename T, typename T2, typename... Ts>
bool arg_check(const AmfArray& args, size_t index = 0) {
	return arg_check<T>(args, index) && arg_check<T2, Ts...>(args, index + 1);
}

uint64 to_uint64(const AmfString& str) {
	std::istringstream ss(str.value);

	uint64 val;
	if (!(ss >> val)) return 0;

	return val;
}

template<typename AmfType, typename T = decltype(AmfType().value)>
std::vector<T> to_vector(const AmfArray& args, size_t index) {
	const AmfItemPtr& elem = args.dense.at(index);
	if (elem.asPtr<AmfArray>() == nullptr)
		return {};

	const AmfArray& array = elem.as<AmfArray>();
	size_t len = array.dense.size();
	std::vector<T> res;
	res.reserve(len);

	for (size_t i = 0; i < len; ++i)
		res.push_back(array.at<AmfType>(i).value);

	return res;
}

/*
 * general functions
 */
bool AIRSteam_Init(const AmfArray& args) {
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

void AIRSteam_Shutdown(const AmfArray& args) {
	SteamAPI_Shutdown();
}

Void AIRSteam_RunCallbacks(const AmfArray& args) {
	SteamAPI_RunCallbacks();
	return {};
}

uint64 AIRSteam_GetUserID(const AmfArray& args) {
	if (!g_Steam) return 0;

	return g_Steam->GetUserID().ConvertToUint64();
}

uint32 AIRSteam_GetAppID(const AmfArray& args) {
	if (!g_Steam) return 0;

	return g_Steam->GetAppID();
}

std::string AIRSteam_GetAvailableGameLanguages(const AmfArray& args) {
	if (!g_Steam) return "";

	return g_Steam->GetAvailableGameLanguages();
}

std::string AIRSteam_GetCurrentGameLanguage(const AmfArray& args) {
	if (!g_Steam) return "";

	return g_Steam->GetCurrentGameLanguage();
}

std::string AIRSteam_GetPersonaName(const AmfArray& args) {
	if (!g_Steam) return "";

	return g_Steam->GetPersonaName();
}

bool AIRSteam_RestartAppIfNecessary(const AmfArray& args) {
	if (!arg_check<AmfInteger>(args))
		return false;

	uint32 appID = args.at<AmfInteger>(0).value;

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

bool AIRSteam_RequestStats(const AmfArray& args) {
	if (!g_Steam) return false;

	return g_Steam->RequestStats();
}

bool AIRSteam_SetAchievement(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	return g_Steam->SetAchievement(name);
}

bool AIRSteam_ClearAchievement(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	return g_Steam->ClearAchievement(name);
}

bool AIRSteam_IsAchievement(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	return g_Steam->IsAchievement(name);
}

bool AIRSteam_IndicateAchievementProgress(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfInteger, AmfInteger>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	uint32 current_progress = args.at<AmfInteger>(1).value;
	uint32 max_progress = args.at<AmfInteger>(2).value;

	return g_Steam->IndicateAchievementProgress(name, current_progress,
		max_progress);
}

int32 AIRSteam_GetStatInt(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return 0;

	std::string name = args.at<AmfString>(0).value;

	int32 value = 0;
	g_Steam->GetStat(name, &value);
	return value;
}

float AIRSteam_GetStatFloat(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return 0.0f;

	std::string name = args.at<AmfString>(0).value;

	float value = 0.0f;
	g_Steam->GetStat(name, &value);
	return value;
}

bool AIRSteam_SetStatInt(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfInteger>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	int32 value = args.at<AmfInteger>(1).value;

	return g_Steam->SetStat(name, value);
}

bool AIRSteam_SetStatFloat(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfDouble>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	double value = args.at<AmfDouble>(1).value;

	return g_Steam->SetStat(name, static_cast<float>(value));
}

bool AIRSteam_StoreStats(const AmfArray& args) {
	if (!g_Steam) return false;

	return g_Steam->StoreStats();
}

bool AIRSteam_ResetAllStats(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfBool>(args))
		return false;

	bool achievementsToo = args.at<AmfBool>(0).value;

	return g_Steam->ResetAllStats(achievementsToo);
}

bool AIRSteam_RequestGlobalStats(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger>(args))
		return false;

	int days = args.at<AmfInteger>(0).value;
	return g_Steam->RequestGlobalStats(days);
}

// Since Flash doesn't have support for 64bit integer types, we just always
// return a double, which gets sent to Flash as an AmfDouble (aka Number).
double AIRSteam_GetGlobalStatInt(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return 0.0;

	std::string name = args.at<AmfString>(0).value;
	int64 value = 0;
	g_Steam->GetGlobalStat(name, &value);

	return static_cast<double>(value);
}

double AIRSteam_GetGlobalStatFloat(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return 0.0;

	std::string name = args.at<AmfString>(0).value;
	double value = 0.0;
	g_Steam->GetGlobalStat(name, &value);

	return value;
}

// See above for why double is used.
AmfArray AIRSteam_GetGlobalStatHistoryInt(const AmfArray& args) {
	AmfArray array;
	if (!g_Steam || !arg_check<AmfString, AmfInteger>(args))
		return array;

	std::string name = args.at<AmfString>(0).value;
	uint32 days = args.at<AmfInteger>(1).value;

	auto history = g_Steam->GetGlobalStatHistoryInt(name, days);
	for (int64 entry : history) {
		array.push_back(AmfDouble(static_cast<double>(entry)));
	}

	return array;
}

AmfArray AIRSteam_GetGlobalStatHistoryFloat(const AmfArray& args) {
	AmfArray array;
	if (!g_Steam || !arg_check<AmfString, AmfInteger>(args))
		return array;

	std::string name = args.at<AmfString>(0).value;
	uint32 days = args.at<AmfInteger>(1).value;

	auto history = g_Steam->GetGlobalStatHistoryFloat(name, days);
	for (double entry : history) {
		array.push_back(AmfDouble(entry));
	}

	return array;
}

/*
 * leaderboards
 */

bool AIRSteam_FindLeaderboard(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	return g_Steam->FindLeaderboard(name);
}

bool AIRSteam_FindOrCreateLeaderboard(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfInteger, AmfInteger>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	uint32 sort = args.at<AmfInteger>(1).value;
	uint32 display = args.at<AmfInteger>(2).value;

	return g_Steam->FindOrCreateLeaderboard(name,
		ELeaderboardSortMethod(sort), ELeaderboardDisplayType(display));
}

SteamLeaderboard_t AIRSteam_FindLeaderboardResult(const AmfArray& args) {
	if (!g_Steam) return 0;

	return g_Steam->FindLeaderboardResult();
}

std::string AIRSteam_GetLeaderboardName(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return "";

	SteamLeaderboard_t handle = to_uint64(args.at<AmfString>(0));
	if (handle == 0) return "";

	return g_Steam->GetLeaderboardName(handle);
}

int32 AIRSteam_GetLeaderboardEntryCount(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return 0;

	SteamLeaderboard_t handle = to_uint64(args.at<AmfString>(0));
	if (handle == 0) return 0;

	return g_Steam->GetLeaderboardEntryCount(handle);
}

uint32 AIRSteam_GetLeaderboardSortMethod(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return 0;

	SteamLeaderboard_t handle = to_uint64(args.at<AmfString>(0));
	if (handle == 0) return 0;

	return g_Steam->GetLeaderboardSortMethod(handle);
}

uint32 AIRSteam_GetLeaderboardDisplayType(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return 0;

	SteamLeaderboard_t handle = to_uint64(args.at<AmfString>(0));
	if (handle == 0) return 0;

	return g_Steam->GetLeaderboardDisplayType(handle);
}

bool AIRSteam_UploadLeaderboardScore(const AmfArray& args) {
	// The check for AmfArray, which may be AmfNull is handled by to_vector
	if (!g_Steam || !arg_check<AmfString, AmfInteger, AmfInteger>(args))
		return false;

	SteamLeaderboard_t handle = to_uint64(args.at<AmfString>(0));
	uint32 method = args.at<AmfInteger>(1).value;
	int32 score = args.at<AmfInteger>(2).value;
	std::vector<int32> details = to_vector<AmfInteger>(args, 3);

	if (!g_Steam) return false;

	return g_Steam->UploadLeaderboardScore(handle,
		ELeaderboardUploadScoreMethod(method), score,
		details.data(), details.size());
}

AmfObject AIRSteam_UploadLeaderboardScoreResult(const AmfArray& args) {
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

bool AIRSteam_DownloadLeaderboardEntries(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfInteger, AmfInteger, AmfInteger>(args))
		return false;

	SteamLeaderboard_t handle = to_uint64(args.at<AmfString>(0));
	uint32 request = args.at<AmfInteger>(1).value;
	int rangeStart = args.at<AmfInteger>(2).value;
	int rangeEnd = args.at<AmfInteger>(3).value;

	return g_Steam->DownloadLeaderboardEntries(handle,
		ELeaderboardDataRequest(request), rangeStart, rangeEnd);
}

AmfArray AIRSteam_DownloadLeaderboardEntriesResult(const AmfArray& args) {
	AmfArray array;
	if (!g_Steam || !arg_check<AmfInteger>(args))
		return array;

	int32 numDetails = args.at<AmfInteger>(0).value;
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

int32 AIRSteam_GetFileCount(const AmfArray& args) {
	if (!g_Steam) return 0;

	return g_Steam->GetFileCount();
}

int32 AIRSteam_GetFileSize(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return 0;

	std::string name = args.at<AmfString>(0).value;
	return g_Steam->GetFileSize(name);
}

bool AIRSteam_FileExists(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	return g_Steam->FileExists(name);
}

#include <fstream>
bool AIRSteam_FileWrite(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfByteArray>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	const v8& data = args.at<AmfByteArray>(1).value;
	std::fstream f("/tmp/steam.log", std::ios::out | std::ios::app);
	f << "Writing to " << name << ": " << data.size() << std::endl;
	return g_Steam->FileWrite(name, data.data(), data.size());
}

bool AIRSteam_FileRead(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;

	char* data = NULL;
	int32 size = g_Steam->FileRead(name, &data);
	if (size == 0) return false;
	std::fstream f("/tmp/steam.log", std::ios::out | std::ios::app);
	f << "Read from " << name << ": " << size << std::endl;

	g_Steam->send(true);
	g_Steam->sendBuffer(AmfByteArray(data, data + size));
	delete[] data;

	return true;
}

bool AIRSteam_FileDelete(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	return g_Steam->FileDelete(name);
}

bool AIRSteam_FileShare(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	return g_Steam->FileShare(name);
}

UGCHandle_t AIRSteam_FileShareResult(const AmfArray& args) {
	if (!g_Steam) return k_UGCHandleInvalid;

	return g_Steam->FileShareResult();
}

bool AIRSteam_IsCloudEnabledForApp(const AmfArray& args) {
	if (!g_Steam) return false;

	return g_Steam->IsCloudEnabledForApp();
}

bool AIRSteam_SetCloudEnabledForApp(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfBool>(args))
		return false;

	bool enabled = args.at<AmfBool>(0).value;
	return g_Steam->SetCloudEnabledForApp(enabled);
}

AmfArray AIRSteam_GetQuota(const AmfArray& args) {
	AmfArray array;

	if (!g_Steam) return array;

	int32 total, avail;
	if (!g_Steam->GetQuota(&total, &avail)) return array;

	array.push_back(AmfInteger(total));
	array.push_back(AmfInteger(avail));

	return array;
}

/*
 * ugc / workshop
 */
bool AIRSteam_UGCDownload(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfInteger>(args))
		return false;

	UGCHandle_t handle = to_uint64(args.at<AmfString>(0));
	uint32 priority = args.at<AmfInteger>(1).value;
	if (handle == 0) return false;

	return g_Steam->UGCDownload(handle, priority);
}

bool AIRSteam_UGCRead(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfInteger, AmfInteger>(args))
		return false;

	UGCHandle_t handle = to_uint64(args.at<AmfString>(0));
	int32 size = args.at<AmfInteger>(1).value;
	uint32 offset = args.at<AmfInteger>(2).value;
	if (handle == 0 || size < 0) return false;

	char* data = nullptr;
	int32 result = 0;
	if (size <= 0) return false;

	result = g_Steam->UGCRead(handle, size, offset, &data);
	if (result <= 0) {
		delete[] data;
		return false;
	}

	g_Steam->send(true);
	g_Steam->sendBuffer(AmfByteArray(data, data + result));
	delete[] data;

	return true;
}

AmfArray AIRSteam_GetUGCDownloadProgress(const AmfArray& args) {
	AmfArray array;
	if (!g_Steam || !arg_check<AmfString>(args))
		return array;

	UGCHandle_t handle = to_uint64(args.at<AmfString>(0));
	if (handle == 0) return array;

	int32 downloaded, expected;
	if (!g_Steam->GetUGCDownloadProgress(handle, &downloaded, &expected))
		return array;

	array.push_back(AmfInteger(downloaded));
	array.push_back(AmfInteger(expected));

	return array;
}

AmfObject AIRSteam_GetUGCDownloadResult(const AmfArray& args) {
	AmfObject obj("com.amanitadesign.steam.DownloadUGCResult", false, false);

	if (!g_Steam || !arg_check<AmfString>(args))
		return obj;

	UGCHandle_t handle = to_uint64(args.at<AmfString>(0));
	if (handle == 0) return obj;

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

bool AIRSteam_PublishWorkshopFile(const AmfArray& args) {
	if (!g_Steam) return false;

	// The check for argument #6 is handled by to_vector.
	if (!arg_check<AmfString, AmfString, AmfInteger, AmfString, AmfString, AmfInteger>(args))
		return false;

	if (!arg_check<AmfInteger>(args, 7))
		return false;

	std::string name = args.at<AmfString>(0).value;
	std::string preview = args.at<AmfString>(1).value;
	uint32 appId = args.at<AmfInteger>(2).value;
	std::string title = args.at<AmfString>(3).value;
	std::string description = args.at<AmfString>(4).value;
	uint32 visibility = args.at<AmfInteger>(5).value;

	std::vector<std::string> tags = to_vector<AmfString>(args, 6);
	SteamParamStringArray_t tagArray;
	createParamStringArray(tags, &tagArray);

	uint32 fileType = args.at<AmfInteger>(7).value;

	bool ret = g_Steam->PublishWorkshopFile(name, preview, appId, title, description,
		ERemoteStoragePublishedFileVisibility(visibility), &tagArray,
		EWorkshopFileType(fileType));

	delete[] tagArray.m_ppStrings;

	return ret;
}

PublishedFileId_t AIRSteam_PublishWorkshopFileResult(const AmfArray& args) {
	if (!g_Steam) return 0;

	return g_Steam->PublishWorkshopFileResult();
}

bool AIRSteam_DeletePublishedFile(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	PublishedFileId_t handle = to_uint64(args.at<AmfString>(0));
	if (handle == 0) return false;

	return g_Steam->DeletePublishedFile(handle);
}

bool AIRSteam_GetPublishedFileDetails(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	PublishedFileId_t handle = to_uint64(args.at<AmfString>(0));
	uint32 maxAge = args.at<AmfInteger>(1).value;
	if (handle == 0) return false;

	return g_Steam->GetPublishedFileDetails(handle, maxAge);
}

AmfObject AIRSteam_GetPublishedFileDetailsResult(const AmfArray& args) {
	AmfObject obj("com.amanitadesign.steam.FileDetailsResult", false, false);
	if (!g_Steam || !arg_check<AmfString>(args))
		return obj;

	PublishedFileId_t file = to_uint64(args.at<AmfString>(0));
	if (file == 0) return obj;

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

bool AIRSteam_EnumerateUserPublishedFiles(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger>(args))
		return false;

	uint32 startIndex = args.at<AmfInteger>(0).value;
	return g_Steam->EnumerateUserPublishedFiles(startIndex);
}

AmfObject AIRSteam_EnumerateUserPublishedFilesResult(const AmfArray& args) {
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

bool AIRSteam_EnumeratePublishedWorkshopFiles(const AmfArray& args) {
	// Arguments #4 and #5 are checked by to_vector.
	if (!g_Steam || !arg_check<AmfInteger, AmfInteger, AmfInteger, AmfInteger>(args))
		return false;

	uint32 type = args.at<AmfInteger>(0).value;
	uint32 start = args.at<AmfInteger>(1).value;
	uint32 count = args.at<AmfInteger>(2).value;
	uint32 days = args.at<AmfInteger>(3).value;

	std::vector<std::string> tags = to_vector<AmfString>(args, 4);
	std::vector<std::string> userTags = to_vector<AmfString>(args, 5);

	SteamParamStringArray_t tagArray, userTagArray;
	createParamStringArray(tags, &tagArray);
	createParamStringArray(userTags, &userTagArray);

	bool ret = g_Steam->EnumeratePublishedWorkshopFiles(
		EWorkshopEnumerationType(type), start, count, days, &tagArray, &userTagArray);

	delete[] tagArray.m_ppStrings;
	delete[] userTagArray.m_ppStrings;

	return ret;
}

AmfObject AIRSteam_EnumeratePublishedWorkshopFilesResult(const AmfArray& args) {
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

bool AIRSteam_EnumerateUserSubscribedFiles(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger>(args))
		return false;

	uint32 startIndex = args.at<AmfInteger>(0).value;
	return g_Steam->EnumerateUserSubscribedFiles(startIndex);
}

AmfObject AIRSteam_EnumerateUserSubscribedFilesResult(const AmfArray& args) {
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

bool AIRSteam_EnumerateUserSharedWorkshopFiles(const AmfArray& args) {
	// Arguments #2 and #3 are checked by to_vector
	if (!g_Steam || !arg_check<AmfString, AmfInteger>(args))
		return false;

	uint64 steamID = to_uint64(args.at<AmfString>(0));
	uint32 start = args.at<AmfInteger>(1).value;

	std::vector<std::string> required = to_vector<AmfString>(args, 2);
	std::vector<std::string> excluded = to_vector<AmfString>(args, 3);

	SteamParamStringArray_t requiredArray, excludedArray;
	createParamStringArray(required, &requiredArray);
	createParamStringArray(excluded, &excludedArray);

	bool ret = g_Steam->EnumerateUserSharedWorkshopFiles(steamID, start,
		&requiredArray, &excludedArray);

	delete[] requiredArray.m_ppStrings;
	delete[] excludedArray.m_ppStrings;

	return ret;
}

AmfObject AIRSteam_EnumerateUserSharedWorkshopFilesResult(const AmfArray& args) {
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

bool AIRSteam_EnumeratePublishedFilesByUserAction(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger, AmfInteger>(args))
		return false;

	uint32 action = args.at<AmfInteger>(0).value;
	uint32 startIndex = args.at<AmfInteger>(1).value;
	return g_Steam->EnumeratePublishedFilesByUserAction(
		EWorkshopFileAction(action), startIndex);
}

AmfObject AIRSteam_EnumeratePublishedFilesByUserActionResult(const AmfArray& args) {
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

bool AIRSteam_SubscribePublishedFile(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	PublishedFileId_t handle = to_uint64(args.at<AmfString>(0));
	if (handle == 0) return false;

	return g_Steam->SubscribePublishedFile(handle);
}

bool AIRSteam_UnsubscribePublishedFile(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	PublishedFileId_t handle = to_uint64(args.at<AmfString>(0));
	if (handle == 0) return false;

	return g_Steam->UnsubscribePublishedFile(handle);
}

PublishedFileUpdateHandle_t AIRSteam_CreatePublishedFileUpdateRequest(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return k_PublishedFileUpdateHandleInvalid;

	PublishedFileId_t file = to_uint64(args.at<AmfString>(0));
	if (file == 0) return k_PublishedFileUpdateHandleInvalid;

	return g_Steam->CreatePublishedFileUpdateRequest(file);
}

bool AIRSteam_UpdatePublishedFileFile(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfString>(args))
		return false;

	PublishedFileId_t handle = to_uint64(args.at<AmfString>(0));
	std::string file = args.at<AmfString>(1).value;
	if (handle == k_PublishedFileUpdateHandleInvalid)
		return false;

	return g_Steam->UpdatePublishedFileFile(handle, file);
}

bool AIRSteam_UpdatePublishedFilePreviewFile(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfString>(args))
		return false;

	PublishedFileId_t handle = to_uint64(args.at<AmfString>(0));
	std::string preview = args.at<AmfString>(1).value;
	if (handle == k_PublishedFileUpdateHandleInvalid)
		return false;

	return g_Steam->UpdatePublishedFilePreviewFile(handle, preview);
}

bool AIRSteam_UpdatePublishedFileTitle(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfString>(args))
		return false;

	PublishedFileId_t handle = to_uint64(args.at<AmfString>(0));
	std::string title = args.at<AmfString>(1).value;
	if (handle == k_PublishedFileUpdateHandleInvalid)
		return false;

	return g_Steam->UpdatePublishedFileTitle(handle, title);
}

bool AIRSteam_UpdatePublishedFileDescription(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfString>(args))
		return false;

	PublishedFileId_t handle = to_uint64(args.at<AmfString>(0));
	std::string description = args.at<AmfString>(1).value;
	if (handle == k_PublishedFileUpdateHandleInvalid)
		return false;

	return g_Steam->UpdatePublishedFileDescription(handle, description);
}

bool AIRSteam_UpdatePublishedFileSetChangeDescription(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfString>(args))
		return false;

	PublishedFileId_t handle = to_uint64(args.at<AmfString>(0));
	std::string changeDesc = args.at<AmfString>(1).value;
	if (handle == k_PublishedFileUpdateHandleInvalid)
		return false;

	return g_Steam->UpdatePublishedFileSetChangeDescription(handle, changeDesc);
}

bool AIRSteam_UpdatePublishedFileVisibility(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfInteger>(args))
		return false;

	PublishedFileId_t handle = to_uint64(args.at<AmfString>(0));
	uint32 visibility = args.at<AmfInteger>(1).value;
	if (handle == k_PublishedFileUpdateHandleInvalid)
		return false;

	return g_Steam->UpdatePublishedFileVisibility(handle,
		ERemoteStoragePublishedFileVisibility(visibility));
}

bool AIRSteam_UpdatePublishedFileTags(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	PublishedFileUpdateHandle_t handle = to_uint64(args.at<AmfString>(0));

	std::vector<std::string> tags = to_vector<AmfString>(args, 1);
	SteamParamStringArray_t tagArray;
	createParamStringArray(tags, &tagArray);

	bool ret = g_Steam->UpdatePublishedFileTags(handle, &tagArray);

	delete[] tagArray.m_ppStrings;
	return ret;
}

bool AIRSteam_CommitPublishedFileUpdate(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	PublishedFileId_t handle = to_uint64(args.at<AmfString>(0));
	if (handle == k_PublishedFileUpdateHandleInvalid)
		return false;

	return g_Steam->CommitPublishedFileUpdate(handle);
}

bool AIRSteam_GetPublishedItemVoteDetails(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	PublishedFileId_t file = to_uint64(args.at<AmfString>(0));
	if (file == 0) return false;

	return g_Steam->GetPublishedItemVoteDetails(file);
}

AmfObject AIRSteam_GetPublishedItemVoteDetailsResult(const AmfArray& args) {
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

bool AIRSteam_GetUserPublishedItemVoteDetails(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	PublishedFileId_t file = to_uint64(args.at<AmfString>(0));
	if (file == 0) return false;

	return g_Steam->GetUserPublishedItemVoteDetails(file);
}

AmfObject AIRSteam_GetUserPublishedItemVoteDetailsResult(const AmfArray& args) {
	AmfObject obj("com.amanitadesign.steam.UserVoteDetails", false, false);
	if (!g_Steam) return obj;

	auto details = g_Steam->GetUserPublishedItemVoteDetailsResult();
	if (!details) return obj;

	obj.addSealedProperty("result", AmfInteger(details->m_eResult));
	obj.addSealedProperty("publishedFileId", AmfString(std::to_string(details->m_nPublishedFileId)));
	obj.addSealedProperty("vote", AmfInteger(details->m_eVote));

	return obj;
}

bool AIRSteam_UpdateUserPublishedItemVote(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfBool>(args))
		return false;

	PublishedFileId_t file = to_uint64(args.at<AmfString>(0));
	bool upvote = args.at<AmfBool>(1).value;
	if (file == 0) return false;

	return g_Steam->UpdateUserPublishedItemVote(file, upvote);
}

bool AIRSteam_SetUserPublishedFileAction(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfInteger>(args))
		return false;

	PublishedFileId_t file = to_uint64(args.at<AmfString>(0));
	uint32 action = args.at<AmfInteger>(1).value;
	if (file == 0) return false;

	return g_Steam->SetUserPublishedFileAction(file,
		EWorkshopFileAction(action));
}

/*
 * friends
 */

int AIRSteam_GetFriendCount(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger>(args))
		return 0;

	uint32 flags = args.at<AmfInteger>(0).value;
	return g_Steam->GetFriendCount(flags);
}

uint64 AIRSteam_GetFriendByIndex(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger, AmfInteger>(args))
		return 0;

	int index = args.at<AmfInteger>(0).value;
	uint32 flags = args.at<AmfInteger>(1).value;
	return g_Steam->GetFriendByIndex(index, flags).ConvertToUint64();
}

std::string AIRSteam_GetFriendPersonaName(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return "";

	uint64 steamId = to_uint64(args.at<AmfString>(0));
	return g_Steam->GetFriendPersonaName(CSteamID(steamId));
}

/*
 * authentication & ownership
 */
uint32 AIRSteam_GetAuthSessionTicket(const AmfArray& args) {
	if (!g_Steam) return k_HAuthTicketInvalid;

	char* data = nullptr;
	uint32 length = 0;
	HAuthTicket ret = g_Steam->GetAuthSessionTicket(&data, &length);

	g_Steam->sendBuffer(AmfByteArray(data, data + length));
	delete[] data;

	return ret;
}

uint32 AIRSteam_GetAuthSessionTicketResult(const AmfArray& args) {
	if (!g_Steam) return k_HAuthTicketInvalid;

	return g_Steam->GetAuthSessionTicketResult();
}

int AIRSteam_BeginAuthSession(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfByteArray, AmfString>(args))
		return k_EBeginAuthSessionResultInvalidTicket;

	const v8& data = args.at<AmfByteArray>(0).value;
	uint64 steamId = to_uint64(args.at<AmfString>(1));
	return g_Steam->BeginAuthSession(data.data(), data.size(), CSteamID(steamId));
}

bool AIRSteam_EndAuthSession(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	uint64 steamId = to_uint64(args.at<AmfString>(0));
	return g_Steam->EndAuthSession(CSteamID(steamId));
}

bool AIRSteam_CancelAuthTicket(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger>(args))
		return false;

	uint32 ticketHandle = args.at<AmfInteger>(0).value;
	return g_Steam->CancelAuthTicket(ticketHandle);
}

int AIRSteam_UserHasLicenseForApp(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfInteger>(args))
		return k_EUserHasLicenseResultNoAuth;

	uint64 steamId = to_uint64(args.at<AmfString>(0));
	uint32 appId = args.at<AmfInteger>(1).value;
	return g_Steam->UserHasLicenseForApp(CSteamID(steamId), appId);
}

/*
 * overlay
 */
bool AIRSteam_ActivateGameOverlay(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	std::string dialog = args.at<AmfString>(0).value;
	return g_Steam->ActivateGameOverlay(dialog);
}

bool AIRSteam_ActivateGameOverlayToUser(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfString>(args))
		return false;

	std::string dialog = args.at<AmfString>(0).value;
	uint64 steamId = to_uint64(args.at<AmfString>(1));
	if (steamId == 0) return false;

	return g_Steam->ActivateGameOverlayToUser(dialog, CSteamID(steamId));
}

bool AIRSteam_ActivateGameOverlayToWebPage(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	std::string url = args.at<AmfString>(0).value;
	return g_Steam->ActivateGameOverlayToWebPage(url);
}

bool AIRSteam_ActivateGameOverlayToStore(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger, AmfInteger>(args))
		return false;

	uint32 appId = args.at<AmfInteger>(0).value;
	uint32 flag = args.at<AmfInteger>(1).value;
	return g_Steam->ActivateGameOverlayToStore(appId,
		EOverlayToStoreFlag(flag));
}

bool AIRSteam_ActivateGameOverlayInviteDialog(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return false;

	uint64 lobbyId = to_uint64(args.at<AmfString>(0));
	if (lobbyId == 0) return false;

	return g_Steam->ActivateGameOverlayInviteDialog(CSteamID(lobbyId));
}

bool AIRSteam_IsOverlayEnabled(const AmfArray& args) {
	if (!g_Steam) return false;

	return g_Steam->IsOverlayEnabled();
}

bool AIRSteam_SetOverlayNotificationPosition(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger>(args))
		return false;

	uint32 pos = args.at<AmfInteger>(0).value;
	return g_Steam->SetOverlayNotificationPosition(ENotificationPosition(pos));
}

/*
 * DLC / subscription
 */
bool AIRSteam_IsSubscribedApp(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger>(args))
		return false;

	uint32 appId = args.at<AmfInteger>(0).value;
	return g_Steam->IsSubscribedApp(appId);
}

bool AIRSteam_IsDLCInstalled(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger>(args))
		return false;

	uint32 appId = args.at<AmfInteger>(0).value;
	return g_Steam->IsDLCInstalled(appId);
}

int32 AIRSteam_GetDLCCount(const AmfArray& args) {
	if (!g_Steam) return 0;

	return g_Steam->GetDLCCount();
}

bool AIRSteam_InstallDLC(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger>(args))
		return false;

	uint32 appId = args.at<AmfInteger>(0).value;
	return g_Steam->InstallDLC(appId);
}

bool AIRSteam_UninstallDLC(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfInteger>(args))
		return false;

	uint32 appId = args.at<AmfInteger>(0).value;
	return g_Steam->UninstallDLC(appId);
}

bool AIRSteam_DLCInstalledResult(const AmfArray& args) {
	if (!g_Steam) return 0;

	return g_Steam->DLCInstalledResult();
}

/*
 * Microtransaction
 */

// FIXME: sends either an object or null
Void AIRSteam_MicroTxnResult(const AmfArray& args) {
	MicroTxnAuthorizationResponse_t res;
	if (!g_Steam || !g_Steam->MicroTxnResult(&res)) {
		g_Steam->send(AmfNull());
		return {};
	}

	AmfObject obj("com.amanitadesign.steam.MicroTxnAuthorizationResponse", false, false);
	obj.addSealedProperty("appID", AmfInteger(res.m_unAppID));
	obj.addSealedProperty("orderID", AmfString(std::to_string(res.m_ulOrderID)));
	obj.addSealedProperty("authorized", AmfBool(res.m_bAuthorized));

	g_Steam->send(obj);
	return {};
}

/*
 * other
 */

std::string AIRSteam_GetEnv(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString>(args))
		return "";

	std::string name = args.at<AmfString>(0).value;

	const char* ret = ::getenv(name.c_str());
	return ret == nullptr ? "" : ret;
}

bool AIRSteam_SetEnv(const AmfArray& args) {
	if (!g_Steam || !arg_check<AmfString, AmfString>(args))
		return false;

	std::string name = args.at<AmfString>(0).value;
	std::string value = args.at<AmfString>(0).value;

	int ret = ::setenv(name.c_str(), value.c_str(), 1);
	return ret == 0;
}
