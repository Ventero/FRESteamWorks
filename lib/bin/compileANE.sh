#!/bin/sh
set -xe

[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

install_name_tool -change \
    "@loader_path/libsteam_api.dylib" "@rpath/../Resources/libsteam_api.dylib" \
    ./FRESteamWorks.framework/FRESteamWorks
# XCode on OS X Mavericks creates frameworks with a broken symlink
# Create that link's target so adt doesn't complain later on
mkdir -p FRESteamWorks.framework/Versions/Current/Resources

"$FLEX_SDK/bin/compc" +configname=air -source-path ../src -optimize \
                      -include-sources ../src/ \
                      -swf-version=11 -output FRESteamWorksLib.swc

unzip -o FRESteamWorksLib.swc
trap "rm -f library.swf catalog.xml" EXIT

if [ ! -f FRESteamWorks.dll ]; then
	echo "Creating dummy FRESteamWorks.dll"
	touch FRESteamWorks.dll
fi

"$AIR_SDK"/bin/adt -package -target ane FRESteamWorks.ane descriptor.xml \
                   -swc FRESteamWorksLib.swc \
                   -platform Windows-x86 library.swf FRESteamWorks.dll \
                   -platform MacOS-x86 library.swf FRESteamWorks.framework \
                   -platform default library.swf

rm -rf FRESteamWorks.Unpacked.ane
unzip FRESteamWorks.ane -d FRESteamWorks.Unpacked.ane
