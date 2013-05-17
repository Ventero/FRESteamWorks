/*
 *  FRESteamWorks.h
 *  This file is part of FRESteamWorks.
 *
 *  Created by David ´Oldes´ Oliva on 3/29/12.
 *  Contributors: Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012 Amanita Design. All rights reserved.
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

#if defined(WIN32)
	#define _CRT_SECURE_NO_WARNINGS
	#define EXPORT __declspec(dllexport)
#else
	// Symbols tagged with EXPORT are externally visible.
	// Must use the -fvisibility=hidden gcc option.
	#define EXPORT __attribute__((visibility("default")))
#endif

#include <string>
#include <vector>

#include <FlashRuntimeExtensions.h>
#include <steam_api.h>

#include "CSteam.h"

#define AIR_FUNC(name) \
	FREObject name(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])

#define ARG_CHECK(arg_num, default) \
	if (!g_Steam || argc != arg_num) return default

#define SET_PROP(object, property, value) \
	FRESetObjectProperty(object, (const uint8_t*)property, value, NULL)

// utility functions for conversion of FRE types
FREObject FREBool(bool);
FREObject FREInt(int32);
FREObject FREUint(uint32);
FREObject FREFloat(float);
FREObject FREString(std::string);
FREObject FREString(const char*);
FREObject FREUint64(uint64);
FREObject FREArray(uint32_t);
bool FREGetString(FREObject, std::string&);
bool FREGetBool(FREObject, uint32* val);
bool FREGetDouble(FREObject, double* val);
bool FREGetInt32(FREObject, int32* val);
bool FREGetUint32(FREObject, uint32* val);
bool FREGetUint64(FREObject, uint64* val);

bool extractParamStringArray(FREObject, SteamParamStringArray_t*);

class ANESteam : public CSteam {
	void DispatchEvent(char* code, char* level);
};

extern "C" {
	AIR_FUNC(AIRSteam_Init);
	AIR_FUNC(AIRSteam_RunCallbacks);
	AIR_FUNC(AIRSteam_GetUserID);
	AIR_FUNC(AIRSteam_GetAppID);
	AIR_FUNC(AIRSteam_GetPersonaName);
	AIR_FUNC(AIRSteam_UseCrashHandler);

	// stats / achievements
	AIR_FUNC(AIRSteam_RequestStats);
	AIR_FUNC(AIRSteam_SetAchievement);
	AIR_FUNC(AIRSteam_ClearAchievement);
	AIR_FUNC(AIRSteam_IsAchievement);
	AIR_FUNC(AIRSteam_GetStatInt);
	AIR_FUNC(AIRSteam_GetStatFloat);
	AIR_FUNC(AIRSteam_SetStatInt);
	AIR_FUNC(AIRSteam_SetStatFloat);
	AIR_FUNC(AIRSteam_StoreStats);
	AIR_FUNC(AIRSteam_ResetAllStats);

	// cloud
	AIR_FUNC(AIRSteam_GetFileCount);
	AIR_FUNC(AIRSteam_GetFileSize);
	AIR_FUNC(AIRSteam_FileExists);
	AIR_FUNC(AIRSteam_FileWrite);
	AIR_FUNC(AIRSteam_FileRead);
	AIR_FUNC(AIRSteam_FileDelete);
	AIR_FUNC(AIRSteam_FileShare);
	AIR_FUNC(AIRSteam_FileShareResult);
	AIR_FUNC(AIRSteam_IsCloudEnabledForApp);
	AIR_FUNC(AIRSteam_SetCloudEnabledForApp);
	AIR_FUNC(AIRSteam_GetQuota);

	// workshop
	AIR_FUNC(AIRSteam_UGCDownload);
	AIR_FUNC(AIRSteam_UGCRead);
	AIR_FUNC(AIRSteam_GetUGCDownloadProgress);
	AIR_FUNC(AIRSteam_GetUGCDownloadResult);
	AIR_FUNC(AIRSteam_PublishWorkshopFile);
	AIR_FUNC(AIRSteam_PublishWorkshopFileResult);
	AIR_FUNC(AIRSteam_DeletePublishedFile);
	AIR_FUNC(AIRSteam_GetPublishedFileDetails);
	AIR_FUNC(AIRSteam_GetPublishedFileDetailsResult);
	AIR_FUNC(AIRSteam_EnumerateUserPublishedFiles);
	AIR_FUNC(AIRSteam_EnumerateUserPublishedFilesResult);
	AIR_FUNC(AIRSteam_EnumeratePublishedWorkshopFiles);
	AIR_FUNC(AIRSteam_EnumeratePublishedWorkshopFilesResult);
	AIR_FUNC(AIRSteam_EnumerateUserSubscribedFiles);
	AIR_FUNC(AIRSteam_EnumerateUserSubscribedFilesResult);
	AIR_FUNC(AIRSteam_EnumerateUserSharedWorkshopFiles);
	AIR_FUNC(AIRSteam_EnumerateUserSharedWorkshopFilesResult);
	AIR_FUNC(AIRSteam_EnumeratePublishedFilesByUserAction);
	AIR_FUNC(AIRSteam_EnumeratePublishedFilesByUserActionResult);
	AIR_FUNC(AIRSteam_SubscribePublishedFile);
	AIR_FUNC(AIRSteam_UnsubscribePublishedFile);
	AIR_FUNC(AIRSteam_CreatePublishedFileUpdateRequest);
	AIR_FUNC(AIRSteam_UpdatePublishedFileFile);
	AIR_FUNC(AIRSteam_UpdatePublishedFilePreviewFile);
	AIR_FUNC(AIRSteam_UpdatePublishedFileTitle);
	AIR_FUNC(AIRSteam_UpdatePublishedFileDescription);
	AIR_FUNC(AIRSteam_UpdatePublishedFileSetChangeDescription);
	AIR_FUNC(AIRSteam_UpdatePublishedFileVisibility);
	AIR_FUNC(AIRSteam_UpdatePublishedFileTags);
	AIR_FUNC(AIRSteam_CommitPublishedFileUpdate);
	AIR_FUNC(AIRSteam_GetPublishedItemVoteDetails);
	AIR_FUNC(AIRSteam_GetPublishedItemVoteDetailsResult);
	AIR_FUNC(AIRSteam_UpdateUserPublishedItemVote);
	AIR_FUNC(AIRSteam_SetUserPublishedFileAction);

	// overlay
	AIR_FUNC(AIRSteam_ActivateGameOverlay);
	AIR_FUNC(AIRSteam_ActivateGameOverlayToUser);
	AIR_FUNC(AIRSteam_ActivateGameOverlayToWebPage);
	AIR_FUNC(AIRSteam_ActivateGameOverlayToStore);
	AIR_FUNC(AIRSteam_ActivateGameOverlayInviteDialog);
	AIR_FUNC(AIRSteam_IsOverlayEnabled);

	// DLC / subscription
	AIR_FUNC(AIRSteam_IsSubscribedApp);
	AIR_FUNC(AIRSteam_IsDLCInstalled);
	AIR_FUNC(AIRSteam_GetDLCCount);
	AIR_FUNC(AIRSteam_InstallDLC);
	AIR_FUNC(AIRSteam_UninstallDLC);
	AIR_FUNC(AIRSteam_DLCInstalledResult);

	// A native context instance is created
	void ContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx,
	                        uint32_t* numFunctions, const FRENamedFunction** functions);
	void ContextFinalizer(FREContext ctx);
	EXPORT void ExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet,
	                           FREContextFinalizer* ctxFinalizerToSet);
	EXPORT void ExtFinalizer(void* extData);
}
