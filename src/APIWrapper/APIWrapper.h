/*
 *  APIWrapper.h
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "CSteam.h"

#include "amf-cpp/types/amfitem.hpp"
#include "amf-cpp/types/amfbool.hpp"
#include "amf-cpp/types/amfdouble.hpp"
#include "amf-cpp/types/amfinteger.hpp"
#include "amf-cpp/serializer.hpp"

#include <steam_api.h>
#include <isteamremotestorage.h>

class CLISteam : public CSteam {
	void DispatchEvent(char* code, char* level);
};

void sendData(Serializer&);

template<class T>
void sendItem(T item);

void send(bool value);
void send(int32 value);
void send(uint32 value);
void send(uint64 value);
void send(float value);
void send(std::string value);
void send(std::nullptr_t);

bool get_bool();
int32 get_int();
float get_float();
std::string get_string();
uint64 get_uint64();
std::vector<std::string> get_array();

void steamWarningMessageHook(int severity, const char* msg);

#define X(a) void a();
#include "functions.h"
#undef X

std::vector<std::function<void()>> apiFunctions {
#define X(a) std::function<void()>(a),
#include "functions.h"
#undef X
};
