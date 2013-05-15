#!/bin/bash
set -xe

[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

"$FLEX_SDK/bin/compc" +configname=air -source-path ../src_linux -optimize \
                      -include-classes com.amanitadesign.steam.FRESteamWorks \
                                       com.amanitadesign.steam.SteamConstants \
                                       com.amanitadesign.steam.SteamEvent \
                      -swf-version=11 -output FRESteamWorksLibLinux.swc
