//
//  FRESteamWorks.cpp
//  FRESteamWorks
//
//  Created by David `Oldes` Oliva on 3/29/12.
//  Copyright (c) 2012 Amanita Design. All rights reserved.
//

#include "FRESteamWorks.h"

FREContext  AIRContext; // Used to dispatch event back to AIR

// Global access to Steam object
CSteam*	g_Steam = NULL;

CSteam::CSteam(): 				
m_iAppID( 0 ),
m_bInitialized( false ),
m_CallbackUserStatsReceived( this, &CSteam::OnUserStatsReceived ),
m_CallbackUserStatsStored( this, &CSteam::OnUserStatsStored ),
m_CallbackAchievementStored( this, &CSteam::OnAchievementStored ),
m_CallbackGameOverlayActivated( this, &CSteam::OnGameOverlayActivated )
{
    m_iAppID = SteamUtils()->GetAppID();
    m_bInitialized = !( NULL == SteamUserStats() || NULL == SteamUser() );
    RequestStats();
}

bool CSteam::RequestStats() {
	// Is Steam loaded? If not we can't get stats.
	if ( NULL == SteamUserStats() || NULL == SteamUser() ) {
		return false;
	}
	// Is the user logged on?  If not we can't get stats.
	if ( !SteamUser()->BLoggedOn() ) {
		return false;
	}
	// Request user stats.
	return SteamUserStats()->RequestCurrentStats();
}

bool CSteam::SetAchievement(const char* ID) {
	bool result = false;
	if (m_bInitialized) {
		SteamUserStats()->SetAchievement(ID);
		return SteamUserStats()->StoreStats();
	}
	return result;
}

bool CSteam::ClearAchievement(const char* ID) {
	bool result = false;
	if (m_bInitialized) {
		SteamUserStats()->ClearAchievement(ID);
		return SteamUserStats()->StoreStats();
	}
	return result;
}

bool CSteam::IsAchievement(const char* ID) {
	bool result = false;
	if (m_bInitialized) SteamUserStats()->GetAchievement(ID, &result);
	return result;
}

bool CSteam::GetStat(const char* ID, int32 *value) {
	bool result = false;
	if (m_bInitialized) result = SteamUserStats()->GetStat(ID, value);
	return result;
}
bool CSteam::GetStat(const char* ID, float *value) {
	bool result = false;
	if (m_bInitialized) result = SteamUserStats()->GetStat(ID, value);
	return result;
}


bool CSteam::SetStat(const char* ID, int32 value) {
	bool result = false;
	if (m_bInitialized) result = SteamUserStats()->SetStat(ID, value);
	return result;
}
bool CSteam::SetStat(const char* ID, float value) {
	bool result = false;
	if (m_bInitialized) result = SteamUserStats()->SetStat(ID, value);
	return result;
}

bool CSteam::StoreStats() {
	bool result = false;
	if (m_bInitialized) result = SteamUserStats()->StoreStats();
	return result;
}

bool CSteam::ResetAllStats( bool bAchievementsToo ) {
	if (m_bInitialized) {
		SteamUserStats()->ResetAllStats(bAchievementsToo);
		return SteamUserStats()->StoreStats();
	}
	return false;
}

void CSteam::DispatchEvent(const int req_type, const int response) {
	FREResult res;
	char code[5];
	char level[5];
    
	sprintf(code,  "%d", req_type);
	sprintf(level, "%d", response);
	if((res=FREDispatchStatusEventAsync(AIRContext, (const uint8_t*)code, (const uint8_t*)level)) != FRE_OK)
	{
		//Debug::logDebug("ERROR: FREDispatchStatusEventAsync(ctx, (const uint8_t*)code, (const uint8_t*)level) = %d", res);
		return;
	}
}

void CSteam::OnUserStatsReceived( UserStatsReceived_t *pCallback ) {
	// we may get callbacks for other games' stats arriving, ignore them
	if ( m_iAppID == pCallback->m_nGameID )	{
		g_Steam->DispatchEvent(RESPONSE_OnUserStatsReceived, pCallback->m_eResult);
	}
}


void CSteam::OnUserStatsStored( UserStatsStored_t *pCallback ) {
	// we may get callbacks for other games' stats arriving, ignore them
	if ( m_iAppID == pCallback->m_nGameID )	{
		DispatchEvent(RESPONSE_OnUserStatsStored, pCallback->m_eResult);
	}
}


