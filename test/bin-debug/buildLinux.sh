#!/bin/bash

[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

"$FLEX_SDK/bin/amxmlc" -library-path+=../../lib/bin/FRESteamWorksLibLinux.swc \
                       -swf-version=11 -output FRESteamWorksTest_linux.swf \
                       ../src/FRESteamWorksTest.as
