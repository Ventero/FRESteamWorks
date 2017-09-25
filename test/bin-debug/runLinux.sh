#!/bin/bash

[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

if [ $(uname -s) = Darwin ]; then
    STEAM_LIB="$STEAM_SDK/redistributable_bin/osx32"
else
    STEAM_LIB="$STEAM_SDK/redistributable_bin/linux32/libsteam_api.so"
fi

if [ -z "$SteamAppId" ] && [ ! -e "steam_appid.txt" ] && [[ "$@" != *-appid* ]]; then
	echo "\$SteamAppId not set and steam_appid.txt not found"
	exit 1
fi

if [ ! -e "$STEAM_LIB" ]; then
	echo "libsteam_api.so not found"
	exit 2
fi

if [ ! -x "NativeApps/Linux/APIWrapper" ]; then
	echo "APIWrapper not found, expected it to be in"
	echo "NativeApps/Linux/APIWrapper"
	exit 4
fi

if [ $(uname -s) = Darwin ]; then
    export DYLD_LIBRARY_PATH="$STEAM_LIB${DYLD_LIBRARY_PATH+:$DYLD_LIBRARY_PATH}"
    install_name_tool -add_rpath "$AIR_SDK/runtimes/air/mac" NativeApps/Linux/APIWrapper
else
    export LD_PRELOAD="$STEAM_LIB${LD_PRELOAD+:$LD_PRELOAD}"
fi

"$AIR_SDK/bin/adl" FRESteamWorksTest-linux.xml . -- "$@"
