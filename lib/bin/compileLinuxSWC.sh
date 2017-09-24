#!/bin/bash
set -xe

[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

"$FLEX_SDK/bin/acompc" -source-path ../src_linux \
                       -debug=false -optimize \
                       -include-sources ../src_linux/ \
                       -swf-version=11 -output FRESteamWorksLibLinux.swc
