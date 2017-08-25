/*
 *  FREConverters.cpp
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero> on 2014-04-09.
 *  Copyright (c) 2012-2014 Level Up Labs, LLC. All rights reserved.
 */

#ifndef FRECONVERTERS_H
#define FRECONVERTERS_H

#include <string>
#include <vector>

#include <FlashRuntimeExtensions.h>
// includes definitions of {u,}int{32,64}
#include <steam/steamtypes.h>

// Utility functions for conversion of FRE types
FREObject FREBool(bool);
FREObject FREDouble(double);
FREObject FREInt(int32);
FREObject FREUint(uint32);
FREObject FREUint64(uint64);
FREObject FREString(std::string);
FREObject FREString(const char*);
FREObject FREArray(uint32);
FREObject FREBitmapDataFromImageRGBA(uint32 width, uint32 height, uint8* imageRGBA);
bool FREGetBool(FREObject object, bool* val);
bool FREGetDouble(FREObject, double* val);
bool FREGetInt32(FREObject, int32* val);
bool FREGetUint32(FREObject, uint32* val);
bool FREGetUint64(FREObject, uint64* val);
bool FREGetString(FREObject, std::string& val);
bool FREGetStringP(FREObject, std::string* val);

// Extracts a std::vector<T> out of an FREObject array.
// The converter function is one of the FREGet* functions above.
template<typename T, typename Converter>
std::vector<T> getArray(FREObject object, Converter conv) {
	std::vector<T> ret;

	uint32 arrayLength;
	if (FREGetArrayLength(object, &arrayLength) != FRE_OK)
		return ret;

	if (!arrayLength)
		return ret;

	ret.reserve(arrayLength);
	for (uint32 i = 0; i < arrayLength; ++i) {
		FREObject value;
		if (FREGetArrayElementAt(object, i, &value) != FRE_OK)
			continue;

		T val;
		if (!conv(value, &val))
			continue;

		ret.push_back(val);
	}

	return ret;
}

// Commonly used alias to extract a string array.
std::vector<std::string> extractStringArray(FREObject object);

#endif
