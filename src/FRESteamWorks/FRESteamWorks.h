/*
 *  FRESteamWorks.h
 *  This file is part of FRESteamWorks.
 *
 *  Created by David ´Oldes´ Oliva on 3/29/12.
 *  Contributors: Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012 Amanita Design. All rights reserved.
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

#ifndef FRESTEAMWORKS_H
#define FRESTEAMWORKS_H

#if defined(WIN32)
	#define EXPORT __declspec(dllexport)
#else
	// Symbols tagged with EXPORT are externally visible.
	// Must use the -fvisibility=hidden gcc option.
	#define EXPORT __attribute__((visibility("default")))
#endif

#include <FlashRuntimeExtensions.h>

#include "CSteam/CSteam.h"

void steamWarningMessageHook(int severity, const char* msg);

class ANESteam : public CSteam {
	void DispatchEvent(char* code, char* level);
};

void ContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx,
                        uint32* numFunctions, const FRENamedFunction** functions);
void ContextFinalizer(FREContext ctx);

extern "C" {
	EXPORT void ExtInitializerFRESteamWorks(void** extDataToSet, FREContextInitializer* ctxInitializerToSet,
	                           FREContextFinalizer* ctxFinalizerToSet);
	EXPORT void ExtFinalizerFRESteamWorks(void* extData);
}

#endif
