#!/bin/bash

[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

export DYLD_FALLBACK_LIBRARY_PATH="$STEAM_SDK/redistributable_bin/osx32"
export DYLD_INSERT_LIBRARIES=/Applications/Steam.app/Contents/MacOS/osx32/gameoverlayrenderer.dylib
"$AIR_SDK/bin/adl" -extdir ../../lib/bin/ FRESteamWorksTest-app.xml . -- "$@"
