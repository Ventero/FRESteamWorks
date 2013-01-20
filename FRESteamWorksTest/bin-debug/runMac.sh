AIR_SDK=../../../AdobeAIRSDK

export DYLD_LIBRARY_PATH=.
"$AIR_SDK/bin/adl" -extdir ../../FRESteamWorksLib/bin/ FRESteamWorksTest-app.xml .
