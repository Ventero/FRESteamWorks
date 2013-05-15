#!/bin/bash
[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

"$FLEX_SDK/bin/amxmlc" -swf-version=11 -compiler.source-path=../../lib/src -output FRESteamWorksTest.swf ../src/FRESteamWorksTest.as
