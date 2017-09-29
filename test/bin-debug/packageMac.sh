#!/bin/sh
set -x

[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

ANE_PATH=../../lib/bin

rm -rf FRESteamWorksTest.app

cp "$STEAM_SDK/redistributable_bin/steam_api.dll" .
cp "$STEAM_SDK/redistributable_bin/osx32/libsteam_api.dylib" .

# Generate cert on the fly
"$AIR_SDK/bin/adt" -certificate -cn MyCertificateName 2048-RSA \
                   CertificateTest.p12 test


"$AIR_SDK"/bin/adt -package -XnoAneValidate -tsa none -storetype pkcs12 \
                   -keystore CertificateTest.p12 -storepass test \
                   -target bundle FRESteamWorksTest FRESteamWorksTest-app.xml \
                   FRESteamWorksTest.swf \
                   steam_api.dll libsteam_api.dylib steam_appid.txt \
                   -extdir "$ANE_PATH"

cp FRESteamWorksTest.app/Contents/Resources/steam_appid.txt FRESteamWorksTest.app/Contents/MacOS/
rm -f steam_api.dll libsteam_api.dylib
