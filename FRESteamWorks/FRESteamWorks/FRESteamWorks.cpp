//
//  FRESteamWorks.cpp
//  FRESteamWorks
//
//  Created by David `Oldes` Oliva on 3/29/12.
//  Copyright (c) 2012 Amanita Design. All rights reserved.
//

#include "FRESteamWorks.h"

// Used to dispatch event back to AIR
FREContext AIRContext;

// Global access to Steam object
CSteam*	g_Steam = NULL;

CSteam::CSteam():
    m_CallbackUserStatsReceived(this, &CSteam::OnUserStatsReceived),
    m_CallbackUserStatsStored(this, &CSteam::OnUserStatsStored),
    m_CallbackAchievementStored(this, &CSteam::OnAchievementStored),
    m_CallbackGameOverlayActivated(this, &CSteam::OnGameOverlayActivated)
{
	m_iAppID = SteamUtils()->GetAppID();
	m_bInitialized = (SteamUserStats() != NULL && SteamUser() != NULL);
	RequestStats();
}

bool CSteam::RequestStats() {
	// Is Steam loaded? If not we can't get stats.
	if (!m_bInitialized) return false;

	// Is the user logged on?  If not we can't get stats.
	if (!SteamUser()->BLoggedOn()) return false;

	// Request user stats.
	return SteamUserStats()->RequestCurrentStats();
}

bool CSteam::SetAchievement(const char* ID) {
	if (!m_bInitialized) return false;

	SteamUserStats()->SetAchievement(ID);
	return SteamUserStats()->StoreStats();
}

bool CSteam::ClearAchievement(const char* ID) {
	if (!m_bInitialized) return false;

	SteamUserStats()->ClearAchievement(ID);
	return SteamUserStats()->StoreStats();
}

bool CSteam::IsAchievement(const char* ID) {
	bool result = false;
	if (m_bInitialized) SteamUserStats()->GetAchievement(ID, &result);
	return result;
}

bool CSteam::GetStat(const char* ID, int32 *value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->GetStat(ID, value);
}

bool CSteam::GetStat(const char* ID, float *value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->GetStat(ID, value);
}

bool CSteam::SetStat(const char* ID, int32 value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->SetStat(ID, value);
}

bool CSteam::SetStat(const char* ID, float value) {
	if (!m_bInitialized) return false;

	return SteamUserStats()->SetStat(ID, value);
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

void CSteam::DispatchEvent(const int req_type, const int response) {
	char code[5];
	char level[5];

	snprintf(code, sizeof(code), "%d", req_type);
	snprintf(level, sizeof(level), "%d", response);
	FREResult res = FREDispatchStatusEventAsync(AIRContext, (const uint8_t*)code, (const uint8_t*)level);
	if (res != FRE_OK) {
		return;
	}
}

void CSteam::OnUserStatsReceived(UserStatsReceived_t *pCallback) {
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID != pCallback->m_nGameID) return;

	g_Steam->DispatchEvent(RESPONSE_OnUserStatsReceived, pCallback->m_eResult);
}


void CSteam::OnUserStatsStored(UserStatsStored_t *pCallback) {
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_iAppID != pCallback->m_nGameID) return;

	DispatchEvent(RESPONSE_OnUserStatsStored, pCallback->m_eResult);
}


void CSteam::OnAchievementStored(UserAchievementStored_t *pCallback) {
  // we may get callbacks for other games' stats arriving, ignore them
  if (m_iAppID != pCallback->m_nGameID) return;

  g_Steam->DispatchEvent(RESPONSE_OnAchievementStored, RESPONSE_OK);
}

void CSteam::OnGameOverlayActivated(GameOverlayActivated_t *pCallback) {
	g_Steam->DispatchEvent(RESPONSE_OnGameOverlayActivated, pCallback->m_bActive);
}

