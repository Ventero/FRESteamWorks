#!/bin/bash
FLEX_SDK=../../../FlexSDK

"$FLEX_SDK/bin/acompc" -swf-version 11 -source-path ../src \
                       -include-sources ../src \
                       -output FRESteamWorks.swc
