/*
 *  CSteam.cpp
 *  This file is part of FRESteamWorks.
 *
 *  Created by David ´Oldes´ Oliva on 3/29/12.
 *  Contributors: Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012 Amanita Design. All rights reserved.
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

#include "CSteam.h"

#include <algorithm>
#include <stdio.h>
#include <sstream>

#include "ResponseTypes.h"

CSteam::CSteam():
	m_CurrentLeaderboard(0),
	m_FileHandle(k_UGCHandleInvalid),
	m_PublishedFileId(0),
	m_ActualAuthTicket(k_HAuthTicketInvalid),
	m_DLCInstalled(0),
	m_CallbackUserStatsReceived(this, &CSteam::OnUserStatsReceived),
	m_CallbackUserStatsStored(this, &CSteam::OnUserStatsStored),
	m_CallbackAchievementStored(this, &CSteam::OnAchievementStored),
	m_CallbackGetAuthSessionTicketResponse(this, &CSteam::OnGetAuthSessionTicketResponse),
	m_OnValidateAuthTicketResponse(this, &CSteam::OnValidateAuthTicketResponse),
	m_CallbackGameOverlayActivated(this, &CSteam::OnGameOverlayActivated),
	m_CallbackDLCInstalled(this, &CSteam::OnDLCInstalled),
	m_CallbackMicroTxnAuthorizationResponse(this, &CSteam::OnMicroTxnAuthorizationResponse)
{
	m_iAppID = SteamUtils()->GetAppID();
	m_bInitialized = (SteamUserStats() != NULL && SteamUser() != NULL);
	RequestStats();
}

CSteam::~CSteam() {}

bool CSteam::RequestStats() {
	// Is Steam loaded? If not we can't get stats.
	if (!m_bInitialized) return false;

	// Is the user logged on? If not we can't get stats.
	if (!SteamUser()->BLoggedOn()) return false;

	// Request user stats.
	return SteamUserStats()->RequestCurrentStats();
}

CSteamID CSteam::GetUserID() {
	if (!m_bInitialized) return k_steamIDNil;

	return SteamUser()->GetSteamID();
}

uint32 CSteam::GetAppID() {
	if (!m_bInitialized) return 0;

	return m_iAppID;
}

std::string CSteam::GetAvailableGameLanguages() {
	if (!m_bInitialized) return "";

	return std::string(SteamApps()->GetAvailableGameLanguages());
}

std::string CSteam::GetCurrentGameLanguage() {
	if (!m_bInitialized) return "";

	return std::string(SteamApps()->GetCurrentGameLanguage());
}

std::string CSteam::GetPersonaName() {
	if (!m_bInitialized) return "";

	return std::string(SteamFriends()->GetPersonaName());
}

// stats/achievements

bool CSteam::SetAchievement(std::string name) {
	if (!m_bInitialized) return false;

	SteamUserStats()->SetAchievement(name.c_str());
	return SteamUserStats()->StoreStats();
}

bool CSteam::ClearAchievement(std::string name) {
	if (!m_bInitialized) return false;

	SteamUserStats()->ClearAchievement(name.c_str());
	return SteamUserStats()->StoreStats();
}

bool CSteam::IsAchievement(std::string name) {
	bool result = false;
	if (m_bInitialized) SteamUserStats()->GetAchievement(name.c_str(), &result);
	return result;
}

bool CSteam::GetStat(std::string name, int32 *value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->GetStat(name.c_str(), value);
}

bool CSteam::GetStat(std::string name, float *value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->GetStat(name.c_str(), value);
}

bool CSteam::SetStat(std::string name, int32 value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->SetStat(name.c_str(), value);
}

bool CSteam::SetStat(std::string name, float value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->SetStat(name.c_str(), value);
}

bool CSteam::StoreStats() {
	if (!m_bInitialized) return false;

	return SteamUserStats()->StoreStats();
}

bool CSteam::ResetAllStats(bool bAchievementsToo) {
	if (!m_bInitialized) return false;

	SteamUserStats()->ResetAllStats(bAchievementsToo);
	return SteamUserStats()->StoreStats();
}

bool CSteam::RequestGlobalStats(int days) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamUserStats()->RequestGlobalStats(days);
	m_CallbackRequestGlobalStats.Set(result, this, &CSteam::OnRequestGlobalStats);

	return true;
}

bool CSteam::GetGlobalStat(std::string name, int64 *value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->GetGlobalStat(name.c_str(), value);
}

bool CSteam::GetGlobalStat(std::string name, double *value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->GetGlobalStat(name.c_str(), value);
}

std::vector<int64> CSteam::GetGlobalStatHistoryInt(std::string name, uint32 days) {
	if (!m_bInitialized) return std::vector<int64>();

	std::vector<int64> ret(days);
	int32 actual = SteamUserStats()->GetGlobalStatHistory(name.c_str(), ret.data(),
		days);
	ret.resize(actual);

	return ret;
}

std::vector<double> CSteam::GetGlobalStatHistoryFloat(std::string name, uint32 days) {
	if (!m_bInitialized) return std::vector<double>();

	std::vector<double> ret(days);
	int32 actual = SteamUserStats()->GetGlobalStatHistory(name.c_str(), ret.data(),
		days);
	ret.resize(actual);

	return ret;
}

// leaderboard
bool CSteam::FindLeaderboard(std::string name) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamUserStats()->FindLeaderboard(name.c_str());
	m_CallbackFindLeaderboard.Set(result, this, &CSteam::OnFindLeaderboard);

	return true;
}

bool CSteam::FindOrCreateLeaderboard(std::string name, ELeaderboardSortMethod sort,
		ELeaderboardDisplayType display) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamUserStats()->FindOrCreateLeaderboard(
		name.c_str(), sort, display);
	m_CallbackFindLeaderboard.Set(result, this, &CSteam::OnFindLeaderboard);

	return true;
}

SteamLeaderboard_t CSteam::FindLeaderboardResult() {
	if (!m_bInitialized || !m_CurrentLeaderboard) return 0;

	return m_CurrentLeaderboard;
}

std::string CSteam::GetLeaderboardName(SteamLeaderboard_t handle) {
	if (!m_bInitialized) return "";

	return SteamUserStats()->GetLeaderboardName(handle);
}

int CSteam::GetLeaderboardEntryCount(SteamLeaderboard_t handle) {
	if (!m_bInitialized) return 0;

	return SteamUserStats()->GetLeaderboardEntryCount(handle);
}

ELeaderboardSortMethod CSteam::GetLeaderboardSortMethod(SteamLeaderboard_t handle) {
	if (!m_bInitialized) return ELeaderboardSortMethod(0);

	return SteamUserStats()->GetLeaderboardSortMethod(handle);
}

ELeaderboardDisplayType CSteam::GetLeaderboardDisplayType(SteamLeaderboard_t handle) {
	if (!m_bInitialized) return ELeaderboardDisplayType(0);

	return SteamUserStats()->GetLeaderboardDisplayType(handle);
}


bool CSteam::UploadLeaderboardScore(SteamLeaderboard_t handle,
	ELeaderboardUploadScoreMethod method, int32 score,
	const int32* scoreDetails, int detailscount) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamUserStats()->UploadLeaderboardScore(handle,
		method, score, scoreDetails, detailscount);
	m_CallbackUploadLeaderboardScore.Set(result, this, &CSteam::OnUploadLeaderboardScore);

	return true;
}

LeaderboardScoreUploaded_t* CSteam::UploadLeaderboardScoreResult() {
	if (!m_bInitialized) return nullptr;

	return m_ScoreUpload.get();
}

bool CSteam::DownloadLeaderboardEntries(SteamLeaderboard_t handle,
	ELeaderboardDataRequest request,
	int rangeStart, int rangeEnd) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamUserStats()->DownloadLeaderboardEntries(handle,
		request, rangeStart, rangeEnd);
	m_CallbackDownloadLeaderboardEntries.Set(result, this, &CSteam::OnDownloadLeaderboardEntries);

	return true;
}

std::vector<LeaderboardEntry> CSteam::DownloadLeaderboardEntriesResult(int maxDetails) {
	std::vector<LeaderboardEntry> res;
	if (!m_bInitialized || m_ScoreDownloaded.get() == nullptr) return res;

	int cnt = m_ScoreDownloaded->m_cEntryCount;
	if (!cnt) return res;

	res.reserve(cnt);
	for (int i = 0; i < cnt; ++i) {
		LeaderboardEntry entry(maxDetails);
		SteamUserStats()->GetDownloadedLeaderboardEntry(m_ScoreDownloaded->m_hSteamLeaderboardEntries,
			i, &entry.entry, entry.details, maxDetails);

		res.push_back(entry);
	}

	return res;
}

// remote storage

int32 CSteam::GetFileCount() {
	if (!m_bInitialized) return 0;

	return SteamRemoteStorage()->GetFileCount();
}

int32 CSteam::GetFileSize(std::string name) {
	if (!m_bInitialized) return 0;

	return SteamRemoteStorage()->GetFileSize(name.c_str());
}

bool CSteam::FileExists(std::string name) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->FileExists(name.c_str());
}

bool CSteam::FileWrite(std::string name, const void* data, int32 length) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->FileWrite(name.c_str(), data, length);
}

// caller is responsible of deleting data when return value is != 0
int32 CSteam::FileRead(std::string name, char** content) {
	if (!m_bInitialized) return 0;

	int32 size = SteamRemoteStorage()->GetFileSize(name.c_str());
	if (size == 0) return 0;

	char* data = new char[size];
	int32 read = SteamRemoteStorage()->FileRead(name.c_str(), data, size);
	if(read == 0) {
		delete[] data;
		return 0;
	}

	*content = data;
	return read;
}

bool CSteam::FileDelete(std::string name) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->FileDelete(name.c_str());
}

bool CSteam::FileShare(std::string name) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->FileShare(name.c_str());
	m_CallbackFileShare.Set(result, this, &CSteam::OnFileShare);

	return true;
}

UGCHandle_t CSteam::FileShareResult() {
	return m_FileHandle;
}

bool CSteam::IsCloudEnabledForApp() {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->IsCloudEnabledForApp();
}

bool CSteam::SetCloudEnabledForApp(bool enabled) {
	if (!m_bInitialized) return false;

	SteamRemoteStorage()->SetCloudEnabledForApp(enabled);
	return enabled == SteamRemoteStorage()->IsCloudEnabledForApp();
}

bool CSteam::GetQuota(int32 *total, int32 *available) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->GetQuota(total, available);
}

bool CSteam::UGCDownload(UGCHandle_t handle, uint32 priority) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->UGCDownload(handle, priority);
	m_CallbackUGCDownload.Set(result, this, &CSteam::OnUGCDownload);

	return true;
}

int32 CSteam::UGCRead(UGCHandle_t handle, int32 size, uint32 offset, char **content) {
	if (!m_bInitialized) return 0;

	char *data = new char[size];
	// FIXME: add new parameter to API
	int32 read = SteamRemoteStorage()->UGCRead(handle, data, size, offset, k_EUGCRead_ContinueReadingUntilFinished);
	if (read == 0) {
		delete[] data;
		return 0;
	}

	*content = data;
	return read;
}

bool CSteam::GetUGCDownloadProgress(UGCHandle_t handle, int32 *downloaded, int32 *expected) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->GetUGCDownloadProgress(handle, downloaded, expected);
}

RemoteStorageDownloadUGCResult_t* CSteam::GetUGCDownloadResult(UGCHandle_t handle) {
	if (!m_bInitialized) return nullptr;

	auto it = m_DownloadResults.find(handle);
	if (it == m_DownloadResults.end())
		return nullptr;

	return &it->second;
}

bool CSteam::PublishWorkshopFile(std::string name, std::string preview,
	AppId_t nConsumerAppId, std::string title, std::string description,
	ERemoteStoragePublishedFileVisibility visibility,
	SteamParamStringArray_t *tags, EWorkshopFileType fileType) {

	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->PublishWorkshopFile(
		name.c_str(), preview.c_str(), nConsumerAppId, title.c_str(), description.c_str(),
		visibility, tags, fileType);
	m_CallbackPublishWorkshopFile.Set(result, this, &CSteam::OnPublishWorkshopFile);

	return true;
}

PublishedFileId_t CSteam::PublishWorkshopFileResult() {
	if (!m_bInitialized) return 0;

	return m_PublishedFileId;
}

bool CSteam::DeletePublishedFile(PublishedFileId_t file) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->DeletePublishedFile(file);
	m_CallbackDeletePublishedFile.Set(result, this, &CSteam::OnDeletePublishedFile);

	return true;
}

bool CSteam::GetPublishedFileDetails(PublishedFileId_t file, uint32 maxAge) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->GetPublishedFileDetails(file, maxAge);
	m_CallbackGetPublishedFileDetails.Set(result, this, &CSteam::OnGetPublishedFileDetails);

	return true;
}

RemoteStorageGetPublishedFileDetailsResult_t* CSteam::GetPublishedFileDetailsResult(PublishedFileId_t file) {
	if (!m_bInitialized) return nullptr;

	auto it = m_PublishedFileDetails.find(file);
	if (it == m_PublishedFileDetails.end())
		return nullptr;

	return &it->second;
}

bool CSteam::EnumerateUserPublishedFiles(uint32 startIndex) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->EnumerateUserPublishedFiles(startIndex);
	m_CallbackEnumerateUserPublishedFiles.Set(result, this, &CSteam::OnEnumerateUserPublishedFiles);

	return true;
}

RemoteStorageEnumerateUserPublishedFilesResult_t* CSteam::EnumerateUserPublishedFilesResult() {
	if (!m_bInitialized) return nullptr;

	return m_UserPublishedFiles.get();
}

bool CSteam::EnumeratePublishedWorkshopFiles(EWorkshopEnumerationType type, uint32 start, uint32 count,
		uint32 days, SteamParamStringArray_t *tags, SteamParamStringArray_t *userTags) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->EnumeratePublishedWorkshopFiles(
		type, start, count, days, tags, userTags);
	m_CallbackEnumeratePublishedWorkshopFiles.Set(result, this, &CSteam::OnEnumeratePublishedWorkshopFiles);

	return true;
}

RemoteStorageEnumerateWorkshopFilesResult_t* CSteam::EnumeratePublishedWorkshopFilesResult() {
	if (!m_bInitialized) return nullptr;

	return m_WorkshopFiles.get();
}

bool CSteam::EnumerateUserSubscribedFiles(uint32 startIndex) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->EnumerateUserSubscribedFiles(startIndex);
	m_CallbackEnumerateUserSubscribedFiles.Set(result, this, &CSteam::OnEnumerateUserSubscribedFiles);

	return true;
}

RemoteStorageEnumerateUserSubscribedFilesResult_t* CSteam::EnumerateUserSubscribedFilesResult() {
	if (!m_bInitialized) return nullptr;

	return m_SubscribedFiles.get();
}

bool CSteam::EnumerateUserSharedWorkshopFiles(CSteamID steamId, uint32 startIndex,
	SteamParamStringArray_t *requiredTags, SteamParamStringArray_t *excludedTags) {

	if(!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->EnumerateUserSharedWorkshopFiles(
		steamId, startIndex, requiredTags, excludedTags);
	m_CallbackEnumerateUserSharedWorkshopFiles.Set(result, this, &CSteam::OnEnumerateUserSharedWorkshopFiles);

	return true;
}

RemoteStorageEnumerateUserSharedWorkshopFilesResult_t* CSteam::EnumerateUserSharedWorkshopFilesResult() {
	if (!m_bInitialized) return nullptr;

	return m_UserSharedFiles.get();
}

bool CSteam::EnumeratePublishedFilesByUserAction(EWorkshopFileAction action, uint32 startIndex) {
	if(!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->EnumeratePublishedFilesByUserAction(
		action, startIndex);
	m_CallbackEnumeratePublishedFilesByUserAction.Set(result, this, &CSteam::OnEnumeratePublishedFilesByUserAction);

	return true;
}

RemoteStorageEnumeratePublishedFilesByUserActionResult_t* CSteam::EnumeratePublishedFilesByUserActionResult() {
	if(!m_bInitialized) return nullptr;

	return m_PublishedFilesByAction.get();
}

bool CSteam::SubscribePublishedFile(PublishedFileId_t file) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->SubscribePublishedFile(file);
	m_CallbackSubscribePublishedFile.Set(result, this, &CSteam::OnSubscribePublishedFile);

	return true;
}

bool CSteam::UnsubscribePublishedFile(PublishedFileId_t file) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->UnsubscribePublishedFile(file);
	m_CallbackUnsubscribePublishedFile.Set(result, this, &CSteam::OnUnsubscribePublishedFile);

	return true;
}

PublishedFileUpdateHandle_t CSteam::CreatePublishedFileUpdateRequest(PublishedFileId_t file) {
	if (!m_bInitialized) return k_PublishedFileUpdateHandleInvalid;

	return SteamRemoteStorage()->CreatePublishedFileUpdateRequest(file);
}

bool CSteam::UpdatePublishedFileFile(PublishedFileUpdateHandle_t handle, std::string file) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->UpdatePublishedFileFile(handle, file.c_str());
}

bool CSteam::UpdatePublishedFilePreviewFile(PublishedFileUpdateHandle_t handle, std::string preview) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->UpdatePublishedFilePreviewFile(handle, preview.c_str());
}

bool CSteam::UpdatePublishedFileTitle(PublishedFileUpdateHandle_t handle, std::string title) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->UpdatePublishedFileTitle(handle, title.c_str());
}

bool CSteam::UpdatePublishedFileDescription(PublishedFileUpdateHandle_t handle, std::string description) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->UpdatePublishedFileDescription(handle, description.c_str());
}

bool CSteam::UpdatePublishedFileSetChangeDescription(PublishedFileUpdateHandle_t handle, std::string changeDesc) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->UpdatePublishedFileSetChangeDescription(handle, changeDesc.c_str());
}

bool CSteam::UpdatePublishedFileVisibility(PublishedFileUpdateHandle_t handle, ERemoteStoragePublishedFileVisibility visibility) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->UpdatePublishedFileVisibility(handle, visibility);
}

bool CSteam::UpdatePublishedFileTags(PublishedFileUpdateHandle_t handle, SteamParamStringArray_t* tags) {
	if (!m_bInitialized) return false;

	return SteamRemoteStorage()->UpdatePublishedFileTags(handle, tags);
}

bool CSteam::CommitPublishedFileUpdate(PublishedFileUpdateHandle_t handle) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->CommitPublishedFileUpdate(handle);
	m_CallbackCommitPublishedFileUpdate.Set(result, this, &CSteam::OnCommitPublishedFileUpdate);

	return true;
}

bool CSteam::GetPublishedItemVoteDetails(PublishedFileId_t file) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->GetPublishedItemVoteDetails(file);
	m_CallbackGetPublishedItemVoteDetails.Set(result, this, &CSteam::OnGetPublishedItemVoteDetails);

	return true;
}

RemoteStorageGetPublishedItemVoteDetailsResult_t* CSteam::GetPublishedItemVoteDetailsResult() {
	if (!m_bInitialized) return nullptr;

	return m_PublishedItemVoteDetails.get();
}

bool CSteam::GetUserPublishedItemVoteDetails(PublishedFileId_t file) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->GetUserPublishedItemVoteDetails(file);
	m_CallbackGetUserPublishedItemVoteDetails.Set(result, this, &CSteam::OnGetUserPublishedItemVoteDetails);

	return true;
}

RemoteStorageUserVoteDetails_t* CSteam::GetUserPublishedItemVoteDetailsResult() {
	if (!m_bInitialized) return nullptr;

	return m_UserPublishedItemVoteDetails.get();
}

bool CSteam::UpdateUserPublishedItemVote(PublishedFileId_t file, bool upvote) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->UpdateUserPublishedItemVote(file, upvote);
	m_CallbackUpdateUserPublishedItemVote.Set(result, this, &CSteam::OnUpdateUserPublishedItemVote);

	return true;
}

bool CSteam::SetUserPublishedFileAction(PublishedFileId_t file, EWorkshopFileAction action) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->SetUserPublishedFileAction(file, action);
	m_CallbackSetUserPublishedFileAction.Set(result, this, &CSteam::OnSetUserPublishedFileAction);

	return true;
}

// friends
int CSteam::GetFriendCount(int flags) {
	if (!m_bInitialized) return 0;

	return SteamFriends()->GetFriendCount(flags);
}

CSteamID CSteam::GetFriendByIndex(int index, int flags) {
	if (!m_bInitialized) return k_steamIDNil;

	return SteamFriends()->GetFriendByIndex(index, flags);
}

std::string CSteam::GetFriendPersonaName(CSteamID steamId) {
	if (!m_bInitialized) return "";

	return std::string(SteamFriends()->GetFriendPersonaName(steamId));
}

// authentication & ownership
HAuthTicket CSteam::GetAuthSessionTicket(char** data, uint32* length) {
	if (!m_bInitialized) return k_HAuthTicketInvalid;

	// the docs don't state a maximum length anywhere, so just use what their
	// example app uses ...
	const int bufsize = 1024;
	char* buf = new char[bufsize];
	HAuthTicket ret = SteamUser()->GetAuthSessionTicket(buf, bufsize, length);

	*data = buf;
	return ret;
}

HAuthTicket CSteam::GetAuthSessionTicketResult() {
	if (!m_bInitialized) return k_HAuthTicketInvalid;

	return m_ActualAuthTicket;
}

EBeginAuthSessionResult CSteam::BeginAuthSession(const void* data, int length, CSteamID steamId) {
	if (!m_bInitialized) return k_EBeginAuthSessionResultInvalidTicket;

	return SteamUser()->BeginAuthSession(data, length, steamId);
}

bool CSteam::EndAuthSession(CSteamID steamId) {
	if (!m_bInitialized) return false;

	SteamUser()->EndAuthSession(steamId);
	return true;
}

bool CSteam::CancelAuthTicket(HAuthTicket handle) {
	if (!m_bInitialized) return false;

	SteamUser()->CancelAuthTicket(handle);
	return true;
}

EUserHasLicenseForAppResult CSteam::UserHasLicenseForApp(CSteamID steamId, AppId_t appId) {
	if (!m_bInitialized) return k_EUserHasLicenseResultNoAuth;

	return SteamUser()->UserHasLicenseForApp(steamId, appId);
}

// overlay
bool CSteam::ActivateGameOverlay(std::string dialog) {
	if (!m_bInitialized) return false;

	SteamFriends()->ActivateGameOverlay(dialog.c_str());
	return true;
}

bool CSteam::ActivateGameOverlayToUser(std::string dialog, CSteamID steamId) {
	if (!m_bInitialized) return false;

	SteamFriends()->ActivateGameOverlayToUser(dialog.c_str(), steamId);
	return true;
}

bool CSteam::ActivateGameOverlayToWebPage(std::string url) {
	if (!m_bInitialized) return false;

	SteamFriends()->ActivateGameOverlayToWebPage(url.c_str());
	return true;
}

bool CSteam::ActivateGameOverlayToStore(AppId_t appId, EOverlayToStoreFlag flag) {
	if (!m_bInitialized) return false;

	SteamFriends()->ActivateGameOverlayToStore(appId, flag);
	return true;
}

bool CSteam::ActivateGameOverlayInviteDialog(CSteamID lobbyId) {
	if (!m_bInitialized) return false;

	SteamFriends()->ActivateGameOverlayInviteDialog(lobbyId);
	return true;
}

bool CSteam::IsOverlayEnabled() {
	if (!m_bInitialized) return false;

	return SteamUtils()->IsOverlayEnabled();
}

bool CSteam::IsSubscribedApp(AppId_t appId) {
	if (!m_bInitialized) return false;

	return SteamApps()->BIsSubscribedApp(appId);
}

bool CSteam::IsDLCInstalled(AppId_t appId) {
	if (!m_bInitialized) return false;

	return SteamApps()->BIsDlcInstalled(appId);
}

int32 CSteam::GetDLCCount() {
	if (!m_bInitialized) return 0;

	return SteamApps()->GetDLCCount();
}

bool CSteam::InstallDLC(AppId_t appId) {
	if (!m_bInitialized) return false;

	SteamApps()->InstallDLC(appId);
	return true;
}

bool CSteam::UninstallDLC(AppId_t appId) {
	if (!m_bInitialized) return false;

	SteamApps()->UninstallDLC(appId);
	return true;
}

AppId_t CSteam::DLCInstalledResult() {
	return m_DLCInstalled;
}

bool CSteam::MicroTxnResult(MicroTxnAuthorizationResponse_t* out) {
	if (!m_MicroTxnResponses.empty()) {
		*out = m_MicroTxnResponses.front();
		m_MicroTxnResponses.pop();

		return true;
	}

	return false;
}

void CSteam::DispatchEvent(const int req_type, const int response) {
	char type[5];
	char value[5];

	snprintf(type, sizeof(type), "%d", req_type);
	snprintf(value, sizeof(value), "%d", response);
	DispatchEvent(type, value);
}

void CSteam::OnUserStatsReceived(UserStatsReceived_t *pCallback) {
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID != pCallback->m_nGameID) return;

	DispatchEvent(RESPONSE_OnUserStatsReceived, pCallback->m_eResult);
}

void CSteam::OnUserStatsStored(UserStatsStored_t *pCallback) {
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID != pCallback->m_nGameID) return;

	DispatchEvent(RESPONSE_OnUserStatsStored, pCallback->m_eResult);
}

void CSteam::OnAchievementStored(UserAchievementStored_t *pCallback) {
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID != pCallback->m_nGameID) return;

	DispatchEvent(RESPONSE_OnAchievementStored, k_EResultOK);
}

void CSteam::OnRequestGlobalStats(GlobalStatsReceived_t *result, bool failure) {
	if (m_iAppID != result->m_nGameID) return;

	DispatchEvent(RESPONSE_OnGlobalStatsReceived, result->m_eResult);
}

void CSteam::OnFindLeaderboard(LeaderboardFindResult_t *result, bool failure) {
	if (!failure) m_CurrentLeaderboard = result->m_hSteamLeaderboard;
	DispatchEvent(RESPONSE_OnFindLeaderboard, result->m_bLeaderboardFound ? k_EResultOK : k_EResultFail);
}

void CSteam::OnUploadLeaderboardScore(LeaderboardScoreUploaded_t *result, bool failure) {
	if (!failure) m_ScoreUpload.reset(new LeaderboardScoreUploaded_t(*result));
	DispatchEvent(RESPONSE_OnUploadLeaderboardScore, result->m_bSuccess ? k_EResultOK : k_EResultFail);
}

void CSteam::OnDownloadLeaderboardEntries(LeaderboardScoresDownloaded_t *result, bool failure) {
	if (!failure) m_ScoreDownloaded.reset(new LeaderboardScoresDownloaded_t(*result));
	DispatchEvent(RESPONSE_OnDownloadLeaderboardEntries, result->m_cEntryCount ? k_EResultOK : k_EResultFail);
}

void CSteam::OnFileShare(RemoteStorageFileShareResult_t *result, bool failure) {
	if (!failure) m_FileHandle = result->m_hFile;
	DispatchEvent(RESPONSE_OnFileShared, result->m_eResult);
}

void CSteam::OnUGCDownload(RemoteStorageDownloadUGCResult_t *result, bool failure) {
	// TODO: needs to be a map?
	if (!failure) m_DownloadResults[result->m_hFile] = *result;
	DispatchEvent(RESPONSE_OnUGCDownload, result->m_eResult);
}

void CSteam::OnPublishWorkshopFile(RemoteStoragePublishFileResult_t *result, bool failure) {
	if (!failure) m_PublishedFileId = result->m_nPublishedFileId;
	DispatchEvent(RESPONSE_OnPublishWorkshopFile, result->m_eResult);
}

void CSteam::OnDeletePublishedFile(RemoteStorageDeletePublishedFileResult_t *result, bool failure) {
	// we can ignore result->m_nPublishedFileId, as it had to be passed
	// to the call to DeletePublishedFile anyway
	DispatchEvent(RESPONSE_OnDeletePublishedFile, result->m_eResult);
}

void CSteam::OnGetPublishedFileDetails(RemoteStorageGetPublishedFileDetailsResult_t *result, bool failure) {
	// TODO: needs to be a map?
	if (!failure) m_PublishedFileDetails[result->m_nPublishedFileId] = *result;
	DispatchEvent(RESPONSE_OnGetPublishedFileDetails, result->m_eResult);
}

void CSteam::OnEnumerateUserPublishedFiles(RemoteStorageEnumerateUserPublishedFilesResult_t *result, bool failure) {
	if (!failure) m_UserPublishedFiles.reset(new RemoteStorageEnumerateUserPublishedFilesResult_t(*result));
	DispatchEvent(RESPONSE_OnEnumerateUserPublishedFiles, result->m_eResult);
}

void CSteam::OnEnumeratePublishedWorkshopFiles(RemoteStorageEnumerateWorkshopFilesResult_t *result, bool failure) {
	if (!failure) m_WorkshopFiles.reset(new RemoteStorageEnumerateWorkshopFilesResult_t(*result));
	DispatchEvent(RESPONSE_OnEnumeratePublishedWorkshopFiles, result->m_eResult);
}

void CSteam::OnEnumerateUserSubscribedFiles(RemoteStorageEnumerateUserSubscribedFilesResult_t *result, bool failure) {
	if (!failure) m_SubscribedFiles.reset(new RemoteStorageEnumerateUserSubscribedFilesResult_t(*result));
	DispatchEvent(RESPONSE_OnEnumerateUserSubscribedFiles, result->m_eResult);
}

void CSteam::OnEnumerateUserSharedWorkshopFiles(RemoteStorageEnumerateUserSharedWorkshopFilesResult_t *result, bool failure) {
	if (!failure) m_UserSharedFiles.reset(new RemoteStorageEnumerateUserSharedWorkshopFilesResult_t(*result));
	DispatchEvent(RESPONSE_OnEnumerateUserSharedWorkshopFiles, result->m_eResult);
}

void CSteam::OnEnumeratePublishedFilesByUserAction(RemoteStorageEnumeratePublishedFilesByUserActionResult_t *result, bool failure) {
	if (!failure) m_PublishedFilesByAction.reset(new RemoteStorageEnumeratePublishedFilesByUserActionResult_t(*result));
	DispatchEvent(RESPONSE_OnEnumeratePublishedFilesByUserAction, result->m_eResult);
}

void CSteam::OnCommitPublishedFileUpdate(RemoteStorageUpdatePublishedFileResult_t *result, bool failure) {
	// we can ignore m_nPublishedFileId again
	DispatchEvent(RESPONSE_OnCommitPublishedFileUpdate, result->m_eResult);
}

void CSteam::OnSubscribePublishedFile(RemoteStorageSubscribePublishedFileResult_t *result, bool failure) {
	// ignoring m_nPublishedFileId
	DispatchEvent(RESPONSE_OnSubscribePublishedFile, result->m_eResult);
}

void CSteam::OnUnsubscribePublishedFile(RemoteStorageUnsubscribePublishedFileResult_t *result, bool failure) {
	// ignoring m_nPublishedFileId
	DispatchEvent(RESPONSE_OnUnsubscribePublishedFile, result->m_eResult);
}

void CSteam::OnGetPublishedItemVoteDetails(RemoteStorageGetPublishedItemVoteDetailsResult_t *result, bool failure) {
	if (!failure) m_PublishedItemVoteDetails.reset(new RemoteStorageGetPublishedItemVoteDetailsResult_t(*result));
	DispatchEvent(RESPONSE_OnGetPublishedItemVoteDetails, result->m_eResult);
}

void CSteam::OnGetUserPublishedItemVoteDetails(RemoteStorageUserVoteDetails_t *result, bool failure) {
	if (!failure) m_UserPublishedItemVoteDetails.reset(new RemoteStorageUserVoteDetails_t(*result));
	DispatchEvent(RESPONSE_OnGetUserPublishedItemVoteDetails, result->m_eResult);
}

void CSteam::OnUpdateUserPublishedItemVote(RemoteStorageUpdateUserPublishedItemVoteResult_t *result, bool failure) {
	// ignoring m_nPublishedFileId
	DispatchEvent(RESPONSE_OnUpdateUserPublishedItemVote, result->m_eResult);
}

void CSteam::OnSetUserPublishedFileAction(RemoteStorageSetUserPublishedFileActionResult_t *result, bool failure) {
	// ignores m_nPublishedFileId and m_eAction?
	DispatchEvent(RESPONSE_OnSetUserPublishedFileAction, result->m_eResult);
}

void CSteam::OnGameOverlayActivated(GameOverlayActivated_t *pCallback) {
	DispatchEvent(RESPONSE_OnGameOverlayActivated, pCallback->m_bActive ? k_EResultOK : k_EResultFail);
}

void CSteam::OnGetAuthSessionTicketResponse(GetAuthSessionTicketResponse_t *pCallback) {
	m_ActualAuthTicket = pCallback->m_hAuthTicket;
	DispatchEvent(RESPONSE_OnGetAuthSessionTicketResponse, pCallback->m_eResult);
}

void CSteam::OnValidateAuthTicketResponse(ValidateAuthTicketResponse_t *pCallback) {
	// ignoring m_SteamID, m_OwnerSteamID;
	DispatchEvent(RESPONSE_OnValidateAuthTicketResponse, pCallback->m_eAuthSessionResponse);
}


void CSteam::OnDLCInstalled(DlcInstalled_t *pCallback) {
	m_DLCInstalled = pCallback->m_nAppID;
	DispatchEvent(RESPONSE_OnDLCInstalled, k_EResultOK);
}

void CSteam::OnMicroTxnAuthorizationResponse(MicroTxnAuthorizationResponse_t *pCallback) {
	// ignore callbacks for other games' transactions
	if (m_iAppID != pCallback->m_unAppID) return;
	m_MicroTxnResponses.push(*pCallback);
	DispatchEvent(RESPONSE_OnMicroTxnAuthorizationResponse, pCallback->m_bAuthorized ? k_EResultOK : k_EResultFail);
}

/*
 * utility functions
 */
void createParamStringArray(std::vector<std::string> tags,
	SteamParamStringArray_t* tagArray) {

	if (tags.empty()) {
		tagArray->m_nNumStrings = 0;
		tagArray->m_ppStrings = nullptr;
		return;
	}

	size_t arrayLength = tags.size();
	const char** tagstrings = new const char*[arrayLength];
	for (uint32 i = 0; i < arrayLength; ++i) {
		tagstrings[i] = tags[i].c_str();
	}

	tagArray->m_nNumStrings = arrayLength;
	tagArray->m_ppStrings = tagstrings;
}
