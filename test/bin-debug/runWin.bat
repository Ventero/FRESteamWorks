@echo off
if not exist ..\..\config.bat (
    echo "FRESteamWorks/config.bat is not set up!"
    goto :eof
)
call ..\..\config.bat

set ANE_PATH=..\..\lib\bin
set "PATH=%STEAM_SDK%\redistributable_bin;%PATH%"
"%AIR_SDK%\bin\adl.exe" -extdir "%ANE_PATH%" FRESteamWorksTest-app.xml -- %*
