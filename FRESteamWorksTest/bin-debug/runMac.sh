AIR_SDK=../../../AdobeAIRSDK
STEAM_SDK=../../../Steam/sdk

export DYLD_FALLBACK_LIBRARY_PATH="$STEAM_SDK/redistributable_bin/osx32"
"$AIR_SDK/bin/adl" -extdir ../../FRESteamWorksLib/bin/ FRESteamWorksTest-app.xml .
