#!/bin/bash

[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

export DYLD_FALLBACK_LIBRARY_PATH="$STEAM_SDK/redistributable_bin/osx32"

# Ensure the overlay is added when running outside Steam
overlay_libs=(
    "/Library/Application Support/Steam/Steam.AppBundle/Steam/Contents/MacOS/gameoverlayrenderer.dylib"
    "/Applications/Steam.app/Contents/MacOS/gameoverlayrenderer.dylib"
)
for file in "${overlay_libs[@]}"; do
    if [ -f "$file" ]; then
        DYLD_INSERT_LIBRARIES="$file${DYLD_INSERT_LIBRARIES+:$DYLD_INSERT_LIBRARIES}"
    fi
done
export DYLD_INSERT_LIBRARIES

"$AIR_SDK/bin/adl" -extdir ../../lib/bin/ FRESteamWorksTest-app.xml . -- "$@"
