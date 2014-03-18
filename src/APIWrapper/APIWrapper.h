/*
 *  APIWrapper.h
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

#ifndef APIWRAPPER_H
#define APIWRAPPER_H

#include <steam_api.h>

#include "CSteam.h"
#include "WrapperIO.h"

class CLISteam : public CSteam {
	void DispatchEvent(char* code, char* level);
};

void steamWarningMessageHook(int severity, const char* msg);

#endif
