/*
 *  SteamWrapper.h
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2015 Ventero. All rights reserved.
 */

#ifndef STEAMWRAPPER_H
#define STEAMWRAPPER_H

#include <string>
#include <vector>

#include <types/amfitem.hpp>

#include <steam/steamtypes.h>

#include "CSteam/CSteam.h"

/* A wrapper class around CSteam that also defines an interface for communication
 * with the AS3 side.
 */
class SteamWrapper : public CSteam {
public:
	// Required by CSteam
	virtual void DispatchEvent(char* code, char* level) = 0;

	// Functions for sending data to the AS3 side.
	virtual void send(bool value) = 0;
	virtual void send(int32 value) = 0;
	virtual void send(uint32 value) = 0;
	virtual void send(uint64 value) = 0;
	virtual void send(float value) = 0;
	virtual void send(double value) = 0;
	virtual void send(std::string value) = 0;
	virtual void send(const amf::AmfItem& item) = 0;
	// Used for sending large buffers.
	virtual void sendBuffer(const amf::AmfItem& item) = 0;
	// Required for functions that don't return any data.
	virtual void send(std::nullptr_t) = 0;

	// Functions for getting data.
	virtual bool get_bool() = 0;
	virtual int32 get_int() = 0;
	virtual float get_float() = 0;
	virtual std::string get_string() = 0;
	virtual std::string get_bytearray() = 0;
	virtual uint64 get_uint64() = 0;
	virtual std::vector<int> get_int_array() = 0;
	virtual std::vector<std::string> get_string_array() = 0;
};

#endif // STEAMWRAPPER_H
