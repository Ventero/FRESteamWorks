/*
 *  APIWrapper.h
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

#include <cstdlib>
#include <iostream>
#include <string>

#include "CSteam.h"

#include <steam_api.h>
#include <isteamremotestorage.h>

class CLISteam : public CSteam {
	void DispatchEvent(char* code, char* level);
};

enum APIFunc {
	AIRSteam_Init = 0,
	AIRSteam_RunCallbacks,
	AIRSteam_RequestStats,
	AIRSteam_SetAchievement,
	AIRSteam_ClearAchievement,
	AIRSteam_IsAchievement,
	AIRSteam_GetStatInt,
	AIRSteam_GetStatFloat,
	AIRSteam_SetStatInt,
	AIRSteam_SetStatFloat,
	AIRSteam_StoreStats,
	AIRSteam_ResetAllStats,
	AIRSteam_GetFileCount,
	AIRSteam_GetFileSize,
	AIRSteam_FileExists,
	AIRSteam_FileWrite,
	AIRSteam_FileRead,
	AIRSteam_FileDelete,
	AIRSteam_IsCloudEnabledForApp,
	AIRSteam_SetCloudEnabledForApp,
	AIRSteam_GetUserID,
	AIRSteam_GetPersonaName
};
