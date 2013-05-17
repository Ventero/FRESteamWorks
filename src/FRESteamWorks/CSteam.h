/*
 *  CSteam.h
 *  This file is part of FRESteamWorks.
 *
 *  Created by David ´Oldes´ Oliva on 3/29/12.
 *  Contributors: Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012 Amanita Design. All rights reserved.
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

#if defined(WIN32)
	#define _CRT_SECURE_NO_WARNINGS
	#define snprintf _snprintf
#endif

#include <map>
#include <stdio.h>
#include <string>
#include <sstream>

#include <steam_api.h>

// General result codes
enum ResponseTypes {
	RESPONSE_OnUserStatsReceived,
	RESPONSE_OnUserStatsStored,
	RESPONSE_OnAchievementStored,
	RESPONSE_OnGameOverlayActivated,
	RESPONSE_OnFileShared,
	RESPONSE_OnUGCDownload,
	RESPONSE_OnPublishWorkshopFile,
	RESPONSE_OnDeletePublishedFile,
	RESPONSE_OnGetPublishedFileDetails,
	RESPONSE_OnEnumerateUserPublishedFiles,
	RESPONSE_OnEnumeratePublishedWorkshopFiles,
	RESPONSE_OnEnumerateUserSubscribedFiles,
	RESPONSE_OnEnumerateUserSharedWorkshopFiles,
	RESPONSE_OnEnumeratePublishedFilesByUserAction,
	RESPONSE_OnCommitPublishedFileUpdate,
	RESPONSE_OnSubscribePublishedFile,
	RESPONSE_OnUnsubscribePublishedFile,
	RESPONSE_OnGetPublishedItemVoteDetails,
	RESPONSE_OnUpdateUserPublishedItemVote,
	RESPONSE_OnSetUserPublishedFileAction
};

class CSteam {
public:
	CSteam();
	virtual ~CSteam();

	// general stuff
	std::string GetUserID();
	std::string GetPersonaName();

	// stats/achievements
	bool RequestStats();
	bool SetAchievement(std::string name);
	bool ClearAchievement(std::string name);
	bool IsAchievement(std::string name);
	bool GetStat(std::string name, int32 *value);
	bool GetStat(std::string name, float *value);
	bool SetStat(std::string name, int32 value);
	bool SetStat(std::string name, float value);
	bool StoreStats();
	bool ResetAllStats(bool bAchievementsToo);

	// remote storage
	int32 GetFileCount();
	int32 GetFileSize(std::string name);
	bool FileExists(std::string name);
	bool FileWrite(std::string name, const void* data, int32 length);
	int32 FileRead(std::string name, char** data);
	bool FileDelete(std::string name);
	bool FileShare(std::string name);
	UGCHandle_t FileShareResult();
	bool IsCloudEnabledForApp();
	bool SetCloudEnabledForApp(bool enabled);
	bool GetQuota(int32 *total, int32 *available);

	// workshop/ugc
	bool UGCDownload(UGCHandle_t handle, uint32 priority);
	int32 UGCRead(UGCHandle_t handle, int32 size, uint32 offset, char **content);
	bool GetUGCDownloadProgress(UGCHandle_t handle, int32 *downloaded, int32 *expected);
	RemoteStorageDownloadUGCResult_t* GetUGCDownloadResult(UGCHandle_t handle);

	bool PublishWorkshopFile(std::string name, std::string preview, AppId_t nConsumerAppId,
		std::string title, std::string description, ERemoteStoragePublishedFileVisibility visibility,
		SteamParamStringArray_t *tags, EWorkshopFileType fileType);
	PublishedFileId_t PublishWorkshopFileResult();
	bool DeletePublishedFile(PublishedFileId_t file);

	bool GetPublishedFileDetails(PublishedFileId_t file);
	RemoteStorageGetPublishedFileDetailsResult_t* GetPublishedFileDetailsResult(PublishedFileId_t file);

	bool EnumerateUserPublishedFiles(uint32 startIndex);
	RemoteStorageEnumerateUserPublishedFilesResult_t* EnumerateUserPublishedFilesResult();
	bool EnumeratePublishedWorkshopFiles(EWorkshopEnumerationType type, uint32 start, uint32 count,
		uint32 days, SteamParamStringArray_t *tags, SteamParamStringArray_t *userTags);
	RemoteStorageEnumerateWorkshopFilesResult_t* EnumeratePublishedWorkshopFilesResult();

	bool EnumerateUserSubscribedFiles(uint32 startIndex);
	RemoteStorageEnumerateUserSubscribedFilesResult_t* EnumerateUserSubscribedFilesResult();

	bool EnumerateUserSharedWorkshopFiles(uint64 steamId, uint32 startIndex,
		SteamParamStringArray_t *requiredTags, SteamParamStringArray_t *excludedTags);
	RemoteStorageEnumerateUserSharedWorkshopFilesResult_t* EnumerateUserSharedWorkshopFilesResult();

	bool EnumeratePublishedFilesByUserAction(EWorkshopFileAction action, uint32 startIndex);
	RemoteStorageEnumeratePublishedFilesByUserActionResult_t* EnumeratePublishedFilesByUserActionResult();

	bool SubscribePublishedFile(PublishedFileId_t file);
	bool UnsubscribePublishedFile(PublishedFileId_t file);

	PublishedFileUpdateHandle_t CreatePublishedFileUpdateRequest(PublishedFileId_t file);
	bool UpdatePublishedFileFile(PublishedFileUpdateHandle_t handle, std::string file);
	bool UpdatePublishedFilePreviewFile(PublishedFileUpdateHandle_t handle, std::string preview);
	bool UpdatePublishedFileTitle(PublishedFileUpdateHandle_t handle, std::string title);
	bool UpdatePublishedFileDescription(PublishedFileUpdateHandle_t handle, std::string description);
	bool UpdatePublishedFileSetChangeDescription(PublishedFileUpdateHandle_t handle, std::string changeDesc);
	bool UpdatePublishedFileVisibility(PublishedFileUpdateHandle_t handle, ERemoteStoragePublishedFileVisibility visibility);
	bool UpdatePublishedFileTags(PublishedFileUpdateHandle_t handle, SteamParamStringArray_t *tags);
	bool CommitPublishedFileUpdate(PublishedFileUpdateHandle_t handle);

	bool GetPublishedItemVoteDetails(PublishedFileId_t file);
	RemoteStorageGetPublishedItemVoteDetailsResult_t* GetPublishedItemVoteDetailsResult();
	bool UpdateUserPublishedItemVote(PublishedFileId_t file, bool upvote);

	bool SetUserPublishedFileAction(PublishedFileId_t file, EWorkshopFileAction action);

	// overlay
	bool ActivateGameOverlay(std::string dialog);
	bool ActivateGameOverlayToUser(std::string dialog, CSteamID steamId);
	bool ActivateGameOverlayToWebPage(std::string url);
	bool ActivateGameOverlayToStore(AppId_t appId, EOverlayToStoreFlag flag);
	bool ActivateGameOverlayInviteDialog(CSteamID lobbyId);

protected:
	virtual void DispatchEvent(char* code, char* level) = 0;

private:
	// Our current AppID
	uint32 m_iAppID;
	bool m_bInitialized;

	// the most recent received UGCResult
	UGCHandle_t m_FileHandle;
	std::map<UGCHandle_t, RemoteStorageDownloadUGCResult_t*> m_DownloadResults;
	PublishedFileId_t m_PublishedFileId;
	std::map<PublishedFileId_t, RemoteStorageGetPublishedFileDetailsResult_t*> m_PublishedFileDetails;
	RemoteStorageEnumerateUserPublishedFilesResult_t *m_UserPublishedFiles;
	RemoteStorageEnumerateWorkshopFilesResult_t *m_WorkshopFiles;
	RemoteStorageEnumerateUserSubscribedFilesResult_t *m_SubscribedFiles;
	RemoteStorageEnumerateUserSharedWorkshopFilesResult_t *m_UserSharedFiles;
	RemoteStorageEnumeratePublishedFilesByUserActionResult_t *m_PublishedFilesByAction;
	RemoteStorageGetPublishedItemVoteDetailsResult_t *m_PublishedItemVoteDetails;

	void DispatchEvent(const int req_type, const int response);

	/*
	 * callbacks / call results
	 */

