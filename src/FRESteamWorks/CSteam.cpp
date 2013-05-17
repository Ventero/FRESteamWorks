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

CSteam::CSteam():
	m_FileHandle(k_UGCHandleInvalid),
	m_PublishedFileId(0),
	m_UserPublishedFiles(nullptr),
	m_WorkshopFiles(nullptr),
	m_SubscribedFiles(nullptr),
	m_UserSharedFiles(nullptr),
	m_PublishedFilesByAction(nullptr),
	m_PublishedItemVoteDetails(nullptr),
	m_CallbackUserStatsReceived(this, &CSteam::OnUserStatsReceived),
	m_CallbackUserStatsStored(this, &CSteam::OnUserStatsStored),
	m_CallbackAchievementStored(this, &CSteam::OnAchievementStored),
	m_CallbackGameOverlayActivated(this, &CSteam::OnGameOverlayActivated)
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

std::string CSteam::GetUserID() {
	if (!m_bInitialized) return "";

	uint64 id = SteamUser()->GetSteamID().ConvertToUint64();
	std::stringstream stream;
	stream << id;
	return stream.str();
}

uint32 CSteam::GetAppID() {
	if (!m_bInitialized) return 0;

	return m_iAppID;
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
		delete data;
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
	int32 read = SteamRemoteStorage()->UGCRead(handle, data, size, offset);
	if (read == 0) {
		delete data;
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

	return m_DownloadResults[handle];
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

bool CSteam::GetPublishedFileDetails(PublishedFileId_t file) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->GetPublishedFileDetails(file);
	m_CallbackGetPublishedFileDetails.Set(result, this, &CSteam::OnGetPublishedFileDetails);

	return true;
}

RemoteStorageGetPublishedFileDetailsResult_t* CSteam::GetPublishedFileDetailsResult(PublishedFileId_t file) {
	if (!m_bInitialized) return nullptr;

	return m_PublishedFileDetails[file];
}

bool CSteam::EnumerateUserPublishedFiles(uint32 startIndex) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->EnumerateUserPublishedFiles(startIndex);
	m_CallbackEnumerateUserPublishedFiles.Set(result, this, &CSteam::OnEnumerateUserPublishedFiles);

	return true;
}

RemoteStorageEnumerateUserPublishedFilesResult_t* CSteam::EnumerateUserPublishedFilesResult() {
	if (!m_bInitialized) return nullptr;

	return m_UserPublishedFiles;
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

	return m_WorkshopFiles;
}

bool CSteam::EnumerateUserSubscribedFiles(uint32 startIndex) {
	if (!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->EnumerateUserSubscribedFiles(startIndex);
	m_CallbackEnumerateUserSubscribedFiles.Set(result, this, &CSteam::OnEnumerateUserSubscribedFiles);

	return true;
}

RemoteStorageEnumerateUserSubscribedFilesResult_t* CSteam::EnumerateUserSubscribedFilesResult() {
	if (!m_bInitialized) return nullptr;

	return m_SubscribedFiles;
}

bool CSteam::EnumerateUserSharedWorkshopFiles(uint64 steamId, uint32 startIndex,
	SteamParamStringArray_t *requiredTags, SteamParamStringArray_t *excludedTags) {

	if(!m_bInitialized) return false;

	CSteamID user(steamId);
	SteamAPICall_t result = SteamRemoteStorage()->EnumerateUserSharedWorkshopFiles(
		user, startIndex, requiredTags, excludedTags);
	m_CallbackEnumerateUserSharedWorkshopFiles.Set(result, this, &CSteam::OnEnumerateUserSharedWorkshopFiles);

	return true;
}

RemoteStorageEnumerateUserSharedWorkshopFilesResult_t* CSteam::EnumerateUserSharedWorkshopFilesResult() {
	if (!m_bInitialized) return nullptr;

	return m_UserSharedFiles;
}

bool CSteam::EnumeratePublishedFilesByUserAction(EWorkshopFileAction action, uint32 startIndex) {
	if(!m_bInitialized) return false;

	SteamAPICall_t result = SteamRemoteStorage()->EnumeratePublishedFilesByUserAction(
		action, startIndex);
	m_CallbackEnumeratePublishedFilesByUserAction.Set(result, this, &CSteam::OnEnumeratePublishedFilesByUserAction);

	return true;
}

RemoteStorageEnumeratePublishedFilesByUserActionResult_t* CSteam::EnumeratePublishedFilesByUserActionResult() {
	if(!m_bInitialized) return false;

	return m_PublishedFilesByAction;
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
	if (!m_bInitialized) return false;

	return m_PublishedItemVoteDetails;
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

void CSteam::OnFileShare(RemoteStorageFileShareResult_t *result, bool failure) {
	if (!failure) m_FileHandle = result->m_hFile;
	DispatchEvent(RESPONSE_OnFileShared, result->m_eResult);
}

void CSteam::OnUGCDownload(RemoteStorageDownloadUGCResult_t *result, bool failure) {
	// TODO: needs to be a map?
	if (!failure) m_DownloadResults[result->m_hFile] = result;
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
	if (!failure) m_PublishedFileDetails[result->m_nPublishedFileId] = result;
	DispatchEvent(RESPONSE_OnGetPublishedFileDetails, result->m_eResult);
}

void CSteam::OnEnumerateUserPublishedFiles(RemoteStorageEnumerateUserPublishedFilesResult_t *result, bool failure) {
	if (!failure) m_UserPublishedFiles = result;
	DispatchEvent(RESPONSE_OnEnumerateUserPublishedFiles, result->m_eResult);
}

void CSteam::OnEnumeratePublishedWorkshopFiles(RemoteStorageEnumerateWorkshopFilesResult_t *result, bool failure) {
	if (!failure) m_WorkshopFiles = result;
	DispatchEvent(RESPONSE_OnEnumeratePublishedWorkshopFiles, result->m_eResult);
}

void CSteam::OnEnumerateUserSubscribedFiles(RemoteStorageEnumerateUserSubscribedFilesResult_t *result, bool failure) {
	if (!failure) m_SubscribedFiles = result;
	DispatchEvent(RESPONSE_OnEnumerateUserSubscribedFiles, result->m_eResult);
}

void CSteam::OnEnumerateUserSharedWorkshopFiles(RemoteStorageEnumerateUserSharedWorkshopFilesResult_t *result, bool failure) {
	if (!failure) m_UserSharedFiles = result;
	DispatchEvent(RESPONSE_OnEnumerateUserSharedWorkshopFiles, result->m_eResult);
}

void CSteam::OnEnumeratePublishedFilesByUserAction(RemoteStorageEnumeratePublishedFilesByUserActionResult_t *result, bool failure) {
	if (!failure) m_PublishedFilesByAction = result;
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
	if (!failure) m_PublishedItemVoteDetails = result;
	DispatchEvent(RESPONSE_OnGetPublishedItemVoteDetails, result->m_eResult);
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
	DispatchEvent(RESPONSE_OnGameOverlayActivated, pCallback->m_bActive);
}
