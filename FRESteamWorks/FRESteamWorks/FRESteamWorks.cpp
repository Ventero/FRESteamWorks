#define _CRT_SECURE_NO_WARNINGS
#define DllExport   __declspec( dllexport ) 

#include "FlashRuntimeExtensions.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <steam_api.h>
#include <ISteamRemoteStorage.h>

#include "FRESteamWorks.h"


FREContext  AIRContext; // Used to dispatch event back to AIR

// Global access to Steam object
CSteam*	g_Steam = NULL;


CSteam::CSteam(): 				
 m_iAppID( 0 ),
 m_bInitialized( false ),
 m_CallbackUserStatsReceived( this, &CSteam::OnUserStatsReceived ),
 m_CallbackUserStatsStored( this, &CSteam::OnUserStatsStored ),
 m_CallbackAchievementStored( this, &CSteam::OnAchievementStored )
{
     m_iAppID = SteamUtils()->GetAppID();
	 m_bInitialized = !( NULL == SteamUserStats() || NULL == SteamUser() );
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
	bool result;
	if (m_bInitialized) {
		SteamUserStats()->SetAchievement(ID);
		return SteamUserStats()->StoreStats();
	}
	return result;
}

bool CSteam::ClearAchievement(const char* ID) {
	bool result;
	if (m_bInitialized) {
		SteamUserStats()->ClearAchievement(ID);
		return SteamUserStats()->StoreStats();
	}
	return result;
}

bool CSteam::IsAchievement(const char* ID) {
	bool result;
	if (m_bInitialized) SteamUserStats()->GetAchievement(ID, &result);
	return result;
}

bool CSteam::GetStat(const char* ID, int32 *value) {
	bool result;
	if (m_bInitialized) result = SteamUserStats()->GetStat(ID, value);
	return result;
}
bool CSteam::GetStat(const char* ID, float *value) {
	bool result;
	if (m_bInitialized) result = SteamUserStats()->GetStat(ID, value);
	return result;
}


bool CSteam::SetStat(const char* ID, int32 value) {
	bool result;
	if (m_bInitialized) result = SteamUserStats()->SetStat(ID, value);
	return result;
}
bool CSteam::SetStat(const char* ID, float value) {
	bool result;
	if (m_bInitialized) result = SteamUserStats()->SetStat(ID, value);
	return result;
}

bool CSteam::StoreStats() {
	bool result;
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
		FREObject result;
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
		FREObject result;
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
		FREObject result;
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
		if (g_Steam) {
			FREResult res;
			uint32_t len = -1;
			const uint8_t *ID = 0;
			int32 value;
			if((res=FREGetObjectAsUTF8(argv[0], &len, &ID)) == FRE_OK) {
				g_Steam->GetStat((const char *)ID, &value);
				FRENewObjectFromInt32(value, &result);
			}
		}
		return result;
	}

	FREObject AIRSteam_GetStatFloat(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
		if (g_Steam) {
			FREResult res;
			uint32_t len = -1;
			const uint8_t *ID = 0;
			float value;
			if((res=FREGetObjectAsUTF8(argv[0], &len, &ID)) == FRE_OK) {
				g_Steam->GetStat((const char *)ID, &value);
				FRENewObjectFromDouble(value, &result);
			}
		}
		return result;
	}

	FREObject AIRSteam_SetStatInt(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
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
		return result;
	}
	FREObject AIRSteam_SetStatFloat(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
		FREObject result;
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
		FREObject result;
		if (g_Steam && argc == 1) {
			uint32_t bAchievementsToo;
			if( FREGetObjectAsBool( argv[0], &bAchievementsToo ) == FRE_OK ) {
				FRENewObjectFromBool((uint32_t)g_Steam->ResetAllStats((bAchievementsToo!=0)), &result);
			}
		}
		if ( NULL == result ) FRENewObjectFromBool(false, &result);
		return result;
	}
	

	//============================

	void contextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctions, const FRENamedFunction** functions)
	{
		AIRContext = ctx;

		*numFunctions = 12;

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
		
		*functions = func;

	}

	void contextFinalizer(FREContext ctx)
	{
		AIRContext = NULL;
		// Shutdown Steam
		SteamAPI_Shutdown();
		// Delete the SteamAchievements object
		if (g_Steam)
			g_Steam = NULL;
		return;
	}

	DllExport void initializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer)
	{
		*ctxInitializer = &contextInitializer;
		*ctxFinalizer = &contextFinalizer;
	}

	DllExport void finalizer(void* extData)
	{
		return;
	}
}

