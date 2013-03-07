SET "AIR_SDK=%USERPROFILE%\Documents\AdobeAIRSDK"
set "STEAM_SDK=%USERPROFILE%\Documents\Steam\sdk"
SET ANE_PATH=..\..\lib\bin

set "PATH=%STEAM_SDK%\redistributable_bin;%PATH%"
"%AIR_SDK%\bin\adl.exe" -extdir "%ANE_PATH%" FRESteamWorksTest-app.xml
