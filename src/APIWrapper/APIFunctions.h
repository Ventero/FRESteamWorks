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

struct Void;

bool AIRSteam_Init(const amf::AmfArray &);
void AIRSteam_Shutdown(const amf::AmfArray &);
Void AIRSteam_RunCallbacks(const amf::AmfArray &);
uint64 AIRSteam_GetUserID(const amf::AmfArray &);
uint32 AIRSteam_GetAppID(const amf::AmfArray &);
std::string AIRSteam_GetAvailableGameLanguages(const amf::AmfArray &);
std::string AIRSteam_GetCurrentGameLanguage(const amf::AmfArray &);
std::string AIRSteam_GetPersonaName(const amf::AmfArray &);
bool AIRSteam_RestartAppIfNecessary(const amf::AmfArray &);
bool AIRSteam_RequestStats(const amf::AmfArray &);
bool AIRSteam_SetAchievement(const amf::AmfArray &);
bool AIRSteam_ClearAchievement(const amf::AmfArray &);
bool AIRSteam_IsAchievement(const amf::AmfArray &);
bool AIRSteam_IndicateAchievementProgress(const amf::AmfArray &);
int32 AIRSteam_GetStatInt(const amf::AmfArray &);
float AIRSteam_GetStatFloat(const amf::AmfArray &);
bool AIRSteam_SetStatInt(const amf::AmfArray &);
bool AIRSteam_SetStatFloat(const amf::AmfArray &);
bool AIRSteam_StoreStats(const amf::AmfArray &);
bool AIRSteam_ResetAllStats(const amf::AmfArray &);
bool AIRSteam_RequestGlobalStats(const amf::AmfArray &);
double AIRSteam_GetGlobalStatInt(const amf::AmfArray &);
double AIRSteam_GetGlobalStatFloat(const amf::AmfArray &);
amf::AmfArray AIRSteam_GetGlobalStatHistoryInt(const amf::AmfArray &);
amf::AmfArray AIRSteam_GetGlobalStatHistoryFloat(const amf::AmfArray &);
bool AIRSteam_FindLeaderboard(const amf::AmfArray &);
bool AIRSteam_FindOrCreateLeaderboard(const amf::AmfArray &);
SteamLeaderboard_t AIRSteam_FindLeaderboardResult(const amf::AmfArray &);
std::string AIRSteam_GetLeaderboardName(const amf::AmfArray &);
int32 AIRSteam_GetLeaderboardEntryCount(const amf::AmfArray &);
uint32 AIRSteam_GetLeaderboardSortMethod(const amf::AmfArray &);
uint32 AIRSteam_GetLeaderboardDisplayType(const amf::AmfArray &);
bool AIRSteam_UploadLeaderboardScore(const amf::AmfArray &);
amf::AmfObject AIRSteam_UploadLeaderboardScoreResult(const amf::AmfArray &);
bool AIRSteam_DownloadLeaderboardEntries(const amf::AmfArray &);
amf::AmfArray AIRSteam_DownloadLeaderboardEntriesResult(const amf::AmfArray &);
int32 AIRSteam_GetFileCount(const amf::AmfArray &);
int32 AIRSteam_GetFileSize(const amf::AmfArray &);
bool AIRSteam_FileExists(const amf::AmfArray &);
bool AIRSteam_FileWrite(const amf::AmfArray &);
bool AIRSteam_FileRead(const amf::AmfArray &);
bool AIRSteam_FileDelete(const amf::AmfArray &);
bool AIRSteam_FileShare(const amf::AmfArray &);
UGCHandle_t AIRSteam_FileShareResult(const amf::AmfArray &);
bool AIRSteam_IsCloudEnabledForApp(const amf::AmfArray &);
bool AIRSteam_SetCloudEnabledForApp(const amf::AmfArray &);
amf::AmfArray AIRSteam_GetQuota(const amf::AmfArray &);
bool AIRSteam_UGCDownload(const amf::AmfArray &);
bool AIRSteam_UGCRead(const amf::AmfArray &);
amf::AmfArray AIRSteam_GetUGCDownloadProgress(const amf::AmfArray &);
amf::AmfObject AIRSteam_GetUGCDownloadResult(const amf::AmfArray &);
bool AIRSteam_PublishWorkshopFile(const amf::AmfArray &);
PublishedFileId_t AIRSteam_PublishWorkshopFileResult(const amf::AmfArray &);
bool AIRSteam_DeletePublishedFile(const amf::AmfArray &);
bool AIRSteam_GetPublishedFileDetails(const amf::AmfArray &);
amf::AmfObject AIRSteam_GetPublishedFileDetailsResult(const amf::AmfArray &);
bool AIRSteam_EnumerateUserPublishedFiles(const amf::AmfArray &);
amf::AmfObject AIRSteam_EnumerateUserPublishedFilesResult(const amf::AmfArray &);
bool AIRSteam_EnumeratePublishedWorkshopFiles(const amf::AmfArray &);
amf::AmfObject AIRSteam_EnumeratePublishedWorkshopFilesResult(const amf::AmfArray &);
bool AIRSteam_EnumerateUserSubscribedFiles(const amf::AmfArray &);
amf::AmfObject AIRSteam_EnumerateUserSubscribedFilesResult(const amf::AmfArray &);
bool AIRSteam_EnumerateUserSharedWorkshopFiles(const amf::AmfArray &);
amf::AmfObject AIRSteam_EnumerateUserSharedWorkshopFilesResult(const amf::AmfArray &);
bool AIRSteam_EnumeratePublishedFilesByUserAction(const amf::AmfArray &);
amf::AmfObject AIRSteam_EnumeratePublishedFilesByUserActionResult(const amf::AmfArray &);
bool AIRSteam_SubscribePublishedFile(const amf::AmfArray &);
bool AIRSteam_UnsubscribePublishedFile(const amf::AmfArray &);
PublishedFileUpdateHandle_t AIRSteam_CreatePublishedFileUpdateRequest(const amf::AmfArray &);
bool AIRSteam_UpdatePublishedFileFile(const amf::AmfArray &);
bool AIRSteam_UpdatePublishedFilePreviewFile(const amf::AmfArray &);
bool AIRSteam_UpdatePublishedFileTitle(const amf::AmfArray &);
bool AIRSteam_UpdatePublishedFileDescription(const amf::AmfArray &);
bool AIRSteam_UpdatePublishedFileSetChangeDescription(const amf::AmfArray &);
bool AIRSteam_UpdatePublishedFileVisibility(const amf::AmfArray &);
bool AIRSteam_UpdatePublishedFileTags(const amf::AmfArray &);
bool AIRSteam_CommitPublishedFileUpdate(const amf::AmfArray &);
bool AIRSteam_GetPublishedItemVoteDetails(const amf::AmfArray &);
amf::AmfObject AIRSteam_GetPublishedItemVoteDetailsResult(const amf::AmfArray &);
bool AIRSteam_GetUserPublishedItemVoteDetails(const amf::AmfArray &);
amf::AmfObject AIRSteam_GetUserPublishedItemVoteDetailsResult(const amf::AmfArray &);
bool AIRSteam_UpdateUserPublishedItemVote(const amf::AmfArray &);
bool AIRSteam_SetUserPublishedFileAction(const amf::AmfArray &);
int AIRSteam_GetFriendCount(const amf::AmfArray &);
uint64 AIRSteam_GetFriendByIndex(const amf::AmfArray &);
std::string AIRSteam_GetFriendPersonaName(const amf::AmfArray &);
uint32 AIRSteam_GetAuthSessionTicket(const amf::AmfArray &);
uint32 AIRSteam_GetAuthSessionTicketResult(const amf::AmfArray &);
int AIRSteam_BeginAuthSession(const amf::AmfArray &);
bool AIRSteam_EndAuthSession(const amf::AmfArray &);
bool AIRSteam_CancelAuthTicket(const amf::AmfArray &);
int AIRSteam_UserHasLicenseForApp(const amf::AmfArray &);
bool AIRSteam_ActivateGameOverlay(const amf::AmfArray &);
bool AIRSteam_ActivateGameOverlayToUser(const amf::AmfArray &);
bool AIRSteam_ActivateGameOverlayToWebPage(const amf::AmfArray &);
bool AIRSteam_ActivateGameOverlayToStore(const amf::AmfArray &);
bool AIRSteam_ActivateGameOverlayInviteDialog(const amf::AmfArray &);
bool AIRSteam_IsOverlayEnabled(const amf::AmfArray &);
bool AIRSteam_SetOverlayNotificationPosition(const amf::AmfArray &);
bool AIRSteam_IsSubscribedApp(const amf::AmfArray &);
bool AIRSteam_IsDLCInstalled(const amf::AmfArray &);
int32 AIRSteam_GetDLCCount(const amf::AmfArray &);
bool AIRSteam_InstallDLC(const amf::AmfArray &);
bool AIRSteam_UninstallDLC(const amf::AmfArray &);
bool AIRSteam_DLCInstalledResult(const amf::AmfArray &);
Void AIRSteam_MicroTxnResult(const amf::AmfArray &);
std::string AIRSteam_GetEnv(const amf::AmfArray &);
bool AIRSteam_SetEnv(const amf::AmfArray &);

#endif
