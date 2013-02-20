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
	#define EXPORT __declspec(dllexport)
#else
	// Symbols tagged with EXPORT are externally visible.
	// Must use the -fvisibility=hidden gcc option.
	#define EXPORT __attribute__((visibility("default")))
#endif

#include <string>
#include <stdlib.h>

#include <FlashRuntimeExtensions.h>

#include <steam_api.h>
#include <isteamremotestorage.h>

#include "CSteam.h"

// utility functions for conversion of FRE types
FREObject FREBool(bool);
FREObject FREInt(int32);
FREObject FREFloat(float);
FREObject FREString(std::string);
std::string FREGetString(FREObject);

class ANESteam : public CSteam {
	void DispatchEvent(char* code, char* level);
};

extern "C" {
	FREObject AIRSteam_Init(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_RunCallbacks(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_GetUserID(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_GetPersonaName(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_RequestStats(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_SetAchievement(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_ClearAchievement(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_IsAchievement(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_GetStatInt(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_GetStatFloat(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_SetStatInt(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_SetStatFloat(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_StoreStats(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_ResetAllStats(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	//Steam Cloud
	FREObject AIRSteam_GetFileCount(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_GetFileSize(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_FileExists(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_FileWrite(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_FileRead(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_FileDelete(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_IsCloudEnabledForApp(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	FREObject AIRSteam_SetCloudEnabledForApp(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]);
	// A native context instance is created
	void ContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx,
	                        uint32_t* numFunctions, const FRENamedFunction** functions);
	void ContextFinalizer(FREContext ctx);
	EXPORT void ExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet,
	                           FREContextFinalizer* ctxFinalizerToSet);
	EXPORT void ExtFinalizer(void* extData);
}
