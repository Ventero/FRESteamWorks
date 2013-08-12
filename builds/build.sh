#!/bin/sh
set -xe
[ ! -e ../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../config.sh

die() {
	echo "$@"
	exit 1
}

target="$(git describe --tags --always)"

for type in Debug Release; do
	dir="$target/$type"
	[ ! -d "$dir" ] && die "Build directory $dir missing"
	pushd "$dir"

	[ ! -f "FRESteamWorks.framework" ] && die "$dir/FRESteamWorks.framework missing"
	[ ! -f "FRESteamWorks.dll" ] && die "$dir/FRESteamWorks.dll missing"

	install_name_tool -change \
	    "@loader_path/libsteam_api.dylib" "@rpath/../Resources/libsteam_api.dylib" \
	    "./FRESteamWorks.framework/FRESteamWorks"

	"$FLEX_SDK/bin/compc" +configname=air -source-path ../../../lib/src -optimize \
	                      -include-classes com.amanitadesign.steam.FRESteamWorks \
	                                       com.amanitadesign.steam.SteamConstants \
	                      -swf-version=11 -output FRESteamWorksLib.swc

	unzip -o FRESteamWorksLib.swc

	cp ../../../lib/bin/descriptor.xml .
	version="$(git describe --abbrev=0)"
	sed -E -i "" -e "s/<versionNumber>[^<]+/<versionNumber>${version##v}/" descriptor.xml

	"$AIR_SDK"/bin/adt -package -target ane FRESteamWorks.ane descriptor.xml \
	                   -swc FRESteamWorksLib.swc \
	                   -platform Windows-x86 library.swf FRESteamWorks.dll \
	                   -platform MacOS-x86 library.swf FRESteamWorks.framework \
	                   -platform default library.swf

	rm -f library.swf catalog.xml
	popd
done

pushd "$target"
cp Debug/FRESteamWorks.ane FRESteamWorks-Debug.ane
cp Release/FRESteamWorks.ane FRESteamWorks.ane
"$FLEX_SDK/bin/compc" +configname=air -source-path ../../lib/src_linux -optimize \
                      -include-classes com.amanitadesign.steam.FRESteamWorks \
                                       com.amanitadesign.steam.SteamConstants \
                                       com.amanitadesign.steam.SteamEvent \
                      -swf-version=11 -output FRESteamWorksLibLinux.swc
popd


if [ -n "$UPLOAD_URL" ]; then
	curl --ftp-create-dirs -nT \
	     "{$target/FRESteamWorks-Debug.ane,$target/FRESteamWorks.ane,$target/FRESteamWorksLibLinux.swc}" \
	     "$UPLOAD_URL/$target/"
fi
