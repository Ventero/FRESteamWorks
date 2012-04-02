#!/bin/sh
set -x

AIR_SDK=/Developer/SDKs/AdobeAIRSDK
ANE_PATH=../../FRESteamWorksLib/bin

"$AIR_SDK"/bin/adt -package -XnoAneValidate -tsa none -storetype pkcs12 -keystore CertificateTest.p12 -storepass test -target bundle FRESteamWorksTest  FRESteamWorksTest-app.xml FRESteamWorksTest.swf steam_api.dll libsteam_api.dylib steam_appid.txt -extdir "$ANE_PATH"

mv ./FRESteamWorksTest.app/Contents/Resources/steam_appid.txt ./FRESteamWorksTest.app/Contents/MacOS/steam_appid.txt 