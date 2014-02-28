REM To set up your config on Windows, copy this file to config.bat and change
REM the values below accordingly. All paths must be absolute paths!

REM Path to the main AIR SDK directory, so that %AIR_SDK%\bin\adl exists.
set "AIR_SDK=%USERPROFILE%\Documents\AdobeAIRSDK"
REM Path to the FLEX SDK directory, so that %FLEX_SDK%\bin\compc exists.
REM Might be identical to AIR_SDK if you use an overlayed SDK.
set "FLEX_SDK=%ProgramFiles(x86)%\FlashDevelop\Tools\flexsdk"
REM Path to the Steam SDK, so that %STEAM_SDK%\redistributable_bin exists.
set "STEAM_SDK=%USERPROFILE%\Documents\Steam\sdk"
REM Path to the unzip binary. Only needed when building the ANE.
set "UNZIP_BIN=%ProgramFiles(x86)%\GnuWin32\bin\unzip"
