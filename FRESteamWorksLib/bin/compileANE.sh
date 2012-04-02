#!/bin/sh
set -x
AIR_SDK=/Developer/SDKs/AdobeAIRSDK

rm -R ./FRESteamWorks.framework
cp -R ../../FRESteamWorks/build/Release/ ./

install_name_tool -change \
    "@loader_path/libsteam_api.dylib" "@rpath/../Resources/libsteam_api.dylib" \
    ./FRESteamWorks.framework/FRESteamWorks

unzip -o FRESteamWorksLib.swc
"$AIR_SDK"/bin/adt -package -target ane FRESteamWorks.ane descriptor.xml -swc FRESteamWorksLib.swc -platform Windows-x86 library.swf FRESteamWorks.dll -platform MacOS-x86 library.swf FRESteamWorks.framework
rm library.swf
rm catalog.xml
