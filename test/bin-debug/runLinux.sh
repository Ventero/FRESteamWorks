#!/bin/bash

[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

STEAM_LIB="$STEAM_SDK/redistributable_bin/linux32/libsteam_api.so"

if [ -z "$SteamAppId" ] && [ ! -e "steam_appid.txt" ]; then
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

export LD_PRELOAD="$STEAM_LIB${LD_PRELOAD+:$LD_PRELOAD}"
"$AIR_SDK/bin/adl" FRESteamWorksTest-linux.xml .