extern "C" {

	FREObject AIRSteam_Init(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		bool bRet;

		if (g_Steam == NULL) {
			bRet = SteamAPI_Init();
			if (bRet)
				g_Steam = new CSteam();
		} else {
			bRet = true;
		}
		FRENewObjectFromBool((uint32_t)bRet, &result);
		return result;
	}

	FREObject AIRSteam_RequestStats(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		if (g_Steam) {
			FRENewObjectFromBool((uint32_t)g_Steam->RequestStats(), &result);
		} else {
			FRENewObjectFromBool(false, &result);
		}
		SteamAPI_RunCallbacks();
		return result;
	}

	FREObject AIRSteam_SetAchievement(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result = NULL;
		if (g_Steam && argc == 1) {
			uint32_t len = -1;
			const uint8_t *ID = 0;
			FREResult res = FREGetObjectAsUTF8(argv[0], &len, &ID);
			if (res == FRE_OK) {
				FRENewObjectFromBool((uint32_t)g_Steam->SetAchievement((const char *)ID), &result);
			}
		}
		if (result == NULL) FRENewObjectFromBool(false, &result);
		SteamAPI_RunCallbacks();
		return result;
	}

	FREObject AIRSteam_ClearAchievement(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result = NULL;
		if (g_Steam) {
			uint32_t len = -1;
			const uint8_t *ID = 0;
			FREResult res = FREGetObjectAsUTF8(argv[0], &len, &ID);
			if (res == FRE_OK) {
				FRENewObjectFromBool((uint32_t)g_Steam->ClearAchievement((const char *)ID), &result);
			}
		}
		if (result == NULL) FRENewObjectFromBool(false, &result);
		return result;
	}

	FREObject AIRSteam_IsAchievement(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result = NULL;
		if (g_Steam && argc == 1) {
			uint32_t len = -1;
			const uint8_t *ID = 0;
			FREResult res = FREGetObjectAsUTF8(argv[0], &len, &ID);
			if (res == FRE_OK) {
				FRENewObjectFromBool((uint32_t)g_Steam->IsAchievement((const char *)ID), &result);
			}
		}
		if (result == NULL) FRENewObjectFromBool(false, &result);
		return result;
	}

	FREObject AIRSteam_RunCallbacks(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		SteamAPI_RunCallbacks();
		return NULL;
	}

	FREObject AIRSteam_GetStatInt(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		int32 value = 0;
		if (g_Steam) {
			uint32_t len = -1;
			const uint8_t *ID = 0;
			FREResult res = FREGetObjectAsUTF8(argv[0], &len, &ID);
			if (res == FRE_OK) {
				g_Steam->GetStat((const char *)ID, &value);
			}
		}
		FRENewObjectFromInt32(value, &result);
		return result;
	}

	FREObject AIRSteam_GetStatFloat(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		float value = 0.0f;
		if (g_Steam) {
			uint32_t len = -1;
			const uint8_t *ID = 0;
			FREResult res = FREGetObjectAsUTF8(argv[0], &len, &ID);
			if (res == FRE_OK) {
				g_Steam->GetStat((const char *)ID, &value);
			}
		}
		FRENewObjectFromDouble(value, &result);
		return result;
	}

	FREObject AIRSteam_SetStatInt(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result = NULL;
		if (g_Steam && argc == 2) {
			uint32_t len = -1;
			const uint8_t *ID = 0;
			int32 value;
			if (FREGetObjectAsUTF8(argv[0], &len, &ID) == FRE_OK &&
			    FREGetObjectAsInt32(argv[1], &value) == FRE_OK) {
				FRENewObjectFromBool((uint32_t)g_Steam->SetStat((const char *)ID, value), &result);
			}
		}
		if (result == NULL) FRENewObjectFromBool(false, &result);
		return result;
	}
	FREObject AIRSteam_SetStatFloat(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result = NULL;
		if (g_Steam && argc == 2) {
			uint32_t len = -1;
			const uint8_t *ID = 0;
			double value;
			if (FREGetObjectAsUTF8(argv[0], &len, &ID) == FRE_OK &&
			    FREGetObjectAsDouble(argv[1], &value) == FRE_OK) {
				FRENewObjectFromBool((uint32_t)g_Steam->SetStat((const char *)ID, (float)value), &result);
			}
		}
		if (result == NULL) FRENewObjectFromBool(false, &result);
		return result;
	}

	FREObject AIRSteam_StoreStats(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		if (g_Steam) {
			FRENewObjectFromBool((uint32_t)g_Steam->StoreStats(), &result);
		} else {
			FRENewObjectFromBool(false, &result);
		}
		return result;
	}

	FREObject AIRSteam_ResetAllStats(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result = NULL;
		if (g_Steam && argc == 1) {
			uint32_t bAchievementsToo;
			if (FREGetObjectAsBool(argv[0], &bAchievementsToo) == FRE_OK) {
				FRENewObjectFromBool((uint32_t)g_Steam->ResetAllStats((bAchievementsToo!=0)), &result);
			}
		}
		if (result == NULL) FRENewObjectFromBool(false, &result);
		return result;
	}

	//Steam Cloud
	FREObject AIRSteam_GetFileCount(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		int32 count = 0;
		if (g_Steam) {
			count = SteamRemoteStorage()->GetFileCount();
		}
		FRENewObjectFromInt32((uint32_t)count, &result);
		return result;
	}

	FREObject AIRSteam_GetFileSize(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		int32 fileSize = 0;
		if (g_Steam && argc==1) {
			uint32_t len = -1;
			const uint8_t *fileName = 0;
			if (FREGetObjectAsUTF8(argv[0], &len, &fileName) == FRE_OK) {
				fileSize = SteamRemoteStorage()->GetFileSize((char *)fileName);
			}
		}
		FRENewObjectFromInt32((uint32_t)fileSize, &result);
		return result;
	}

	FREObject AIRSteam_FileExists(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		bool fileExists = 0;
		if (g_Steam && argc==1) {
			uint32_t len = -1;
			const uint8_t *fileName = 0;
			if (FREGetObjectAsUTF8(argv[0], &len, &fileName) == FRE_OK) {
				fileExists = SteamRemoteStorage()->FileExists((char *)fileName);
			}
		}
		FRENewObjectFromBool((uint32_t)fileExists, &result);
		return result;
	}

	FREObject AIRSteam_FileWrite(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		FREByteArray byteArray;
		bool retVal=0;

		if (g_Steam && argc==2) {
			uint32_t len = -1;
			const uint8_t *fileName = 0;
			if (FREGetObjectAsUTF8(argv[0], &len, &fileName) == FRE_OK &&
				  FREAcquireByteArray(argv[1], &byteArray) == FRE_OK) {
				retVal = SteamRemoteStorage()->FileWrite((char *)fileName, byteArray.bytes, byteArray.length);
				FREReleaseByteArray(argv[1]);
			}
		}
		FRENewObjectFromBool((uint32_t)retVal, &result);
		return result;
	}

	FREObject AIRSteam_FileRead(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		FREByteArray byteArray;
		int32 retVal=0;

		if (g_Steam && argc==2) {

			uint32_t len = -1;
			const uint8_t *fileName = 0;
			uint32 fileSize;
			char *dataIn;
			if (FREGetObjectAsUTF8(argv[0], &len, &fileName) == FRE_OK &&
				  FREAcquireByteArray(argv[1], &byteArray) == FRE_OK) {
				fileSize = SteamRemoteStorage()->GetFileSize((char *)fileName);
				if (fileSize > 0 && fileSize <= byteArray.length) {
					dataIn = (char *)malloc(fileSize);
					retVal = SteamRemoteStorage()->FileRead((char *)fileName, dataIn, fileSize);
					memcpy(byteArray.bytes, dataIn, fileSize);
					free(dataIn);
				}
				FREReleaseByteArray(argv[1]);
			}
		}
		FRENewObjectFromBool((uint32_t)retVal, &result);
		return result;
	}

	FREObject AIRSteam_FileDelete(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		bool retVal = 0;
		if (g_Steam && argc==1) {
			uint32_t len = -1;
			const uint8_t *fileName = 0;
			if (FREGetObjectAsUTF8(argv[0], &len, &fileName) == FRE_OK) {
				retVal = SteamRemoteStorage()->FileDelete((char *)fileName);
			}
		}
		FRENewObjectFromBool((uint32_t)retVal, &result);
		return result;
	}

	FREObject AIRSteam_IsCloudEnabledForApp(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		bool retVal = 0;
		if (g_Steam) {
			retVal = SteamRemoteStorage()->IsCloudEnabledForApp();
		}
		FRENewObjectFromBool((uint32_t)retVal, &result);
		return result;
	}

	FREObject AIRSteam_SetCloudEnabledForApp(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		bool retVal = 0;
		if (g_Steam && argc==1) {
			uint32_t enabled = 0;
			bool bEnabled;
			if (FREGetObjectAsBool(argv[0], &enabled) == FRE_OK) {
				bEnabled = (enabled != 0);
				SteamRemoteStorage()->SetCloudEnabledForApp(bEnabled);
				retVal = (bEnabled == SteamRemoteStorage()->IsCloudEnabledForApp());
			}
		}
		FRENewObjectFromBool((uint32_t)retVal, &result);
		return result;
	}
	//============================

	// A native context instance is created
	void ContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx,
	                        uint32_t* numFunctions, const FRENamedFunction** functions) {
		AIRContext = ctx;

#define FRE_FUNC(fname) \
		{ (const uint8_t*) #fname, NULL, &fname }

		static FRENamedFunction func[] = {
			FRE_FUNC(AIRSteam_Init),
			FRE_FUNC(AIRSteam_RunCallbacks),

			// stats / achievements
			FRE_FUNC(AIRSteam_RequestStats),
			FRE_FUNC(AIRSteam_SetAchievement),
			FRE_FUNC(AIRSteam_ClearAchievement),
			FRE_FUNC(AIRSteam_IsAchievement),
			FRE_FUNC(AIRSteam_GetStatInt),
			FRE_FUNC(AIRSteam_GetStatFloat),
			FRE_FUNC(AIRSteam_SetStatInt),
			FRE_FUNC(AIRSteam_SetStatFloat),
			FRE_FUNC(AIRSteam_StoreStats),
			FRE_FUNC(AIRSteam_ResetAllStats),

			// remote storage
			FRE_FUNC(AIRSteam_GetFileCount),
			FRE_FUNC(AIRSteam_GetFileSize),
			FRE_FUNC(AIRSteam_FileExists),
			FRE_FUNC(AIRSteam_FileWrite),
			FRE_FUNC(AIRSteam_FileRead),
			FRE_FUNC(AIRSteam_FileDelete),
			FRE_FUNC(AIRSteam_IsCloudEnabledForApp),
			FRE_FUNC(AIRSteam_SetCloudEnabledForApp)
		};

		*functions = func;
		*numFunctions = sizeof(func) / sizeof(func[0]);
	}

	// A native context instance is disposed
	void ContextFinalizer(FREContext ctx) {
		AIRContext = NULL;
		// Shutdown Steam
		SteamAPI_Shutdown();
		// Delete the SteamAchievements object
		if (g_Steam)
			g_Steam = NULL;
		return;
	}

	// Initialization function of each extension
	EXPORT void ExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet,
	                           FREContextFinalizer* ctxFinalizerToSet) {
		*extDataToSet = NULL;
		*ctxInitializerToSet = &ContextInitializer;
		*ctxFinalizerToSet = &ContextFinalizer;
	}

	// Called when extension is unloaded
	EXPORT void ExtFinalizer(void* extData) {
		return;
	}
}
