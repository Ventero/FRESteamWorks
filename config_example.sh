# all paths must be absolute paths
# on Linux, remember to wrap all variables in ${}, so that they can
# be used in the Makefile
# path to the main AIR SDK directory, so that $AIR_SDK/bin/adt exists
AIR_SDK="${HOME}/Coding/AdobeAIRSDK"
# path to the Flex SDK, so that $FLEX_SDK/bin/compc exists
# might be identical to AIR_SDK
FLEX_SDK="${HOME}/Coding/FlexSDK"
# path to the Steam SDK, so that $STEAM_SDK/redistributable_bin exists
STEAM_SDK="${HOME}/Coding/SteamSDK"
# ftp path to upload the built binaries to when running builds/build.sh
# version number gets appended to the path. set it to an empty value to not
# upload the binaries.
UPLOAD_URL="ftp://example.org/FRESteamWorks"
