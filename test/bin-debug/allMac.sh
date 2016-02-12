#!/bin/bash

# Builds the native lib *and* the ANE (as post-build script).
(cd ../../src && xcodebuild -configuration Debug)

# Build the test app
./build.sh

# Run as requested.
if [ -z "$1" ]; then
	./runMac.sh
else
	./packageMac.sh
	open -a FRESteamWorks.app
fi
