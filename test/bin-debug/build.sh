#!/bin/bash
[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

"$FLEX_SDK/bin/amxmlc" -swf-version=11 \
                       -define=CONFIG::linux,false \
                       -external-library-path+=../../lib/bin/FRESteamWorks.ane \
                       -output FRESteamWorksTest.swf ../src/FRESteamWorksTest.as
