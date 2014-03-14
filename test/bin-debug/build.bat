@echo off
if not exist ..\..\config.bat (
    echo "FRESteamWorks/config.bat is not set up!"
    goto :eof
)
call ..\..\config.bat

call "%FLEX_SDK%/bin/amxmlc" -swf-version=11 ^
    -define=CONFIG::linux,false ^
    -external-library-path+=../../lib/bin/FRESteamWorks.ane ^
    -output FRESteamWorksTest.swf ../src/FRESteamWorksTest.as
