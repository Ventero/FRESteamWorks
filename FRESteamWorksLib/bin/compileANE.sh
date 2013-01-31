#!/bin/sh
set -xe
AIR_SDK=../../../AdobeAIRSDK
FLEX_SDK=../../../FlexSDK

# FIXME: this seems wrong
install_name_tool -change \
    "@loader_path/libsteam_api.dylib" "@executable_path/../Resources/libsteam_api.dylib" \
    ./FRESteamWorks.framework/FRESteamWorks

"$FLEX_SDK/bin/compc" +configname=air -source-path ../src -optimize \
                      -include-classes com.amanitadesign.steam.FRESteamWorks \
                                       com.amanitadesign.steam.SteamConstants \
                      -swf-version=11 -output FRESteamWorksLib.swc

unzip -o FRESteamWorksLib.swc
trap "rm -f library.swf catalog.xml" EXIT

"$AIR_SDK"/bin/adt -package -target ane FRESteamWorks.ane descriptor.xml \
                   -swc FRESteamWorksLib.swc \
                   -platform Windows-x86 library.swf FRESteamWorks.dll \
                   -platform MacOS-x86 library.swf FRESteamWorks.framework

rm -rf FRESteamWorks.Unpacked.ane
unzip FRESteamWorks.ane -d FRESteamWorks.Unpacked.ane
