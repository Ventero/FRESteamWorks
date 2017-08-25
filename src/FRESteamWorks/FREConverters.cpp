/*
 *  FREConverters.cpp
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero> on 2014-04-09.
 *  Copyright (c) 2012-2014 Level Up Labs, LLC. All rights reserved.
 */

/*
 * Functions to create FREObjects from native primitives.
 */

#include "FREConverters.h"

#include <sstream>

FREObject FREBool(bool value) {
	FREObject result;
	FRENewObjectFromBool(value, &result);
	return result;
}

FREObject FREInt(int32 value) {
	FREObject result;
	FRENewObjectFromInt32(value, &result);
	return result;
}

FREObject FREUint(uint32 value) {
	FREObject result;
	FRENewObjectFromUint32(value, &result);
	return result;
}

FREObject FREDouble(double value) {
	FREObject result;
	FRENewObjectFromDouble(value, &result);
	return result;
}

FREObject FREString(std::string value) {
	FREObject result;
	FRENewObjectFromUTF8(
		static_cast<uint32>(value.size()) + 1,
		(const uint8_t*)value.c_str(),
		&result);
	return result;
}

FREObject FREString(const char* value) {
	return FREString(std::string(value));
}

FREObject FREUint64(uint64 value) {
	std::stringstream stream;
	stream << value;
	return FREString(stream.str());
}

FREObject FREArray(uint32 length) {
	FREObject array;
	FRENewObject((const uint8_t*)"Array", 0, NULL, &array, NULL);
	FRESetArrayLength(array, length);

	return array;
}

FREObject FREBitmapDataFromImageRGBA(uint32 width, uint32 height, uint8* pImageRGBA)
{
	FREObject freWidth;
	FRENewObjectFromUint32(uint32_t(width), &freWidth);

	FREObject freHeight;
	FRENewObjectFromUint32(uint32_t(height), &freHeight);

	FREObject freTransparent;
	FRENewObjectFromBool(uint32_t(0), &freTransparent);

	FREObject freFillColor;
	FRENewObjectFromUint32(uint32_t(0xFF0000), &freFillColor);

	FREObject freArguments[4] = { freWidth, freHeight, freTransparent, freFillColor };

	FREObject freoBitmapData;
	FRENewObject((uint8_t *)"flash.display.BitmapData", 4, freArguments, &freoBitmapData, NULL);

	FREBitmapData freBitmapData;
	FREAcquireBitmapData(freoBitmapData, &freBitmapData);

	int x, y;
	// There may be extra pixels in each row due to the value of
	// bmd.lineStride32, we'll skip over those as needed
	int offset = freBitmapData.lineStride32 - freBitmapData.width;
	uint32_t *bmdPixels = freBitmapData.bits32;

	int index = 0;
	int bitmapWidth = freBitmapData.width;
	int bitmapHeight = freBitmapData.height;
	for (y = bitmapHeight - 1; y > -1; y--)
	{
		for (x = 0; x < bitmapWidth; x++, bmdPixels++)
		{
			index = y * freBitmapData.width + x;
			int red = *(pImageRGBA + index * 4);
			int green = *(pImageRGBA + index * 4 + 1);
			int blue = *(pImageRGBA + index * 4 + 2);
			int alpha = *(pImageRGBA + index * 4 + 3);

			// Combine values into ARGB32
			*bmdPixels = (alpha << 24) | (red << 16) | (green << 8) | blue;
		}
		bmdPixels += offset;
	}

	FREInvalidateBitmapDataRect(freoBitmapData, 0, 0, freBitmapData.width, freBitmapData.height);
	// Release our control over the bitmapData
	FREReleaseBitmapData(freoBitmapData);

	delete[] pImageRGBA;
	return freoBitmapData;
}

/*
 * Functions to extract native primitives out of FREObjects.
 */

bool FREGetString(FREObject object, std::string& str) {
	uint32 len;
	const uint8_t* data;
	FREResult res = FREGetObjectAsUTF8(object, &len, &data);
	if (res != FRE_OK) return false;

	str = std::string((const char*)data, len);
	return true;
}

bool FREGetStringP(FREObject object, std::string* str) {
	std::string s;
	if (!FREGetString(object, s)) return false;

	*str = s;
	return true;
}

bool FREGetBool(FREObject object, bool* val) {
	uint32 int_val;

	FREResult ret = FREGetObjectAsBool(object, &int_val);
	*val = (int_val != 0);

	return ret == FRE_OK;
}

bool FREGetDouble(FREObject object, double* val) {
	return (FREGetObjectAsDouble(object, val) == FRE_OK);
}

bool FREGetInt32(FREObject object, int32* val) {
	return (FREGetObjectAsInt32(object, val) == FRE_OK);
}

bool FREGetUint32(FREObject object, uint32* val) {
	// really, Adobe ...?
#ifdef LINUX
	return (FREGetObjectAsUInt32(object, val) == FRE_OK);
#else
	return (FREGetObjectAsUint32(object, val) == FRE_OK);
#endif
}

bool FREGetUint64(FREObject object, uint64* val) {
	std::string str;
	if (!FREGetString(object, str)) return false;

	// Clang doesn't support std::stoull yet...
	std::istringstream ss(str);
	if (!(ss >> *val)) return false;

	return true;
}

// Extract a string array. For getArray<T>, see the header.
std::vector<std::string> extractStringArray(FREObject object) {
	return getArray<std::string>(object, FREGetStringP);
}
