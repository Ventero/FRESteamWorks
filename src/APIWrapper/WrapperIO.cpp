/*
 *  WrapperIO.cpp
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012-2014 Level Up Labs, LLC. All rights reserved.
 */

#include "WrapperIO.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <ios>
#include <iostream>
#include <iterator>
#include <sstream>

using namespace amf;

void sendData(Serializer& serializer) {
	std::vector<u8> data = serializer.data();
	if(data.size() > 1024) {
		// due to output buffering, large outputs need to be sent via a tempfile
		sendDataTempFile(serializer);
		return;
	}

	serializer.clear();

	// first, send a length prefix
	std::vector<u8> length = AmfInteger(data.size()).serialize();
	std::copy(length.begin(), length.end(), std::ostream_iterator<u8>(std::cout));
	// send actual data
	std::copy(data.begin(), data.end(), std::ostream_iterator<u8>(std::cout));
}

void sendDataTempFile(Serializer& serializer) {
	std::vector<u8> data = serializer.data();
	serializer.clear();

	// send length via stdout
	std::vector<u8> length = AmfInteger(data.size()).serialize();
	std::copy(length.begin(), length.end(), std::ostream_iterator<u8>(std::cout));

	std::string filename = std::tmpnam(nullptr);
	std::fstream tmpfile(filename, std::ios::out | std::ios::binary);
	std::copy(data.begin(), data.end(), std::ostream_iterator<u8>(tmpfile));
	tmpfile.close();
	send(filename);
}

void sendItem(const AmfItem& item) {
	Serializer serializer;
	serializer << item;
	sendData(serializer);
}

void send(bool value) {
	sendItem(AmfBool(value));
}

void send(int32 value) {
	sendItem(AmfInteger(value));
}

void send(uint32 value) {
	sendItem(AmfInteger(value));
}

void send(uint64 value) {
	sendItem(AmfString(std::to_string(value)));
}

void send(float value) {
	sendItem(AmfDouble(value));
}

void send(std::string value) {
	sendItem(AmfString(value));
}

void send(const char* value) {
	sendItem(AmfString(value));
}

void send(std::nullptr_t) {
	sendItem(AmfNull());
}

// TODO: replace this mess with AMF
bool get_bool() {
	std::string item;
	std::getline(std::cin, item);
	return item == "true";
}

int32 get_int() {
	std::string item;
	std::getline(std::cin, item);
	return std::stoi(item);
}

float get_float() {
	std::string item;
	std::getline(std::cin, item);
	return std::stof(item);
}

std::string get_string() {
	std::string item;
	std::getline(std::cin, item);

	size_t length = std::stoi(item);
	if (length < 1) return "";
	char* buf = new char[length];

	if (length > 1024)
		return readTempFileBuf(length);

	std::cin.read(buf, length);
	// remove trailing newline
	std::string result(buf, length - 1);

	delete[] buf;
	return result;
}

std::string get_bytearray() {
	std::string item;
	std::getline(std::cin, item);

	size_t length = std::stoi(item);
	if (length < 1) return "";
	return readTempFileBuf(length);
}

uint64 get_uint64() {
	std::string str = get_string();
	std::istringstream ss(str);

	uint64 val;
	if(!(ss >> val)) return 0;

	return val;
}

std::vector<std::string> get_string_array() {
	return get_array<std::string>(get_string);
}

std::string readTempFileBuf(size_t length) {
	std::string filename;
	std::getline(std::cin, filename);
	std::ifstream tempfile(filename, std::ios::in | std::ios::binary);

	char* buf = new char[length];
	tempfile.read(buf, length);

	std::string result(buf, length - 1);

	delete[] buf;
	tempfile.close();
	std::remove(filename.c_str());

	return result;
}
