#!/bin/bash

FLEXSDK="../../../FlexSDK"
"$FLEXSDK/bin/amxmlc" -compiler.source-path=../../FRESteamWorksLib/src -output FRESteamWorksTest.swf ../src/FRESteamWorksTest.as
