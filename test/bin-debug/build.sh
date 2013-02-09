#!/bin/bash

FLEXSDK="../../../FlexSDK"
"$FLEXSDK/bin/amxmlc" -swf-version=11 -compiler.source-path=../../lib/src -output FRESteamWorksTest.swf ../src/FRESteamWorksTest.as
