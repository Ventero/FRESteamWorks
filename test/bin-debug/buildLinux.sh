#!/bin/bash

set -e

[ ! -e ../../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../../config.sh

make -C ../../src DEBUG=1 wrapper
mkdir -p NativeApps/Linux/
cp ../../src/APIWrapper/APIWrapper NativeApps/Linux/

"$FLEX_SDK/bin/amxmlc" -library-path+=../../lib/bin/FRESteamWorksLibLinux.swc \
                       -swf-version=11 -output FRESteamWorksTest_linux.swf \
                       -define=CONFIG::linux,true \
                       ../src/FRESteamWorksTest.as
