AIR_SDK=../../../AdobeAIRSDK
STEAM_SDK=../../../SteamSDK

export LD_PRELOAD="$STEAM_SDK/redistributable_bin/linux32/libsteam_api.so"
export LD_LIBRARY_PATH="$HOME/.steam/bin"

if [ -z "$SteamAppId" ] && [ ! -e "steam_appid.txt" ]; then
	echo "\$SteamAppId not set and steam_appid.txt not found"
	exit 1
fi

if [ ! -e "$LD_PRELOAD" ]; then
	echo "libsteam_api.so not found"
	exit 2
fi

if [ ! -e "$LD_LIBRARY_PATH/libsteam.so" ]; then
	echo "libsteam.so not found"
	exit 3
fi

"$AIR_SDK/bin/adl" FRESteamWorksTest-linux.xml .
