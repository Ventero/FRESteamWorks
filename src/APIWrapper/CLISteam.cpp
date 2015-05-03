#include "CLISteam.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <ios>
#include <iostream>
#include <iterator>
#include <sstream>

#include <types/amfbool.hpp>
#include <types/amfbytearray.hpp>
#include <types/amfdouble.hpp>
#include <types/amfinteger.hpp>
#include <types/amfstring.hpp>
#include <utils/amfitemptr.hpp>

using namespace amf;

#ifdef DEBUG
void steamWarningMessageHook(int severity, const char* msg) {
	std::fstream f("/tmp/steam.log", std::ios::out | std::ios::app);
	f << "Severity " << severity << ": " << msg << std::endl;
}
#else
void steamWarningMessageHook(int severity, const char* msg) {
	// silently ignore
}
#endif

void CLISteam::DispatchEvent(char* code, char* level) {
	// we abuse std::cerr for event dispatching, that way it doesn't interfere
	// with the normal communication on stdout
	std::cerr << "__event__<" << code << "," << level << ">" << std::endl;
}

void CLISteam::sendData(Serializer& serializer) {
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
	std::cout << std::flush;
}

void CLISteam::sendDataTempFile(Serializer& serializer) {
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

void CLISteam::sendItem(const AmfItem& item) {
	Serializer serializer;
	serializer << item;
	sendData(serializer);
}

void CLISteam::send(bool value) {
	sendItem(AmfBool(value));
}

void CLISteam::send(int32 value) {
	sendItem(AmfInteger(value));
}

void CLISteam::send(uint32 value) {
	sendItem(AmfInteger(value));
}

void CLISteam::send(uint64 value) {
	sendItem(AmfString(std::to_string(value)));
}

void CLISteam::send(float value) {
	sendItem(AmfDouble(value));
}

void CLISteam::send(double value) {
	sendItem(AmfDouble(value));
}

void CLISteam::send(std::string value) {
	sendItem(AmfString(value));
}

void CLISteam::send(const AmfItem& value) {
	sendItem(value);
}

// Specialization for byte arrays/large objects to always go through temp files.
void CLISteam::sendBuffer(const AmfItem& byte_array) {
	Serializer serializer;
	serializer << byte_array;
	sendDataTempFile(serializer);
}

std::string CLISteam::readTempFileBuf(size_t length) {
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
