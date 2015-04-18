/*
 *  CLISteam.h
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2014-2015 Ventero. All rights reserved.
 */

#ifndef CLISTEAM_H
#define CLISTEAM_H

#include "SteamWrapper.h"

#include <types/amfitem.hpp>
#include <serializer.hpp>

class CLISteam : public SteamWrapper {
public:
	// Event dispatching.
	virtual void DispatchEvent(char* code, char* level);

	// Functions for sending data to the AS3 side.
	virtual void send(bool value);
	virtual void send(int32 value);
	virtual void send(uint32 value);
	virtual void send(uint64 value);
	virtual void send(float value);
	virtual void send(double value);
	virtual void send(std::string value);
	virtual void send(const amf::AmfItem& item);
	virtual void send(std::nullptr_t);

	void send(const amf::AmfByteArray& byte_array);

	// Functions for getting data.
	virtual bool get_bool();
	virtual int32 get_int();
	virtual float get_float();
	virtual std::string get_string();
	virtual std::string get_bytearray();
	virtual uint64 get_uint64();
	virtual std::vector<int> get_int_array();
	virtual std::vector<std::string> get_string_array();

private:
	void sendData(amf::Serializer&);
	void sendDataTempFile(amf::Serializer&);

	void sendItem(const amf::AmfItem& item);

	std::string readTempFileBuf(size_t length);

	template<typename T, typename Getter>
	std::vector<T> get_array(Getter get) {
		int length = get_int();
		std::vector<T> v;
		if (length < 1) return v;

		v.reserve(length);
		for(int i = 0; i < length; ++i) {
			T val = (this->*get)();
			v.push_back(val);
		}

		return v;
	}
};

#endif // CLISTEAM_H
