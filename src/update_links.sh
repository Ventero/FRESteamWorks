#!/bin/sh
set -xe
[ ! -e ../config.sh ] && die "FRESteamWorks/config.sh is not set up!"
. ../config.sh

ln -hsf "$AIR_SDK" AdobeAIRSDK
ln -hsf "$STEAM_SDK" SteamSDK
