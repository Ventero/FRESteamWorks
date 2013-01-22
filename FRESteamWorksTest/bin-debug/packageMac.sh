#!/bin/sh
set -x

AIR_SDK=../../../AdobeAIRSDK
STEAM_SDK=../../../Steam/sdk
ANE_PATH=../../FRESteamWorksLib/bin

cp "$STEAM_SDK/redistributable_bin/steam_api.dll" .
cp "$STEAM_SDK/redistributable_bin/osx32/libsteam_api.dylib" .

"$AIR_SDK"/bin/adt -package -XnoAneValidate -tsa none -storetype pkcs12 \
                   -keystore CertificateTest.p12 -storepass test \
                   -target bundle FRESteamWorksTest FRESteamWorksTest-app.xml \
                   FRESteamWorksTest.swf \
                   steam_api.dll libsteam_api.dylib steam_appid.txt \
                   -extdir "$ANE_PATH"

rm -f steam_api.dll libsteam_api.dylib
