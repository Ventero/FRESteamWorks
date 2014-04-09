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

#include <FlashRuntimeExtensions.h>

#include "CSteam.h"

#define AIR_FUNC(name) \
	FREObject name(FREContext ctx, void* funcData, uint32 argc, FREObject argv[])

#define ARG_CHECK(arg_num, default) \
	if (!g_Steam || argc != arg_num) return default

#define SET_PROP(object, property, value) \
	FRESetObjectProperty(object, (const uint8_t*)property, value, NULL)

void steamWarningMessageHook(int severity, const char* msg);

class ANESteam : public CSteam {
	void DispatchEvent(char* code, char* level);
};

#define X(a) AIR_FUNC(a);
#include "functions.h"
#undef X

void ContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx,
                        uint32* numFunctions, const FRENamedFunction** functions);
void ContextFinalizer(FREContext ctx);

extern "C" {
	EXPORT void ExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet,
	                           FREContextFinalizer* ctxFinalizerToSet);
	EXPORT void ExtFinalizer(void* extData);
}
