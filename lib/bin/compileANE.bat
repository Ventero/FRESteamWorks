@echo off
if not exist ..\..\config.bat (
    echo "FRESteamWorks/config.bat is not set up!"
    goto :eof
)
call ..\..\config.bat

call "%FLEX_SDK%\bin\compc" +configname=air -source-path ..\src -optimize ^
                            -include-sources ..\src\ ^
                            -swf-version=11 -output FRESteamWorksLib.swc

"%UNZIP_BIN%" -o FRESteamWorksLib.swc

call "%AIR_SDK%\bin\adt" -package -target ane FRESteamWorks.ane descriptor.xml ^
                         -swc FRESteamWorksLib.swc ^
                         -platform Windows-x86 library.swf FRESteamWorks.dll ^
                         -platform MacOS-x86-64 library.swf FRESteamWorks.framework ^
                         -platform default library.swf

del library.swf
del catalog.xml

"%UNZIP_BIN%" -o FRESteamWorks.ane -d FRESteamWorks.Unpacked.ane
