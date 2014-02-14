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
FREObject FREDouble(double);
FREObject FREInt(int32);
FREObject FREUint(uint32);
FREObject FREUint64(uint64);
FREObject FREString(std::string);
FREObject FREString(const char*);
FREObject FREArray(uint32_t);
bool FREGetBool(FREObject, uint32* val);
bool FREGetDouble(FREObject, double* val);
bool FREGetInt32(FREObject, int32* val);
bool FREGetUint32(FREObject, uint32* val);
bool FREGetUint64(FREObject, uint64* val);
bool FREGetString(FREObject, std::string& val);
bool FREGetStringP(FREObject, std::string* val);

template<typename T, typename Converter>
std::vector<T> getArray(FREObject object, Converter conv);

std::vector<std::string> extractStringArray(FREObject object);
void steamWarningMessageHook(int severity, const char* msg);

class ANESteam : public CSteam {
	void DispatchEvent(char* code, char* level);
};

#define X(a) AIR_FUNC(a);
#include "functions.h"
#undef X

void ContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx,
                        uint32_t* numFunctions, const FRENamedFunction** functions);
void ContextFinalizer(FREContext ctx);

extern "C" {
	EXPORT void ExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet,
	                           FREContextFinalizer* ctxFinalizerToSet);
	EXPORT void ExtFinalizer(void* extData);
}
