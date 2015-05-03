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

#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>

CLISteam *g_Steam = NULL;

/*
 * Wrappers
 */

std::vector<std::function<void()>> apiFunctions {
#define X(a) []() { g_Steam->send(a()); },
#include "functions.h"
#undef X
};

int main(int argc, char** argv) {
	g_Steam = new CLISteam();

	std::ios::sync_with_stdio(false);

	while (std::cin.good()) {
		std::string buf;
		std::getline(std::cin, buf);

		if (buf.empty()) break;

		unsigned int func;
		try {
			func = std::stoi(buf);
		} catch (std::exception& e) {
			// conversion failed, either stdin was closed or I/O is desynced
			// either way, abort
			break;
		}

		if (func >= apiFunctions.size())
			continue;

		try {
			apiFunctions[func]();
		} catch (std::exception& e) {
			std::string msg("Exception: ");
			msg += e.what();
			msg += "\n";
			msg += buf;
			steamWarningMessageHook(2, msg.c_str());
			continue;
		} catch (...) {
			// shouldn't happen, just read on and hope for the best
			steamWarningMessageHook(2, "exception caught");
			continue;
		}
	}

	AIRSteam_Shutdown();
	return 0;
}
