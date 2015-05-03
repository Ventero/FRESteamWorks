/*
 *  APIWrapper.cpp
 *  This file is part of FRESteamWorks.
 *
 *  Created by Ventero <http://github.com/Ventero>
 *  Copyright (c) 2012-2013 Level Up Labs, LLC. All rights reserved.
 */

#include "APIFunctions.h"
#include "CLISteam.h"
#include "SteamWrapper.h"

#include "deserializer.hpp"
#include "types/amfinteger.hpp"

#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <sstream>

CLISteam *g_Steam = NULL;

/*
 * Wrappers
 */

std::vector<std::function<void(const amf::AmfArray&)>> apiFunctions {
#define X(a) [](const amf::AmfArray& ptr) { g_Steam->send(a(ptr)); },
#include "functions.h"
#undef X
};

int main(int argc, char** argv) {
	g_Steam = new CLISteam();

	std::ios::sync_with_stdio(false);

	amf::Deserializer deserializer;
	while (std::cin.good()) {
		std::string buf;
		std::getline(std::cin, buf);
		if (buf.empty()) break;

		size_t length = std::stoi(buf);
		amf::v8 data(length);
		std::cin.read(reinterpret_cast<char*>(data.data()), length);

		unsigned int func;
		amf::AmfArray args;
		try {
			auto it = data.cbegin();
			auto end = data.cend();

			func = deserializer.deserialize(it, end).as<amf::AmfInteger>().value;
			args = deserializer.deserialize(it, end).as<amf::AmfArray>();
			deserializer.clearContext();
		} catch (std::exception& e) {
			// reading function index or arguments failed, probably due to
			// desynced I/O
			break;
		}

		if (func >= apiFunctions.size())
			continue;

		try {
			apiFunctions[func](args);
		} catch (std::exception& e) {
			std::ostringstream oss;
			oss << "Exception while calling " << func << ":\n"
			    << e.what() << "\n" << buf << std::endl;
			steamWarningMessageHook(2, oss.str().c_str());
			continue;
		} catch (...) {
			// shouldn't happen, just read on and hope for the best
			steamWarningMessageHook(2, "exception caught");
			continue;
		}
	}

	AIRSteam_Shutdown(amf::AmfArray());
	return 0;
}