void CSteam::OnAchievementStored( UserAchievementStored_t *pCallback ) {
    // we may get callbacks for other games' stats arriving, ignore them
    if ( m_iAppID == pCallback->m_nGameID ) {
        g_Steam->DispatchEvent(RESPONSE_OnAchievementStored, RESPONSE_OK);
    }
}

void CSteam::OnGameOverlayActivated( GameOverlayActivated_t *pCallback ) {
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
			FREResult res;
			uint32_t len = -1;
			const uint8_t *ID = 0;
			if((res=FREGetObjectAsUTF8(argv[0], &len, &ID)) == FRE_OK) {
				FRENewObjectFromBool((uint32_t)g_Steam->SetAchievement((const char *)ID), &result);
			}
		}
		if (NULL == result) FRENewObjectFromBool(false, &result);
		SteamAPI_RunCallbacks();
		return result;
	}
	
	FREObject AIRSteam_ClearAchievement(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result = NULL;
		if (g_Steam) {
			FREResult res;
			uint32_t len = -1;
			const uint8_t *ID = 0;
			if((res=FREGetObjectAsUTF8(argv[0], &len, &ID)) == FRE_OK) {
				FRENewObjectFromBool((uint32_t)g_Steam->ClearAchievement((const char *)ID), &result);
			}
		}
		if (NULL == result) FRENewObjectFromBool(false, &result);
		return result;
	}
    
	FREObject AIRSteam_IsAchievement(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result = NULL;
		if (g_Steam && argc == 1) {
			FREResult res;
			uint32_t len = -1;
			const uint8_t *ID = 0;
			if((res=FREGetObjectAsUTF8(argv[0], &len, &ID)) == FRE_OK) {
				FRENewObjectFromBool((uint32_t)g_Steam->IsAchievement((const char *)ID), &result);
			}
		}
		if (NULL == result) FRENewObjectFromBool(false, &result);
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
			FREResult res;
			uint32_t len = -1;
			const uint8_t *ID = 0;
			if((res=FREGetObjectAsUTF8(argv[0], &len, &ID)) == FRE_OK) {
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
			FREResult res;
			uint32_t len = -1;
			const uint8_t *ID = 0;
			if((res=FREGetObjectAsUTF8(argv[0], &len, &ID)) == FRE_OK) {
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
			if(
               FREGetObjectAsUTF8(argv[0], &len, &ID) == FRE_OK
               && FREGetObjectAsInt32(argv[1], &value) == FRE_OK
               ) {
				FRENewObjectFromBool((uint32_t)g_Steam->SetStat((const char *)ID, value), &result);
			}
		}
        if (NULL == result) FRENewObjectFromBool(false, &result);
		return result;
	}
	FREObject AIRSteam_SetStatFloat(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result = NULL;
		if (g_Steam && argc == 2) {
			uint32_t len = -1;
			const uint8_t *ID = 0;
			double value;
			if(
               FREGetObjectAsUTF8(argv[0], &len, &ID) == FRE_OK
               && FREGetObjectAsDouble(argv[1], &value) == FRE_OK
               ) {
				FRENewObjectFromBool((uint32_t)g_Steam->SetStat((const char *)ID, (float)value), &result);
			}
		}
		if (NULL == result) FRENewObjectFromBool(false, &result);
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
			if( FREGetObjectAsBool( argv[0], &bAchievementsToo ) == FRE_OK ) {
				FRENewObjectFromBool((uint32_t)g_Steam->ResetAllStats((bAchievementsToo!=0)), &result);
			}
		}
		if ( NULL == result ) FRENewObjectFromBool(false, &result);
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
			if(	FREGetObjectAsUTF8(argv[0], &len, &fileName) == FRE_OK ) {
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
			if(	FREGetObjectAsUTF8(argv[0], &len, &fileName) == FRE_OK ) {
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
			if(
               FREGetObjectAsUTF8(argv[0], &len, &fileName) == FRE_OK
               && FREAcquireByteArray(argv[1], &byteArray) == FRE_OK
               ) {
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
			if(
               FREGetObjectAsUTF8(argv[0], &len, &fileName) == FRE_OK
               && FREAcquireByteArray(argv[1], &byteArray) == FRE_OK
               ) {
				fileSize = SteamRemoteStorage()->GetFileSize((char *)fileName);
				if(fileSize > 0 && fileSize <= byteArray.length) {
					dataIn = (char *)malloc(fileSize);
					retVal = SteamRemoteStorage()->FileRead((char *)fileName, dataIn, fileSize);
					memcpy (byteArray.bytes, dataIn, fileSize); 
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
			if(	FREGetObjectAsUTF8(argv[0], &len, &fileName) == FRE_OK ) {
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
			if(	FREGetObjectAsBool(argv[0], &enabled) == FRE_OK ) {
				bEnabled = (enabled!=0);
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
        
        *numFunctions = 20;
        
        FRENamedFunction* func = (FRENamedFunction*) malloc(sizeof(FRENamedFunction) * (*numFunctions));
        
        func[0].name = (const uint8_t*) "AIRSteam_Init";
        func[0].functionData = NULL;
        func[0].function = &AIRSteam_Init;
        
        func[1].name = (const uint8_t*) "AIRSteam_RequestStats";
        func[1].functionData = NULL;
        func[1].function = &AIRSteam_RequestStats;
        
        func[2].name = (const uint8_t*) "AIRSteam_SetAchievement";
        func[2].functionData = NULL;
        func[2].function = &AIRSteam_SetAchievement;
        
        func[3].name = (const uint8_t*) "AIRSteam_ClearAchievement";
        func[3].functionData = NULL;
        func[3].function = &AIRSteam_ClearAchievement;
        
        func[4].name = (const uint8_t*) "AIRSteam_IsAchievement";
        func[4].functionData = NULL;
        func[4].function = &AIRSteam_IsAchievement;
        
        func[5].name = (const uint8_t*) "AIRSteam_GetStatInt";
        func[5].functionData = NULL;
        func[5].function = &AIRSteam_GetStatInt;
        
        func[6].name = (const uint8_t*) "AIRSteam_GetStatFloat";
        func[6].functionData = NULL;
        func[6].function = &AIRSteam_GetStatFloat;
        
        func[7].name = (const uint8_t*) "AIRSteam_SetStatInt";
        func[7].functionData = NULL;
        func[7].function = &AIRSteam_SetStatInt;
        
        func[8].name = (const uint8_t*) "AIRSteam_SetStatFloat";
        func[8].functionData = NULL;
        func[8].function = &AIRSteam_SetStatFloat;
        
        func[9].name = (const uint8_t*) "AIRSteam_StoreStats";
        func[9].functionData = NULL;
        func[9].function = &AIRSteam_StoreStats;
        
        func[10].name = (const uint8_t*) "AIRSteam_RunCallbacks";
        func[10].functionData = NULL;
        func[10].function = &AIRSteam_RunCallbacks;
        
        func[11].name = (const uint8_t*) "AIRSteam_ResetAllStats";
        func[11].functionData = NULL;
        func[11].function = &AIRSteam_ResetAllStats;
        
        //SteamRemoteStorage...
        func[12].name = (const uint8_t*) "AIRSteam_GetFileCount";
        func[12].functionData = NULL;
        func[12].function = &AIRSteam_GetFileCount;
        
        func[13].name = (const uint8_t*) "AIRSteam_GetFileSize";
        func[13].functionData = NULL;
        func[13].function = &AIRSteam_GetFileSize;
        
        func[14].name = (const uint8_t*) "AIRSteam_FileExists";
        func[14].functionData = NULL;
        func[14].function = &AIRSteam_FileExists;
        
        func[15].name = (const uint8_t*) "AIRSteam_FileWrite";
        func[15].functionData = NULL;
        func[15].function = &AIRSteam_FileWrite;
        
        func[16].name = (const uint8_t*) "AIRSteam_FileRead";
        func[16].functionData = NULL;
        func[16].function = &AIRSteam_FileRead;
        
        func[17].name = (const uint8_t*) "AIRSteam_FileDelete";
        func[17].functionData = NULL;
        func[17].function = &AIRSteam_FileDelete;
        
        func[18].name = (const uint8_t*) "AIRSteam_IsCloudEnabledForApp";
        func[18].functionData = NULL;
        func[18].function = &AIRSteam_IsCloudEnabledForApp;
        
        func[19].name = (const uint8_t*) "AIRSteam_SetCloudEnabledForApp";
        func[19].functionData = NULL;
        func[19].function = &AIRSteam_SetCloudEnabledForApp;
        
        *functions = func;
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
