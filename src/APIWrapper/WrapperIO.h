/*
 *  WrapperIO.h
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012-2014 Level Up Labs, LLC. All rights reserved.
 */

#ifndef WRAPPERIO_H
#define WRAPPERIO_H

#include <string>
#include <vector>

#include <steamtypes.h>

#include "amf-cpp/types/amfitem.hpp"
#include "amf-cpp/serializer.hpp"

void sendData(amf::Serializer&);
void sendDataTempFile(amf::Serializer&);

void sendItem(const amf::AmfItem& item);

void send(bool value);
void send(int32 value);
void send(uint32 value);
void send(uint64 value);
void send(float value);
void send(const char* value);
void send(std::string value);
void send(std::nullptr_t);

std::string readTempFileBuf(size_t length);

bool get_bool();
int32 get_int();
float get_float();
std::string get_string();
std::string get_bytearray();
uint64 get_uint64();

template<typename T, typename Getter>
std::vector<T> get_array(Getter get) {
	int length = get_int();
	std::vector<T> v;
	if (length < 1) return v;

	v.reserve(length);
	for(int i = 0; i < length; ++i) {
		T val = get();
		v.push_back(val);
	}

	return v;
}

std::vector<std::string> get_string_array();

#endif
