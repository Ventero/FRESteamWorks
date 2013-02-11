AIR_SDK=../../../AdobeAIRSDK
STEAM_SDK=../../../SteamSDK

export LD_PRELOAD="$STEAM_SDK/redistributable_bin/linux32/libsteam_api.so"
export LD_LIBRARY_PATH="$HOME/.steam/bin"
"$AIR_SDK/bin/adl" FRESteamWorksTest-linux.xml .
