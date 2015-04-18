/*
 *  APIFunctions.h
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012-2014 Ventero. All rights reserved.
 */

#ifndef APIFUNCTIONS_H
#define APIFUNCTIONS_H

#include <string>

#include <steam/steam_api.h>
#include <types/amfarray.hpp>
#include <types/amfobject.hpp>

bool AIRSteam_Init();
void AIRSteam_Shutdown();
std::nullptr_t AIRSteam_RunCallbacks();
uint64 AIRSteam_GetUserID();
uint32 AIRSteam_GetAppID();
std::string AIRSteam_GetAvailableGameLanguages();
std::string AIRSteam_GetCurrentGameLanguage();
std::string AIRSteam_GetPersonaName();
std::nullptr_t AIRSteam_UseCrashHandler();
bool AIRSteam_RestartAppIfNecessary();
bool AIRSteam_RequestStats();
bool AIRSteam_SetAchievement();
bool AIRSteam_ClearAchievement();
bool AIRSteam_IsAchievement();
int32 AIRSteam_GetStatInt();
float AIRSteam_GetStatFloat();
bool AIRSteam_SetStatInt();
bool AIRSteam_SetStatFloat();
bool AIRSteam_StoreStats();
bool AIRSteam_ResetAllStats();
bool AIRSteam_RequestGlobalStats();
double AIRSteam_GetGlobalStatInt();
double AIRSteam_GetGlobalStatFloat();
amf::AmfArray AIRSteam_GetGlobalStatHistoryInt();
amf::AmfArray AIRSteam_GetGlobalStatHistoryFloat();
bool AIRSteam_FindLeaderboard();
bool AIRSteam_FindOrCreateLeaderboard();
SteamLeaderboard_t AIRSteam_FindLeaderboardResult();
std::string AIRSteam_GetLeaderboardName();
int32 AIRSteam_GetLeaderboardEntryCount();
uint32 AIRSteam_GetLeaderboardSortMethod();
uint32 AIRSteam_GetLeaderboardDisplayType();
bool AIRSteam_UploadLeaderboardScore();
amf::AmfObject AIRSteam_UploadLeaderboardScoreResult();
bool AIRSteam_DownloadLeaderboardEntries();
amf::AmfArray AIRSteam_DownloadLeaderboardEntriesResult();
int32 AIRSteam_GetFileCount();
int32 AIRSteam_GetFileSize();
bool AIRSteam_FileExists();
bool AIRSteam_FileWrite();
bool AIRSteam_FileRead();
bool AIRSteam_FileDelete();
bool AIRSteam_FileShare();
UGCHandle_t AIRSteam_FileShareResult();
bool AIRSteam_IsCloudEnabledForApp();
bool AIRSteam_SetCloudEnabledForApp();
amf::AmfArray AIRSteam_GetQuota();
bool AIRSteam_UGCDownload();
bool AIRSteam_UGCRead();
amf::AmfArray AIRSteam_GetUGCDownloadProgress();
amf::AmfObject AIRSteam_GetUGCDownloadResult();
bool AIRSteam_PublishWorkshopFile();
PublishedFileId_t AIRSteam_PublishWorkshopFileResult();
bool AIRSteam_DeletePublishedFile();
bool AIRSteam_GetPublishedFileDetails();
amf::AmfObject AIRSteam_GetPublishedFileDetailsResult();
bool AIRSteam_EnumerateUserPublishedFiles();
amf::AmfObject AIRSteam_EnumerateUserPublishedFilesResult();
bool AIRSteam_EnumeratePublishedWorkshopFiles();
amf::AmfObject AIRSteam_EnumeratePublishedWorkshopFilesResult();
bool AIRSteam_EnumerateUserSubscribedFiles();
amf::AmfObject AIRSteam_EnumerateUserSubscribedFilesResult();
bool AIRSteam_EnumerateUserSharedWorkshopFiles();
amf::AmfObject AIRSteam_EnumerateUserSharedWorkshopFilesResult();
bool AIRSteam_EnumeratePublishedFilesByUserAction();
amf::AmfObject AIRSteam_EnumeratePublishedFilesByUserActionResult();
bool AIRSteam_SubscribePublishedFile();
bool AIRSteam_UnsubscribePublishedFile();
PublishedFileUpdateHandle_t AIRSteam_CreatePublishedFileUpdateRequest();
bool AIRSteam_UpdatePublishedFileFile();
bool AIRSteam_UpdatePublishedFilePreviewFile();
bool AIRSteam_UpdatePublishedFileTitle();
bool AIRSteam_UpdatePublishedFileDescription();
bool AIRSteam_UpdatePublishedFileSetChangeDescription();
bool AIRSteam_UpdatePublishedFileVisibility();
bool AIRSteam_UpdatePublishedFileTags();
bool AIRSteam_CommitPublishedFileUpdate();
bool AIRSteam_GetPublishedItemVoteDetails();
amf::AmfObject AIRSteam_GetPublishedItemVoteDetailsResult();
bool AIRSteam_GetUserPublishedItemVoteDetails();
amf::AmfObject AIRSteam_GetUserPublishedItemVoteDetailsResult();
bool AIRSteam_UpdateUserPublishedItemVote();
bool AIRSteam_SetUserPublishedFileAction();
int AIRSteam_GetFriendCount();
uint64 AIRSteam_GetFriendByIndex();
std::string AIRSteam_GetFriendPersonaName();
uint32 AIRSteam_GetAuthSessionTicket();
uint32 AIRSteam_GetAuthSessionTicketResult();
int AIRSteam_BeginAuthSession();
bool AIRSteam_EndAuthSession();
bool AIRSteam_CancelAuthTicket();
int AIRSteam_UserHasLicenseForApp();
bool AIRSteam_ActivateGameOverlay();
bool AIRSteam_ActivateGameOverlayToUser();
bool AIRSteam_ActivateGameOverlayToWebPage();
bool AIRSteam_ActivateGameOverlayToStore();
bool AIRSteam_ActivateGameOverlayInviteDialog();
bool AIRSteam_IsOverlayEnabled();
bool AIRSteam_SetOverlayNotificationPosition();
bool AIRSteam_IsSubscribedApp();
bool AIRSteam_IsDLCInstalled();
int32 AIRSteam_GetDLCCount();
bool AIRSteam_InstallDLC();
bool AIRSteam_UninstallDLC();
bool AIRSteam_DLCInstalledResult();
std::nullptr_t AIRSteam_MicroTxnResult();
std::string AIRSteam_GetEnv();
bool AIRSteam_SetEnv();

#endif