#define STEAM_CALLRESULT(thisclass, func, param, var) \
		CCallResult<thisclass, param> var; \
		void func(param*, bool)

	// stats/achievements
	STEAM_CALLBACK(CSteam, OnUserStatsReceived, UserStatsReceived_t,
	               m_CallbackUserStatsReceived);
	STEAM_CALLBACK(CSteam, OnUserStatsStored, UserStatsStored_t,
	               m_CallbackUserStatsStored);
	STEAM_CALLBACK(CSteam, OnAchievementStored,
	               UserAchievementStored_t, m_CallbackAchievementStored);

	// workshop
	STEAM_CALLRESULT(CSteam, OnFileShare,
	                 RemoteStorageFileShareResult_t,
	                 m_CallbackFileShare);
	STEAM_CALLRESULT(CSteam, OnUGCDownload,
	                 RemoteStorageDownloadUGCResult_t,
	                 m_CallbackUGCDownload);
	STEAM_CALLRESULT(CSteam, OnPublishWorkshopFile,
	                 RemoteStoragePublishFileResult_t,
	                 m_CallbackPublishWorkshopFile);
	STEAM_CALLRESULT(CSteam, OnDeletePublishedFile,
	                 RemoteStorageDeletePublishedFileResult_t,
	                 m_CallbackDeletePublishedFile);
	STEAM_CALLRESULT(CSteam, OnGetPublishedFileDetails,
	                 RemoteStorageGetPublishedFileDetailsResult_t,
	                 m_CallbackGetPublishedFileDetails);
	STEAM_CALLRESULT(CSteam, OnEnumerateUserPublishedFiles,
	                 RemoteStorageEnumerateUserPublishedFilesResult_t,
	                 m_CallbackEnumerateUserPublishedFiles);
	STEAM_CALLRESULT(CSteam, OnEnumeratePublishedWorkshopFiles,
	                 RemoteStorageEnumerateWorkshopFilesResult_t,
	                 m_CallbackEnumeratePublishedWorkshopFiles);
	STEAM_CALLRESULT(CSteam, OnEnumerateUserSubscribedFiles,
	                 RemoteStorageEnumerateUserSubscribedFilesResult_t,
	                 m_CallbackEnumerateUserSubscribedFiles);
	STEAM_CALLRESULT(CSteam, OnEnumerateUserSharedWorkshopFiles,
	                 RemoteStorageEnumerateUserSharedWorkshopFilesResult_t,
	                 m_CallbackEnumerateUserSharedWorkshopFiles);
	STEAM_CALLRESULT(CSteam, OnEnumeratePublishedFilesByUserAction,
	                 RemoteStorageEnumeratePublishedFilesByUserActionResult_t,
	                 m_CallbackEnumeratePublishedFilesByUserAction);
	STEAM_CALLRESULT(CSteam, OnCommitPublishedFileUpdate,
	                 RemoteStorageUpdatePublishedFileResult_t,
	                 m_CallbackCommitPublishedFileUpdate);
	STEAM_CALLRESULT(CSteam, OnSubscribePublishedFile,
	                 RemoteStorageSubscribePublishedFileResult_t,
	                 m_CallbackSubscribePublishedFile);
	STEAM_CALLRESULT(CSteam, OnUnsubscribePublishedFile,
	                 RemoteStorageUnsubscribePublishedFileResult_t,
	                 m_CallbackUnsubscribePublishedFile);
	STEAM_CALLRESULT(CSteam, OnGetPublishedItemVoteDetails,
	                 RemoteStorageGetPublishedItemVoteDetailsResult_t,
	                 m_CallbackGetPublishedItemVoteDetails);
	STEAM_CALLRESULT(CSteam, OnUpdateUserPublishedItemVote,
	                 RemoteStorageUpdateUserPublishedItemVoteResult_t,
	                 m_CallbackUpdateUserPublishedItemVote);
	STEAM_CALLRESULT(CSteam, OnSetUserPublishedFileAction,
	                 RemoteStorageSetUserPublishedFileActionResult_t,
	                 m_CallbackSetUserPublishedFileAction);

	// overlay
	STEAM_CALLBACK(CSteam, OnGameOverlayActivated, GameOverlayActivated_t,
	               m_CallbackGameOverlayActivated);

};
