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

void steamWarningMessageHook(int severity, const char* msg);

class CLISteam : public SteamWrapper {
public:
	// Event dispatching.
	virtual void DispatchEvent(char* code, char* level) override;

	// Functions for sending data to the AS3 side.
	virtual void send(Void) override { };
	virtual void send(bool value) override;
	virtual void send(int32 value) override;
	virtual void send(uint32 value) override;
	virtual void send(uint64 value) override;
	virtual void send(float value) override;
	virtual void send(double value) override;
	virtual void send(std::string value) override;
	virtual void send(const amf::AmfItem& item) override;
	virtual void sendBuffer(const amf::AmfItem& item) override;

private:
	void sendData(amf::Serializer&);
	void sendDataTempFile(amf::Serializer&);
	void sendItem(const amf::AmfItem& item);

	std::string readTempFileBuf(size_t length);
};

#endif // CLISTEAM_H
